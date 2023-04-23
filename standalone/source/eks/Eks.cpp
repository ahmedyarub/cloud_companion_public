#include "eks/Eks.hpp"

#include <QDebug>
#include <QtConcurrent>
#include <kubernetes/KubernetesClient.hpp>

#include "logging/LogParserEnum.hpp"
#include "sts/StsClient.h"

QList<CloudCompanion::PodInfo> CloudCompanion::Eks::getKubernetesPods() {
  const QSettings settings;
  auto basePath = settings.value("settings/kubernetes/kubernetesBasePath").toString().toStdString();

  return KubernetesClient::getPods(basePath, StsClient::getToken(basePath));
}

void CloudCompanion::Eks::watchKubernetesPods() {
  const QSettings settings;
  auto basePath = settings.value("settings/kubernetes/kubernetesBasePath").toString().toStdString();

  watchFuture = QtConcurrent::run([&basePath]() -> void {
    KubernetesClient::watchPods(basePath, StsClient::getToken(basePath));
  });
}

void CloudCompanion::Eks::stopWatchKubernetesPods() { watchFuture.cancel(); }

void CloudCompanion::Eks::deleteKubernetesPod(const QString& podName) {
  KubernetesClient::getInstance();
  const QSettings settings;
  auto basePath = settings.value("settings/kubernetes/kubernetesBasePath").toString().toStdString();

  KubernetesClient::deletePod(basePath, podName.toStdString().c_str(),
                              StsClient::getToken(basePath));
}

void CloudCompanion::
    Eks::getKubernetesPodLogs(const QString& podName) {
  KubernetesClient::getInstance();
  const QSettings settings;
  auto logParser
      = static_cast<LogParserEnum>(settings.value("settings/kubernetes/logParser").toInt());
  auto jsonField = settings.value("settings/kubernetes/jsonField").toString();
  auto csvCol = settings.value("settings/kubernetes/csvColumn").toInt();
  auto basePath = settings.value("settings/kubernetes/kubernetesBasePath").toString().toStdString();

  QString csvDel;
  switch (settings.value("settings/kubernetes/csvDelimiter").toInt()) {
    case 0:
      csvDel = ",";
      break;
    case 1:
      csvDel = "\t";
      break;
    case 2:
      csvDel = " ";
  }

  KubernetesClient::getPodLogs(basePath, podName.toStdString().c_str(), logParser, jsonField,
                               csvDel, csvCol, StsClient::getToken(basePath));
}