#ifndef CLOUD_COMPANION_OKTA_HPP
#define CLOUD_COMPANION_OKTA_HPP

#include <aws/core/auth/AWSCredentialsProvider.h>

#include <QFuture>
#include <QNetworkAccessManager>

namespace CloudCompanion {
  class Okta : public QObject {
    Q_OBJECT
  private:
    static QFuture<QString> primaryAuth(QString &samlUsername, QString &samlPassword);

    static QFuture<QString> getSessionToken(const QString &stateToken);

    static QFuture<QString> getSessionCookie(const QString &sessionToken);

    static QFuture<QString> getAsserts(const QString &sessionId);

    static QString extractSaml(const QString &html);

  public:
    static std::shared_ptr<Aws::Auth::AWSCredentialsProvider> oktaAuth(const char *allocationTag);
  };
}  // namespace CloudCompanion

#endif  // CLOUD_COMPANION_AUTHUTILS_HPP
