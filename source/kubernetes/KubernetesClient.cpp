#include "kubernetes/KubernetesClient.hpp"

#include <kubernetes/watch/watch_util.h>

#include <logging/Logger.hpp>
#include <sstream>

#define WATCH_EVENT_KEY_TYPE "type"
#define WATCH_EVENT_KEY_OBJECT "object"

extern "C" {
#include <kubernetes/api/CoreV1API.h>
#include <kubernetes/config/kube_config.h>
#include <kubernetes/config/kube_config_common.h>
}

#ifdef _MSC_VER
#define strdup _strdup
#endif

void CloudCompanion::KubernetesClient::onPodEventReceived(const char *event_string) {
  static char fname[] = "process_one_watch_event()";

  if (!event_string) {
    return;
  }

  std::string type;
  v1_pod_t *pod;

  const char *parse_end = nullptr;
  cJSON *event_json_obj = cJSON_ParseWithOpts(event_string, &parse_end, 1);
  if (event_json_obj) {
    cJSON *json_value_type = cJSON_GetObjectItem(event_json_obj, WATCH_EVENT_KEY_TYPE);
    if (json_value_type && json_value_type->type == cJSON_String) {
      type = json_value_type->valuestring;

      cJSON *json_value_object = cJSON_GetObjectItem(event_json_obj, WATCH_EVENT_KEY_OBJECT);
      if (json_value_object && json_value_object->type == cJSON_Object) {
        pod = v1_pod_parseFromJSON(json_value_object);
        if (pod) {
          PodInfo podInfo;
          podInfo.uuid = pod->metadata->uid;
          podInfo.name = pod->metadata->name;
          podInfo.nameSpace = pod->metadata->_namespace;
          podInfo.podIp = pod->status->pod_ip;
          podInfo.created = pod->metadata->creation_timestamp;
          podInfo.status = pod->status->phase;

          if (type == "ADDED") {
            emit KubernetesClient::getInstance().podAdded(podInfo);
          } else if (type == "MODIFIED") {
            emit KubernetesClient::getInstance().podModified(podInfo);
          } else if (type == "DELETED") {
            emit KubernetesClient::getInstance().podRemoved(pod->metadata->uid);
          } else {
            fprintf(stderr, "%s: Unknown command type.\n", type.c_str());
          }

          v1_pod_free(pod);
        } else {
          fprintf(stderr, "%s: Cannot get pod from watch event object.\n", fname);
        }
      } else {
        fprintf(stderr, "%s: Cannot get object in watch event.\n", fname);
      }
    } else {
      fprintf(stderr, "%s: Cannot get type in watch event.\n", fname);
    }

    cJSON_Delete(event_json_obj);
  } else {
    fprintf(stderr, "%s: Cannot create JSON from string: [%s].\n", fname, parse_end);
  }
}

void CloudCompanion::KubernetesClient::podWatchHandler(void **pData, long *pDataLen) {
  kubernets_watch_handler(pData, pDataLen, onPodEventReceived);
}

QList<CloudCompanion::PodInfo> CloudCompanion::KubernetesClient::getPods(
    const std::string &basePath, const std::string &token) {
  QList<PodInfo> pods;
  apiClient_t *apiClient;
  sslConfig_t *sslConfig = nullptr;
  auto *apiKeys = getApiKeys(token);

  apiClient = apiClient_create_with_base_path(basePath.c_str(), nullptr, apiKeys);

  v1_pod_list_t *pod_list;
  char ns[] = "kube-system";

  pod_list = CoreV1API_listNamespacedPod(apiClient, ns, nullptr, 0, nullptr, nullptr, nullptr, 0,
                                         nullptr, nullptr, 0, 0);

  listEntry_t *podListEntry;
  list_ForEach(podListEntry, pod_list->items) {
    auto pod = static_cast<v1_pod_t *>(podListEntry->data);

    PodInfo podInfo;
    podInfo.name = pod->metadata->name;
    podInfo.nameSpace = pod->metadata->_namespace;
    podInfo.podIp = pod->status->pod_ip;
    podInfo.created = pod->metadata->creation_timestamp;
    podInfo.status = pod->status->phase;

    pods.push_back(podInfo);
  }

  v1_pod_list_free(pod_list);

  apiClient_free(apiClient);
  if (!token.empty()) {
    free_client_config(const_cast<char *>(basePath.c_str()), sslConfig, apiKeys);
  }
  apiClient_unsetupGlobalEnv();

  return pods;
}

