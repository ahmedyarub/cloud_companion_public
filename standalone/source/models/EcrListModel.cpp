#include <aws/core/Aws.h>

#include <QDesktopServices>

#include "QDebug"
#include "ecr/Ecr.hpp"
#include "gsl/gsl"
#include "models/EcrListModel.hpp"

CloudCompanion::EcrListModel::EcrListModel(QObject *parent)
    : QAbstractListModel(parent), timer{this}, selectedIndex{-1} {
  connect(&timer, &QTimer::timeout, this, QOverload<>::of(&EcrListModel::updateList));
}

int CloudCompanion::EcrListModel::rowCount(const QModelIndex &) const {
  return gsl::narrow<int>(ecrImages.size());
}

QVariant CloudCompanion::EcrListModel::headerData(const int section,
                                                  const Qt::Orientation orientation,
                                                  const int role) const {
  qDebug() << QString("Header data for section %1 role %2").arg(section).arg(role);

  if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
    switch (section) {
      case 0:
        return QString("Instance ID");
      case 1:
        return QString("AMI");
      case 2:
        return QString("Instance Type");
      case 3:
        return QString("Status");
      case 4:
        return QString("Actions");
      default:
        return {};
    }
  }

  return {};
}

QHash<int, QByteArray> CloudCompanion::EcrListModel::roleNames() const {
  qDebug() << "Role names for ECR list model";

  return {{tag_role, "ecr_tag"}, {name_role, "ecr_name"}};
}

QVariant CloudCompanion::EcrListModel::data(const QModelIndex &index, const int role) const {
  qDebug() << QString("Data for role %1 row %2").arg(role).arg(index.row());

  switch (role) {
    case name_role:
      return ecrImages.at(index.row()).name;
    case tag_role:
      return ecrImages.at(index.row()).tag;
    default:
      return {};
  }
}

[[maybe_unused]] void CloudCompanion::EcrListModel::btnRefreshEcrClick() {
  // required for WASM
  //    std::thread t1(&EcrListModel::refreshEcr, this);
  //
  //    if (!timer.isActive())
  //        timer.start(10000);
  // END WASM

  emit showMessage("Loading ECR records", 10000);

  refreshEcr();

  updateList();
}

void CloudCompanion::EcrListModel::refreshEcr() {
  const Aws::SDKOptions options;
  InitAPI(options);

  ecrImages = describeImages();

  ShutdownAPI(options);
}

void CloudCompanion::EcrListModel::updateList() {
  beginResetModel();
  emit showMessage("Done!", 3000);
  emit ecrListUpdated(ecrImages);
  endResetModel();
}

[[maybe_unused]] void CloudCompanion::EcrListModel::btnBuildImageFromFile(
    QString dockerfile, const QString &imageName) {
  dockerfile.replace("file:///", "");

  qDebug() << QString("Building Docker image from %1").arg(dockerfile);

  dockerClient->buildDockerImage(dockerfile, imageName);
}

[[maybe_unused]] void CloudCompanion::EcrListModel::btnBuildImageFromGit(const QString &repoUrl,
                                                                         const QString &imageName) {
  qDebug() << QString("Building Docker image from Git repository %1").arg(repoUrl);

  dockerClient->buildDockerImageFromGit(repoUrl, imageName);
}

[[maybe_unused]] void CloudCompanion::EcrListModel::btnPushImage(const QString &imageName,
                                                                 const QString &repositoryUrl) {
  qDebug() << "Uploading image to ECR";

  dockerClient->pushDockerImage(imageName, repositoryUrl);
}

[[maybe_unused]] void CloudCompanion::EcrListModel::selectedIndexChanged(int index) {
  qDebug() << QString("ECR selected index: %1").arg(index);

  selectedIndex = index;
}

void CloudCompanion::EcrListModel::setDockerClient(DockerClient *_dockerClient) {
  EcrListModel::dockerClient = _dockerClient;
}

[[maybe_unused]] void CloudCompanion::EcrListModel::imageBuildTroubleshootAction(int action) {
  qDebug() << action;

  QDesktopServices::openUrl(QUrl("https://docs.docker.com/get-docker/"));
}

[[maybe_unused]] void CloudCompanion::EcrListModel::imagePushTroubleshootAction(int action) {
  qDebug() << action;

  dockerClient->pushDockerImage("672775987525.dkr.ecr.us-east-1.amazonaws.com/hello-world:latest",
                                "https://672775987525.dkr.ecr.us-east-1.amazonaws.com");
}
