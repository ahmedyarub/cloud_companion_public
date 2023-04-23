#ifndef CLOUD_COMPANION_ECR_LIST_MODEL_HPP
#define CLOUD_COMPANION_ECR_LIST_MODEL_HPP

#include <QNetworkAccessManager>
#include <QTimer>
#include <QtCore/QAbstractTableModel>

#include "../docker/DockerClient.hpp"
#include "../ecr/EcrImageInfo.hpp"

namespace CloudCompanion {
  class EcrListModel final : public QAbstractListModel {
    enum ecr_attributes { name_role = Qt::UserRole + 1, tag_role };

    Q_OBJECT
  private:
    QTimer timer;
    QVector<EcrImageInfo> ecrImages;
    [[maybe_unused]] int selectedIndex;
    DockerClient* dockerClient;

  public:
    void setDockerClient(DockerClient* _dockerClient);

  public:
    explicit EcrListModel(QObject* parent = nullptr);

    [[nodiscard]] int rowCount(const QModelIndex&) const override;

    [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation,
                                      int role) const override;

    [[nodiscard]] QVariant data(const QModelIndex& index, int role) const override;

    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE [[maybe_unused]] void selectedIndexChanged(int index);

    void refreshEcr();

    void updateList();

  public slots:

    [[maybe_unused]] void btnRefreshEcrClick();

    [[maybe_unused]] void btnBuildImageFromFile(QString dockerfile, const QString& imageName);

    [[maybe_unused]] void btnBuildImageFromGit(const QString& repoUrl, const QString& imageName);

    [[maybe_unused]] void btnPushImage(const QString& imageName, const QString& repositoryUrl);

    [[maybe_unused]] static void imageBuildTroubleshootAction(int action);

    [[maybe_unused]] void imagePushTroubleshootAction(int action);

  signals:
    void ecrListUpdated(QVector<CloudCompanion::EcrImageInfo>);

    void showMessage(QString message, int timeout);
  };
}  // namespace CloudCompanion

#endif  // CLOUD_COMPANION_ECR_LIST_MODEL_HPP