void CloudCompanion::KubernetesClient::watchPods(const std::string &basePath,
                                                 const std::string &token) {
  apiClient_t *apiClient;

  apiClient = apiClient_create_with_base_path(basePath.c_str(), nullptr, getApiKeys(token));
  apiClient->data_callback_func = &KubernetesClient::podWatchHandler;

  char ns[] = "kube-system";

  CoreV1API_listNamespacedPod(apiClient, ns, nullptr, 0, nullptr, nullptr, nullptr, 0, nullptr,
                              nullptr, 0, 1);

  apiClient_free(apiClient);
  //  free_client_config(basePath, sslConfig, apiKeys);
  apiClient_unsetupGlobalEnv();
}
void CloudCompanion::KubernetesClient::deletePod(const std::string &basePath, const char *name,
                                                 const std::string &token) {
  apiClient_t *apiClient
      = apiClient_create_with_base_path(basePath.c_str(), nullptr, getApiKeys(token));
  char ns[] = "kube-system";

  CoreV1API_deleteNamespacedPod(apiClient, const_cast<char *>(name), ns, nullptr, nullptr, 0, 0,
                                nullptr, nullptr);

  apiClient_free(apiClient);
  //  free_client_config(basePath, sslConfig, apiKeys);
  apiClient_unsetupGlobalEnv();
}

QString CloudCompanion::KubernetesClient::getPodLogs(const std::string &basePath, const char *name,
                                                     LogParserEnum logParser,
                                                     const QString &jsonField,
                                                     const QString &csvDel, int csvCol,
                                                     const std::string &token) {
  apiClient_t *apiClient;
  apiClient = apiClient_create_with_base_path(basePath.c_str(), nullptr, getApiKeys(token));
  char ns[] = "kube-system";

  std::string message = CoreV1API_readNamespacedPodLog(
      apiClient, const_cast<char *>(name), ns, nullptr, 0, 0, 100000, nullptr, 0, 360000, 20, 0);

  if (logParser == LogParserEnum::Auto) {
    logParser = detectLogType(message);
  }

  Logger::clear();
  std::string line;
  std::stringstream ss(message);
  while (std::getline(ss, line, '\n')) {
    QString parsedLine;

    if (logParser == LogParserEnum::JSON) {
      QJsonDocument messageJSON(QJsonDocument::fromJson(line.c_str()));
      parsedLine = messageJSON.object()[jsonField].toString();
    } else if (logParser == LogParserEnum::CSV) {
      auto start = nthSubstr(csvCol, line, csvDel.toStdString());

      if (start == std::string::npos) {
        qDebug() << "CSV column not found!";
        continue;
      }

      parsedLine = line.substr(start, line.length() - start).c_str();
    } else {
      parsedLine = line.c_str();
    }

    QMessageLogContext context;
    Logger::log(QtMsgType::QtInfoMsg, context, parsedLine, true);
  }

  qDebug() << message;

  apiClient_free(apiClient);
  //  free_client_config(basePath, sslConfig, apiKeys);
  apiClient_unsetupGlobalEnv();

  return "";
}

list_t *CloudCompanion::KubernetesClient::getApiKeys(const std::string &token) {
  list_t *apiKeys = nullptr;

  if (!token.empty()) {
    apiKeys = list_createList();
    char tokenValue[BEARER_TOKEN_BUFFER_SIZE];
    memset(tokenValue, 0, sizeof(tokenValue));
    snprintf(tokenValue, BEARER_TOKEN_BUFFER_SIZE, BEARER_TOKEN_TEMPLATE, token.c_str());
    keyValuePair_t *keyPairToken = keyValuePair_create((char *)AUTH_TOKEN_KEY, strdup(tokenValue));
    list_addElement(apiKeys, keyPairToken);
  }

  return apiKeys;
}

size_t CloudCompanion::KubernetesClient::nthSubstr(int n, const std::string &s,
                                                   const std::string &p) {
  int cur_n;
  size_t last_i = s.find(p);

  for (cur_n = 1; cur_n < n;) {
    auto found_i = s.find(p, last_i + 1);

    if (found_i == std::string::npos) {
      return -1;
    }

    if (found_i - last_i > 1) {
      cur_n++;
    }

    last_i = found_i;
  }

  return last_i;
}

CloudCompanion::LogParserEnum CloudCompanion::KubernetesClient::detectLogType(
    std::string &message) {
  if (!message.empty() && message.at(0) == '{') {
    return LogParserEnum::JSON;
  } else if (message.find('\t') != std::string::npos) {
    return LogParserEnum::CSV;
  } else {
    return LogParserEnum::Text;
  }
}
