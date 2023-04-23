#ifndef CLOUDCOMPANIONSTANDALONE_WATCHOPERATION_H
#define CLOUDCOMPANIONSTANDALONE_WATCHOPERATION_H

#include <QObject>

namespace CloudCompanion {
  class WatchOperation : public QObject {
    Q_OBJECT

  public:
    enum Operation { ADDED = 0, MODIFIED = 1, DELETED = 2 };
    Q_ENUM(Operation)
  };
}  // namespace CloudCompanion

#endif  // CLOUDCOMPANIONSTANDALONE_WATCHOPERATION_H
