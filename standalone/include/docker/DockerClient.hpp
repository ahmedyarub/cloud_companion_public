#ifndef CLOUD_COMPANION_DOCKERCLIENT_HPP
#define CLOUD_COMPANION_DOCKERCLIENT_HPP

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <solutions/SolutionsRepo.hpp>

namespace CloudCompanion {
  class DockerClient : public QObject {
    Q_OBJECT

    SolutionsRepo &solutionsRepo;

  public:
    explicit DockerClient(SolutionsRepo &);

    void buildDockerImage(const QString &dockerfile, const QString &imageName);

    void pushDockerImage(const QString &imageName, const QString &registryUrl);

    void buildDockerImageFromGit(const QString &gitRepo, const QString &imageName);

  private:
    QNetworkAccessManager mAccessManager;

    void pushImageFinished(QNetworkReply *reply);

    void buildImageFinished(QNetworkReply *reply);

  signals:
    void showMessage(QString message, int timeout);

    void troubleshootPushImage(QString problem);
  };
}  // namespace CloudCompanion

#endif  // CLOUD_COMPANION_DOCKERCLIENT_HPP
