#ifndef CLOUD_COMPANION_EC2_HPP
#define CLOUD_COMPANION_EC2_HPP

#include <QtCore/qstring.h>
#include <aws/ec2/model/Instance.h>

#include <QVector>

#include "InstanceInfo.hpp"

namespace CloudCompanion {
  QVector<InstanceInfo> describeInstances(const QVector<QString> &instanceIds = QVector<QString>());

  void createInstance(const QString &instanceName, const QString &ami_id);

  void startInstance(const QString &instance_id);

  void stopInstance(const QString &instance_id);
}  // namespace CloudCompanion

#endif  // CLOUD_COMPANION_EC2_HPP
