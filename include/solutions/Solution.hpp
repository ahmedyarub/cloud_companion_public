#ifndef CLOUDCOMPANION_SOLUTION_HPP
#define CLOUDCOMPANION_SOLUTION_HPP

#include <actions/Action.hpp>

#include "QString"
#include "QVariantMap"

namespace CloudCompanion {
  class Solution {
  public:
    QString friendlyName;
    Action *actionObject;
    QString parameter;

    Solution(QString, Action *, QString);

    [[nodiscard]] QVariantMap getDisplay() const;
  };
}  // namespace CloudCompanion

#endif  // CLOUDCOMPANION_SOLUTION_HPP
