#include "QJsonArray"
#include "QJsonDocument"
#include "QJsonObject"
#include "solutions/SolutionsRepo.hpp"

CloudCompanion::SolutionsRepo::SolutionsRepo(QFile solutionsFile,
                                               QMap<QString, Action *> action_names) {
  if (!solutionsFile.open(QIODevice::ReadOnly)) {
    qWarning("Couldn't load solutions file.");
    return;
  }

  QByteArray solutionsData = solutionsFile.readAll();

  QJsonDocument solutionsJson(QJsonDocument::fromJson(solutionsData));

  for (auto problem : solutionsJson.object()["problems"].toArray()) {
    QString problem_name = problem.toObject()["name"].toString();

    for (auto solution : problem.toObject()["solutions"].toArray()) {
      problemsSolutions.insert(
          problem_name,
          ::CloudCompanion::Solution(solution.toObject()["friendly_name"].toString(),
                                     action_names[solution.toObject()["action"].toString()],
                                     solution.toObject()["param"].toString()));
    }
  }
}

[[maybe_unused]] QVariantList CloudCompanion::SolutionsRepo::getSolutions(const QString &problem) {
  QVariantList result;

  for (auto &solution : problemsSolutions.values(problem)) {
    result.append(solution.getDisplay());
  }

  return result;
}

[[maybe_unused]] void CloudCompanion::SolutionsRepo::runAction(const QString &problem,
                                                                const QString &solutionName) {
  QList<Solution> solutions = problemsSolutions.values(problem);

  auto currentSolution = std::find_if(
      solutions.begin(), solutions.end(),
      [&solutionName](const Solution &solution) { return solution.friendlyName == solutionName; });

  if (currentSolution != std::end(solutions)) {
    currentSolution->actionObject->run(currentSolution->parameter);
  } else {
    // action not found
  }
}