#ifndef CLOUD_COMPANION_POD_INFO_HPP
#define CLOUD_COMPANION_POD_INFO_HPP

#include <QtCore/qstring.h>

namespace CloudCompanion {
  class PodInfo {
  public:
    QString name;
    QString nameSpace;
    QString podIp;
    QString created;
    QString uuid;
    QString status;
  };
}  // namespace CloudCompanion

#endif  // CLOUD_COMPANION_POD_INFO_HPP
