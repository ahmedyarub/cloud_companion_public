#include "actions/Action.hpp"

CloudCompanion::Action::Action(QString name, QString actionFriendlyName)
    : name(std::move(name)), friendlyName(std::move(actionFriendlyName)) {}

const QString &CloudCompanion::Action::getName() const { return name; }
