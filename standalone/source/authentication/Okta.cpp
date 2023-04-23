#include "authentication/Okta.hpp"

#include <aws/core/auth/AWSCredentialsProviderChain.h>
#include <aws/sts/STSClient.h>
#include <aws/sts/model/AssumeRoleWithSAMLRequest.h>
#include <qeventloop.h>

#include <QNetworkCookie>
#include <QtCore>

#include "QDebug"
#include "QJsonArray"
#include "QJsonDocument"
#include "QJsonObject"
#include "QNetworkCookieJar"
#include "QNetworkReply"
#include "QTextDocument"
#include "QtConcurrentRun"

QFuture<QString> CloudCompanion::Okta::primaryAuth(QString &samlUsername, QString &samlPassword) {
  qDebug() << "primaryAuth1";
  QEventLoop loop;
  QNetworkAccessManager networkAccessManager;

  QNetworkRequest authRequest(QUrl("https://zwift.okta.com/api/v1/authn"));
  authRequest.setHeader(QNetworkRequest::KnownHeaders::ContentTypeHeader, "application/json");

  QSharedPointer<QNetworkReply> reply(networkAccessManager.post(
      authRequest,
      QString(R"({"username": ")" + samlUsername + R"(", "password": ")" + samlPassword + R"("})")
          .toStdString()
          .c_str()));

  auto future = QtFuture::connect(reply.get(), &QNetworkReply::finished).then([&reply, &loop] {
    loop.quit();
    qDebug() << "primaryAuth2";
    auto primaryAuthReply = reply->readAll();

    qDebug() << primaryAuthReply;

    QJsonDocument primaryAuthJSON(QJsonDocument::fromJson(primaryAuthReply));

    return primaryAuthJSON.object()["stateToken"].toString();
  });

  loop.exec();
  return future;
}

QFuture<QString> CloudCompanion::Okta::getSessionToken(const QString &stateToken) {
  qDebug() << "getSessionToken1";

  return QtConcurrent::run([&stateToken]() {
    QEventLoop loop;
    QNetworkAccessManager networkAccessManager;

    qDebug() << "getSessionToken2";
    QFuture<QString> factorResult;
    QString next("https://zwift.okta.com/api/v1/authn/factors/opf6geh7s7SaVypQW357/verify");
    QJsonDocument mfaChallengeJSON;

    qDebug() << "getSessionToken3";
    QNetworkRequest mfaStatusRequest((QUrl(next)));
    mfaStatusRequest.setHeader(QNetworkRequest::KnownHeaders::ContentTypeHeader,
                               "application/json");

    do {
      QSharedPointer<QNetworkReply> reply(networkAccessManager.post(
          mfaStatusRequest,
          QString(R"({"stateToken": ")" + stateToken + "\"}").toStdString().c_str()));

      factorResult = QtFuture::connect(reply.get(), &QNetworkReply::finished)
                         .then(QtFuture::Launch::Async, [reply, &mfaChallengeJSON, &next, &loop] {
                           qDebug() << "getSessionToken4";
                           auto mfaChallengeReply = reply->readAll();

                           qDebug() << mfaChallengeReply;

                           mfaChallengeJSON = QJsonDocument::fromJson(mfaChallengeReply);

                           next = mfaChallengeJSON.object()["_links"]
                                      .toObject()["next"]
                                      .toObject()["href"]
                                      .toString();

                           loop.quit();

                           if (mfaChallengeJSON.object()["status"].toString() == "SUCCESS") {
                             return mfaChallengeJSON.object()["sessionToken"].toString();
                           } else {
                             return QString();
                           }
                         });

      loop.exec();

      factorResult.waitForFinished();

      QThread::msleep(3000);
    } while (!factorResult.result().isEmpty());

    qDebug() << "getSessionToken5";

    return factorResult.result();
  });
}

QFuture<QString> CloudCompanion::Okta::getSessionCookie(const QString &sessionToken) {
  qDebug() << "getSessionCookie1";
  QEventLoop loop;
  QNetworkAccessManager networkAccessManager;

  QNetworkRequest sessionCookieRequest(QUrl("https://zwift.okta.com/api/v1/sessions"));
  sessionCookieRequest.setHeader(QNetworkRequest::KnownHeaders::ContentTypeHeader,
                                 "application/json");

  QSharedPointer<QNetworkReply> reply(networkAccessManager.post(
      sessionCookieRequest,
      QString(R"({"sessionToken": ")" + sessionToken + "\"}").toStdString().c_str()));

  auto future = QtFuture::connect(reply.get(), &QNetworkReply::finished).then([&reply, &loop] {
    loop.quit();
    qDebug() << "getSessionCookie2";
    auto sessionCookieReply = reply->readAll();

    qDebug() << sessionCookieReply;

    QJsonDocument sessionCookieJSON(QJsonDocument::fromJson(sessionCookieReply));

    return sessionCookieJSON.object()["id"].toString();
  });

  loop.exec();
  return future;
}

