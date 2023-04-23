#include <QDebug>
#include <gsl/gsl>

#include "kubernetes/KubernetesClient.hpp"
#include "models/EksPodsTableModel.hpp"

CloudCompanion::EksPodsTableModel::EksPodsTableModel(QObject *parent) noexcept
    : QAbstractTableModel(parent) {}

int CloudCompanion::EksPodsTableModel::rowCount(const QModelIndex &) const {
  return gsl::narrow<int>(pods.size());
}

int CloudCompanion::EksPodsTableModel::columnCount(const QModelIndex &) const noexcept { return 6; }

QVariant CloudCompanion::EksPodsTableModel::data(const QModelIndex &index, const int role) const {
  QString item;
  switch (role) {
    case name:
      item = pods.at(index.row()).name;
      break;
    case ns:
      item = pods.at(index.row()).nameSpace;
      break;
    case pod_ip:
      item = pods.at(index.row()).podIp;
      break;
    case created_at:
      item = pods.at(index.row()).created;
      break;
    case status:
      item = pods.at(index.row()).status;
      break;
    case actions:
      item = "Delete";
      break;
    default:
      item.clear();
  }

  return item;
}

QHash<int, QByteArray> CloudCompanion::EksPodsTableModel::roleNames() const {
  qDebug() << "Role names for EKS list model";

  return {{name, "eks_name"},     {ns, "eks_namespace"},
          {pod_ip, "eks_pod_ip"}, {created_at, "eks_created_at"},
          {status, "eks_status"}, {actions, "eks_actions"}};
}

QVariant CloudCompanion::EksPodsTableModel::headerData(const int section,
                                                       const Qt::Orientation orientation,
                                                       const int role) const {
  if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
    switch (section) {
      case 0:
        return QString("Name");
      case 1:
        return QString("Namespace");
      case 2:
        return QString("Pod IP");
      case 3:
        return QString("Created At");
      case 4:
        return QString("Status");
      case 5:
        return QString("Actions");
      default:
        return {};
    }
  }

  return {};
}

[[maybe_unused]] void CloudCompanion::EksPodsTableModel::btnRefreshPodsClick() {
  //    emit showMessage("Loading EC2 instances", 10000);

  // TODO check whether we are logging in to AWS
  beginResetModel();
  //  const Aws::SDKOptions options;
  //  InitAPI(options);

  pods = Eks::getKubernetesPods();

  //  ShutdownAPI(options);
  endResetModel();
}

[[maybe_unused]] void CloudCompanion::EksPodsTableModel::btnWatchPodsClick(bool watching) {
  if (watching) {
    if (!pods.empty()) removeRows(0, gsl::narrow<int>(pods.size()));

    //    emit showMessage("Loading EC2 instances", 10000);

    // TODO check whether we are logging in to AWS
    connect(&KubernetesClient::getInstance(), &KubernetesClient::podAdded, this,
            &EksPodsTableModel::podAdded);
    connect(&KubernetesClient::getInstance(), &KubernetesClient::podModified, this,
            &EksPodsTableModel::podModified);
    connect(&KubernetesClient::getInstance(), &KubernetesClient::podRemoved, this,
            &EksPodsTableModel::podRemoved);

    eks.watchKubernetesPods();
  } else {
    eks.stopWatchKubernetesPods();
  }
}

void CloudCompanion::EksPodsTableModel::podAdded(const PodInfo &podInfo) {
  beginResetModel();

  pods.append(podInfo);

  endResetModel();
}

void CloudCompanion::EksPodsTableModel::podModified(const PodInfo &podInfo) {
  beginResetModel();

  std::replace_if(
      pods.begin(), pods.end(),
      [&podInfo](auto podIterator) { return podIterator.uuid == podInfo.uuid; }, podInfo);

  endResetModel();
}

void CloudCompanion::EksPodsTableModel::podRemoved(QString podUuid) {
  beginResetModel();

  pods.removeIf([&podUuid](auto podIterator) { return podIterator.uuid == podUuid; });

  endResetModel();
}

[[maybe_unused]] void CloudCompanion::EksPodsTableModel::btnDelete(int index) {
  Eks::deleteKubernetesPod(pods.at(index).name);
}

[[maybe_unused]] void CloudCompanion::EksPodsTableModel::btnLog(int index) {
  Eks::getKubernetesPodLogs(pods.at(index).name);
}