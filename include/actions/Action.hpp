#ifndef CLOUDCOMPANION_ACTION_HPP
#define CLOUDCOMPANION_ACTION_HPP

#include <utility>

#include "QString"

namespace CloudCompanion {
  class Action {
    QString name;
    QString friendlyName;

  public:
    [[nodiscard]] const QString &getName() const;
    explicit Action(QString, QString);
    virtual void run(QString param) = 0;
  };
}  // namespace CloudCompanion

#endif  // CLOUDCOMPANION_ACTION_HPP
