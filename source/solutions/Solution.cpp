#include "solutions/Solution.hpp"

#include <utility>

CloudCompanion::Solution::Solution(QString solutionFriendlyName, Action *actionObject,
                                   QString actionParameter)
    : friendlyName(std::move(solutionFriendlyName)),
      actionObject(actionObject),
      parameter(std::move(actionParameter)) {}

QVariantMap CloudCompanion::Solution::getDisplay() const {
  QVariantMap result;

  result.insert("friendly_name", friendlyName);
  result.insert("parameter", parameter);
  result.insert("action", actionObject->getName());

  return result;
}
