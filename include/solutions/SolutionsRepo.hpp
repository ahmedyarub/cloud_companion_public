#ifndef CLOUDCOMPANION_SOLUTIONS_REPO_HPP
#define CLOUDCOMPANION_SOLUTIONS_REPO_HPP

#include <QVariantMap>
#include <actions/Action.hpp>
#include <solutions/Solution.hpp>

#include "QFile"
#include "QObject"

namespace CloudCompanion {
  class SolutionsRepo : public QObject {
    Q_OBJECT

    QMultiMap<QString, Solution> problemsSolutions;

  public:
    explicit SolutionsRepo(QFile, QMap<QString, Action*>);

    Q_INVOKABLE [[maybe_unused]] QVariantList getSolutions(const QString& problem);

    Q_INVOKABLE [[maybe_unused]] void runAction(const QString& problem,
                                                const QString& solutionName);

  signals:
    void openTroubleshooter(QString problem);
  };
}  // namespace CloudCompanion

#endif  // CLOUDCOMPANION_SOLUTIONS_REPO_HPP
