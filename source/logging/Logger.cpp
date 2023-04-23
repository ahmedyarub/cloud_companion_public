#include <logging/Logger.hpp>
#include <unordered_set>

static std::unordered_set<CloudCompanion::Logger *> loggers;

CloudCompanion::Logger::Logger(QObject *parent) : QObject(parent) { loggers.insert(this); }

CloudCompanion::Logger::~Logger() { loggers.erase(this); }

void CloudCompanion::Logger::registerSingleton() {
  qmlRegisterSingletonType<Logger>("CloudCompanionStandalone", 1, 0, "Logger",
                                   [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
                                     Q_UNUSED(scriptEngine)
                                     auto *instance = new Logger(engine);

                                     return instance;
                                   });
}

void CloudCompanion::Logger::log(QtMsgType type, const QMessageLogContext &context,
                                 const QString &msg, bool printRaw) {
  const auto category = [type]() -> QString {
    switch (type) {
      case QtDebugMsg:
        return "debug";
      case QtWarningMsg:
        return "warning";
      case QtCriticalMsg:
      case QtFatalMsg:
        return "error";
      case QtInfoMsg:
        return "info";
      default:
        return "unknown";
    }
  }();

  QString log;
  if (printRaw) {
    log = msg;
  } else {
    const auto timestamp = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
    log = "[" + timestamp + "] [" + context.category + "] [" + category + "] : " + msg;
  }

  for (auto *logger : loggers) {
    Q_EMIT logger->newLog(log);
  }
}

void CloudCompanion::Logger::clear() {
  for (auto *logger : loggers) {
    Q_EMIT logger->clearLog();
  }
}
