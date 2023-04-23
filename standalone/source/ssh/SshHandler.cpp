#include "ssh/SshHandler.hpp"

#include <QSettings>

#include "QDebug"
#include "ssh/Ssh.hpp"

CloudCompanion::SshHandler::SshHandler(QObject* parent) noexcept : QObject(parent), timer{this} {
  connect(&timer, &QTimer::timeout, this, QOverload<>::of(&SshHandler::healthCheck));

  connect(&mAccessManager, &QNetworkAccessManager::finished, this, &SshHandler::replyFinished);
}

void CloudCompanion::SshHandler::replyFinished(QNetworkReply* reply) {
  const auto statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
  qDebug() << "Response network error" << reply->error();
  qDebug() << "Response HTTP status code" << statusCode;
  qDebug() << "Reply content:" << reply->readAll();
  reply->deleteLater();

  if (statusCode == 200) {
    qDebug() << "Service is healthy!";
    timer.stop();
  }
}

void CloudCompanion::SshHandler::healthCheck() {
  qDebug() << "Health check";

  const QNetworkRequest request(QUrl("http://52.205.211.46:8000/health"));
  mAccessManager.get(QNetworkRequest(request));
}

[[maybe_unused]] void CloudCompanion::SshHandler::deployDockerImage(const QString& targetIp) {
  const QSettings settings;

  auto verbosity = SSH_LOG_DEBUG;
  auto port = 22;

  // Open session and set options
  auto* const sshSession = ssh_new();
  if (sshSession == nullptr) exit(-1);
  ssh_options_set(sshSession, SSH_OPTIONS_HOST, targetIp.toStdString().c_str());
  ssh_options_set(sshSession, SSH_OPTIONS_USER, "ec2-user");
  ssh_options_set(sshSession, SSH_OPTIONS_LOG_VERBOSITY, &verbosity);
  ssh_options_set(sshSession, SSH_OPTIONS_PORT, &port);

  // Connect to server
  auto rc = ssh_connect(sshSession);
  if (rc != SSH_OK) {
    qCritical() << QString("Error connecting to remote host: %1").arg(ssh_get_error(sshSession));
    ssh_free(sshSession);
    return;
  }

  // Verify the server's identity
  //    if (verify_knownhost(sshSession) < 0) {
  //        ssh_disconnect(sshSession);
  //        ssh_free(sshSession);
  //        exit(-1);
  //    }

  // Authenticate ourselves
  rc = vsSshAuthenticate(
      sshSession,
      settings.value("settings/credentials/sshPrivateKey").toString().toStdString().c_str());

  if (rc != SSH_AUTH_SUCCESS) {
    qCritical() << QString("Authentication error: %1").arg(ssh_get_error(sshSession));

    ssh_disconnect(sshSession);
    ssh_free(sshSession);
    return;
  }

  executeSshCommand(sshSession);

  if (!timer.isActive()) timer.start(1000);
  //    ssh_disconnect(sshSession);
  //    ssh_free(sshSession);
}