QString CloudCompanion::Okta::extractSaml(const QString &html) {
  QRegularExpression regExp(R"(SAMLResponse\" type=\"hidden\" value=\"(.+?)\"\/>)");

  qDebug() << html;

  auto match = regExp.match(html);

  if (match.hasMatch()) {
    QTextDocument text;
    text.setHtml(match.captured(1));

    qDebug() << "Matched string";
    qDebug() << text.toPlainText();

    return text.toPlainText();
  } else {
    return "";
  }
}

QFuture<QString> CloudCompanion::Okta::getAsserts(const QString &sessionId) {
  qDebug() << "getAsserts1";

  return QtConcurrent::run([&sessionId]() {
    QEventLoop loop;
    QNetworkAccessManager networkAccessManager;

    QList<QNetworkCookie> cookies;
    QNetworkCookie sessionCookie("sid", sessionId.toUtf8());
    cookies.append(sessionCookie);
    qDebug() << sessionId;
    qDebug() << "getAsserts2";
    QFuture<QString> cookieResult;

    QString cookieUrl("http://zwift.okta.com/home/amazon_aws/0oak5p56EuENPg2Pf355/272");
    QString htmlReply;

    do {
      QNetworkRequest htmlRequest((QUrl(cookieUrl)));
      htmlRequest.setHeader(QNetworkRequest::CookieHeader, QVariant::fromValue(cookies));

      QSharedPointer<QNetworkReply> reply(networkAccessManager.get(htmlRequest));

      cookieResult = QtFuture::connect(reply.get(), &QNetworkReply::finished)
                         .then([reply, &loop, &cookieUrl, &htmlReply] {
                           qDebug() << "getAsserts3";
                           htmlReply = reply->readAll();

                           qDebug() << htmlReply;

                           int httpStatus
                               = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

                           loop.quit();

                           if (httpStatus == 302) {
                             cookieUrl = reply->header(QNetworkRequest::LocationHeader).toString();

                             qDebug() << cookieUrl;

                             return cookieUrl;
                           } else {
                             return QString();
                           }
                         });

      loop.exec();

      cookieResult.waitForFinished();

      QThread::msleep(3000);
      qDebug() << "getAsserts4";
    } while (!cookieResult.result().isEmpty());

    return extractSaml(htmlReply);
  });
}

std::shared_ptr<Aws::Auth::AWSCredentialsProvider> CloudCompanion::Okta::oktaAuth(
    const char *allocationTag) {
  QSettings settings;

  auto accessKey = settings.value("settings/credentials/okta/accessKeyId").toString().toStdString();
  auto secretKey = settings.value("settings/credentials/okta/secretKey").toString().toStdString();
  auto sessionToken
      = settings.value("settings/credentials/okta/sessionToken").toString().toStdString();
  auto tokenExpiry = settings.value("settings/credentials/okta/expiration").toLongLong();

  if (accessKey.length() == 0 || secretKey.length() == 0 || sessionToken.length() == 0
      || tokenExpiry < Aws::Utils::DateTime::CurrentTimeMillis()) {
    auto samlUsername = settings.value("settings/credentials/samlUsername").toString();
    auto samlPassword = settings.value("settings/credentials/samlPassword").toString();
    auto roleArn = settings.value("settings/credentials/samlRoleArn").toString();
    auto principalArn = settings.value("settings/credentials/samlPrincipalArn").toString();

    auto future
        = primaryAuth(samlUsername, samlPassword)
              .then(QtFuture::Launch::Async,
                    [](const QFuture<QString> &stateToken) {
                      return getSessionToken(stateToken.result()).result();
                    })
              .then(QtFuture::Launch::Async,
                    [](const QFuture<QString> &oktaSessionToken) {
                      return getSessionCookie(oktaSessionToken.result()).result();
                    })
              .then(QtFuture::Launch::Async,
                    [](const QFuture<QString> &sessionId) {
                      return getAsserts(sessionId.result()).result();
                    })
              .then(QtFuture::Launch::Async,
                    [&roleArn, &principalArn](const QFuture<QString> &asserts) {
                      // Get STS credentials
                      Aws::STS::Model::AssumeRoleWithSAMLRequest request;

                      request.SetRoleArn(roleArn.toStdString().c_str());
                      request.SetPrincipalArn(principalArn.toStdString().c_str());
                      request.SetSAMLAssertion(asserts.result().toStdString().c_str());

                      Aws::STS::STSClient stsClient;

                      return stsClient.AssumeRoleWithSAML(request).GetResult().GetCredentials();
                    });

    future.waitForFinished();
    auto credentials = future.result();

    settings.setValue("settings/credentials/okta/accessKeyId",
                      (accessKey = credentials.GetAccessKeyId()).c_str());
    settings.setValue("settings/credentials/okta/secretKey",
                      (secretKey = credentials.GetSecretAccessKey()).c_str());
    settings.setValue("settings/credentials/okta/sessionToken",
                      (sessionToken = credentials.GetSessionToken()).c_str());

    settings.setValue("settings/credentials/okta/expiration",
                      (qlonglong)credentials.GetExpiration().Millis());
  }

  return Aws::MakeShared<Aws::Auth::SimpleAWSCredentialsProvider>(
      allocationTag, accessKey.c_str(), secretKey.c_str(), sessionToken.c_str());
}