#ifndef CLOUD_COMPANION_SSHHANDLER_HPP
#define CLOUD_COMPANION_SSHHANDLER_HPP

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>
#include <QTimer>

namespace CloudCompanion {
  class SshHandler : public QObject {
    Q_OBJECT

  public:
    explicit SshHandler(QObject *parent = nullptr) noexcept;

    [[maybe_unused]] Q_INVOKABLE void deployDockerImage(const QString &targetIp);

    void healthCheck();

  private slots:

    void replyFinished(QNetworkReply *reply);

  private:
    QTimer timer;
    QNetworkAccessManager mAccessManager;
  };
}  // namespace CloudCompanion

#endif  // CLOUD_COMPANION_SSHHANDLER_HPP
