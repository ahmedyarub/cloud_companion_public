#include "docker/DockerClient.hpp"

#include <aws/core/Aws.h>

#include <QFile>

#include "QDebug"
#include "ecr/Ecr.hpp"

CloudCompanion::DockerClient::DockerClient(SolutionsRepo &solutionsRepo)
    : solutionsRepo(solutionsRepo) {}

void CloudCompanion::DockerClient::buildDockerImage(const QString &dockerfile,
                                                    const QString &imageName) {
  QNetworkRequest request(QUrl(QString("http://localhost:2375/build?t=%1").arg(imageName)));

  request.setHeader(QNetworkRequest::KnownHeaders::ContentTypeHeader, "application/x-tar");

  auto file = new QFile(dockerfile);
  if (file->open(QIODevice::ReadOnly)) {
    connect(&mAccessManager, &QNetworkAccessManager::finished, this,
            &DockerClient::buildImageFinished);
    mAccessManager.post(QNetworkRequest(request), file);
  }

  qDebug() << file->errorString();
}

void CloudCompanion::DockerClient::buildDockerImageFromGit(const QString &gitRepo,
                                                           const QString &imageName) {
  emit showMessage("Building Docker image", 60000);

  const QNetworkRequest request(
      QUrl(QString("http://localhost:2375/build?remote=%1&dockerfile=amd64.Dockerfile&t=%2")
               .arg(gitRepo, imageName)));

  connect(&mAccessManager, &QNetworkAccessManager::finished, this,
          &DockerClient::buildImageFinished);
  mAccessManager.post(QNetworkRequest(request), QByteArray());
}

void CloudCompanion::DockerClient::buildImageFinished(QNetworkReply *reply) {
  disconnect(&mAccessManager, &QNetworkAccessManager::finished, this,
             &DockerClient::buildImageFinished);

  qDebug() << reply->error();

  switch (reply->error()) {
    case QNetworkReply::ConnectionRefusedError:
    case QNetworkReply::TimeoutError:
      qDebug() << "Connection timeout!";

      emit showMessage("Failed!", 3000);

      emit solutionsRepo.openTroubleshooter("docker_not_found");

      break;
    default:
      emit showMessage("Done!", 3000);

      QString log;
      while ((log = reply->readLine()) > nullptr) qDebug() << log;

      const auto statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

      qDebug() << "Response HTTP status code" << statusCode;
      // qDebug() << "Reply content:" << reply->readAll();
      reply->deleteLater();

      if (statusCode == 200) {
        qDebug() << "Finished building Docker image";
      }
  }
}

void CloudCompanion::DockerClient::pushDockerImage(const QString &imageName,
                                                   const QString &registryUrl) {
  emit showMessage("Pushing image to ECR", 60000);

  qDebug() << "Creating ECR access token";

  const Aws::SDKOptions options;
  InitAPI(options);

  const auto token = getEcrToken();

  ShutdownAPI(options);

  qDebug() << "Uploading image";

  qDebug() << imageName;

  qDebug() << registryUrl;

  QNetworkRequest request(QUrl(QString("http://localhost:2375/images/%1/push")
                                   .arg(static_cast<QString>(QUrl::toPercentEncoding(imageName)))));

  request.setRawHeader(
      "X-Registry-Auth",
      QString(R"({"username": "AWS","password": "%1","serveraddress": "%2"})")
          .arg(QString(QByteArray::fromBase64(token.toUtf8())).replace("AWS:", ""), registryUrl)
          .toLatin1()
          .toBase64());

  connect(&mAccessManager, &QNetworkAccessManager::finished, this,
          &DockerClient::pushImageFinished);
  mAccessManager.post(QNetworkRequest(request), "");
}

void CloudCompanion::DockerClient::pushImageFinished(QNetworkReply *reply) {
  disconnect(&mAccessManager, &QNetworkAccessManager::finished, this,
             &DockerClient::buildImageFinished);

  qDebug() << reply->error();

  switch (reply->error()) {
    case QNetworkReply::NoError: {
      emit showMessage("Done!", 3000);

      auto log = QString(reply->readAll());

      qDebug() << log;

      if (!log.contains("errorDetail")) {
        const auto statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

        qDebug() << "Response HTTP status code" << statusCode;
        // qDebug() << "Reply content:" << reply->readAll();
        reply->deleteLater();

        if (statusCode == 200) {
          qDebug() << "Image pushed to ECR successfully!";
        }
        break;
      }
    }
    default: {
      emit showMessage("Failed!", 3000);

      emit troubleshootPushImage("docker_push_error");
    }
  }
}
