#ifndef CLOUD_COMPANION_EKS_HPP
#define CLOUD_COMPANION_EKS_HPP

#include <QtCore/qstring.h>
#include <aws/ec2/model/Instance.h>

#include <QFuture>
#include <QObject>
#include <QVector>
#include <kubernetes/KubernetesClient.hpp>
#include <kubernetes/PodInfo.hpp>

#include "InstanceInfo.hpp"

namespace CloudCompanion {
  class Eks : public QObject {
    Q_OBJECT
    QFuture<void> watchFuture;

  public:
    static QList<PodInfo> getKubernetesPods();
    void watchKubernetesPods();
    void stopWatchKubernetesPods();
    static void deleteKubernetesPod(const QString& podName);
    static void getKubernetesPodLogs(const QString& podName);
  };
}  // namespace CloudCompanion

#endif  // CLOUD_COMPANION_EKS_HPP
