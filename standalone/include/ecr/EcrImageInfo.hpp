#ifndef CLOUD_COMPANION_ECR_IMAGE_INFO_HPP
#define CLOUD_COMPANION_ECR_IMAGE_INFO_HPP

#include <QtCore/qdatetime.h>
#include <QtCore/qstring.h>

namespace CloudCompanion {
  class EcrImageInfo {
  public:
    QString name;
    QString repositoryId;
    QString tag;
    QDateTime pushedAt;
    long long size = 0;
  };
}  // namespace CloudCompanion

#endif  // CLOUD_COMPANION_ECR_IMAGE_INFO_HPP
