#include <QDebug>
#include <QTimer>
#include <gsl/gsl>

#include "ec2/Ec2.hpp"
#include "models/InstancesTableModel.hpp"

CloudCompanion::InstancesTableModel::InstancesTableModel(QObject *parent) noexcept
    : QAbstractTableModel(parent), timer{this} {
  connect(&timer, &QTimer::timeout, this, QOverload<>::of(&InstancesTableModel::updateTimer));
}

void CloudCompanion::InstancesTableModel::updateTimer() {
  QVector<QString> instanceIds;

  for (auto const &instance : instances) {
    if (instance.instanceState == "pending" || instance.instanceState == "stopping"
        || instance.instanceState == "starting")
      instanceIds.push_back(instance.instanceId);
  }

  if (instanceIds.empty()) {
    timer.stop();

    emit showMessage("Done!", 3000);
    emit ec2ListUpdated(instances);

    return;
  }

  const Aws::SDKOptions options;
  InitAPI(options);

  beginResetModel();
  auto updatedInstances = describeInstances(instanceIds);

  ShutdownAPI(options);

  for (auto const &updatedInstance : updatedInstances) {
    qsizetype i = 0;

    while (instances.at(i).instanceId != updatedInstance.instanceId) i++;

    instances[i].instanceState = updatedInstance.instanceState;
  }

  endResetModel();
}

int CloudCompanion::InstancesTableModel::rowCount(const QModelIndex &) const {
  return gsl::narrow<int>(instances.size());
}

int CloudCompanion::InstancesTableModel::columnCount(const QModelIndex &) const noexcept {
  return 5;
}

QVariant CloudCompanion::InstancesTableModel::data(const QModelIndex &index, const int role) const {
  QString item;
  switch (role) {
    case instance_id:
      item = instances.at(index.row()).instanceId;
      break;
    case ami:
      item = instances.at(index.row()).imageId;
      break;
    case instance_type:
      item = instances.at(index.row()).type;
      break;
    case status:
      item = instances.at(index.row()).instanceState;
      break;
    case actions:
      if (instances.at(index.row()).instanceState == "running")
        item = "Stop";
      else if (instances.at(index.row()).instanceState == "stopped")
        item = "Start";
      else
        item = "Loading";
      break;
    default:
      item.clear();
  }

  return item;
}

QHash<int, QByteArray> CloudCompanion::InstancesTableModel::roleNames() const {
  qDebug() << "Role names for EC2 list model";

  return {{instance_id, "ec2_instance_id"},
          {ami, "ec2_ami"},
          {instance_type, "ec2_instance_type"},
          {status, "ec2_status"},
          {actions, "ec2_actions"}};
}

QVariant CloudCompanion::InstancesTableModel::headerData(const int section,
                                                         const Qt::Orientation orientation,
                                                         const int role) const {
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

[[maybe_unused]] void CloudCompanion::InstancesTableModel::btnRefreshInstancesClick() {
  if (!instances.empty()) removeRows(0, gsl::narrow<int>(instances.size()));

  emit showMessage("Loading EC2 instances", 10000);

  if (!timer.isActive()) timer.start(5000);

  beginResetModel();

  //        const QString instanceName = "instance1";
  //        const QString ami_id = "ami-0fc61db8544a617ed"; //Amazon Linux 2 AMI (HVM), SSD Volume
  //        Type

  const Aws::SDKOptions options;
  InitAPI(options);

  instances = describeInstances();
  // createInstance(instanceName, ami_id);
  // startInstance("i-01d055526ab534e30");
  // stopInstance("i-01d055526ab534e30");

  ShutdownAPI(options);

  endResetModel();
}

[[maybe_unused]] void CloudCompanion::InstancesTableModel::btnStartStopClick(const qsizetype index) {
  auto &instanceInfo = instances[index];

  const Aws::SDKOptions options;
  InitAPI(options);

  if (instanceInfo.instanceState == "stopped")
    startInstance(instanceInfo.instanceId);
  else if (instanceInfo.instanceState == "running")
    stopInstance(instanceInfo.instanceId);

  instanceInfo.instanceState = "pending";

  ShutdownAPI(options);
}

[[maybe_unused]] void CloudCompanion::InstancesTableModel::btnSelectInstanceClick(const qsizetype index) {
  emit ec2Selected(instances[index].publicIp);
}
