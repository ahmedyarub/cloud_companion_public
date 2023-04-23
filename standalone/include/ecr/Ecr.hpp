#ifndef CLOUD_COMPANION_ECR_HPP
#define CLOUD_COMPANION_ECR_HPP

#include "EcrImageInfo.hpp"

namespace CloudCompanion {
  QVector<EcrImageInfo> describeImages();

  QString getEcrToken();
}  // namespace CloudCompanion

#endif  // CLOUD_COMPANION_ECR_HPP
