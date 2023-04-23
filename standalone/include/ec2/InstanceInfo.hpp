#ifndef CLOUD_COMPANION_INSTANCE_INFO_HPP
#define CLOUD_COMPANION_INSTANCE_INFO_HPP

#include <QtCore/qstring.h>
#include <aws/core/Aws.h>

namespace CloudCompanion {
  class InstanceInfo {
  public:
    QString name;
    QString instanceId;
    QString imageId;
    QString type;
    QString instanceState;
    QString publicIp;
  };
}  // namespace CloudCompanion

#endif  // CLOUD_COMPANION_INSTANCE_INFO_HPP
