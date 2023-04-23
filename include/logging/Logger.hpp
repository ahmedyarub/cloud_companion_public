#ifndef CLOUD_COMPANION_LOGGER_HPP
#define CLOUD_COMPANION_LOGGER_HPP

#include <QtCore/QFileSystemWatcher>
#include <QtCore/QStringList>
#include <QtQml/QQmlEngine>
#include <QtQml/QtQml>

namespace CloudCompanion {
  class Logger : public QObject {
    Q_OBJECT

  public:
    explicit Logger(QObject *parent);

    ~Logger() override;

  public:
    static void registerSingleton();

    static void log(QtMsgType type, const QMessageLogContext &context, const QString &msg,
                    bool printRaw = false);
    static void clear();

  Q_SIGNALS:

    void newLog(QString s);
    void clearLog();
  };
}  // namespace CloudCompanion

#endif  // CLOUD_COMPANION_LOGGER_HPP
