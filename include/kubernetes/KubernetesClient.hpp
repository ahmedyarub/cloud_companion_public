#ifndef CLOUDCOMPANIONSTANDALONE_KUBERNETESCLIENT_HPP
#define CLOUDCOMPANIONSTANDALONE_KUBERNETESCLIENT_HPP

extern "C" {
#include <kubernetes/include/list.h>
}

#include <logging/LogParserEnum.hpp>

#include "PodInfo.hpp"
#include "QList"
#include "QObject"

namespace CloudCompanion {
  class KubernetesClient : public QObject {
    Q_OBJECT

  public:
    static KubernetesClient &getInstance() {
      static KubernetesClient instance;
      return instance;
    }

    KubernetesClient(const KubernetesClient &) = delete;
    KubernetesClient &operator=(const KubernetesClient &) = delete;

    static QList<PodInfo> getPods(const std::string &basePath, const std::string &token);
    static void watchPods(const std::string &basePath, const std::string &token);
    static void deletePod(const std::string &basePath, const char *name, const std::string &token);
    static QString getPodLogs(const std::string &basePath, const char *name,
                              LogParserEnum logParser, const QString &jsonField,
                              const QString &csvDel, int csvCol, const std::string &token);

  private:
    KubernetesClient() = default;
    ~KubernetesClient() override = default;
    void static onPodEventReceived(const char *event_string);
    void static podWatchHandler(void **pData, long *pDataLen);
    static LogParserEnum detectLogType(std::string &message);
    static size_t nthSubstr(int n, const std::string &s, const std::string &p);
    static list_t *getApiKeys(const std::string &token);

  signals:
    void podAdded(PodInfo podInfo);
    void podModified(PodInfo podInfo);
    void podRemoved(QString podUuid);
  };
}  // namespace CloudCompanion

#endif  // CLOUDCOMPANIONSTANDALONE_KUBERNETESCLIENT_HPP
