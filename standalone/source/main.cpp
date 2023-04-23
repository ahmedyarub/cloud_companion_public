#include <cloudcompanion/version.h>

#include <QGuiApplication>
#include <QLoggingCategory>
#include <Qaterial/Qaterial.hpp>
#include <QtQml>
#include <logging/Logger.hpp>
#include <models/EksPodsTableModel.hpp>

#include "actions/ActionDiscovery.hpp"
#include "models/EcrListModel.hpp"
#include "models/InstancesTableModel.hpp"
#include "ssh/SshHandler.hpp"

#ifdef Q_OS_WIN
#  include <Windows.h>
#endif

namespace CloudCompanion {
  Q_LOGGING_CATEGORY(CLOUDCOMPANIONSTANDALONE_MAIN_LOGGING_CATEGORY, "CloudCompanionStandalone")
}  // namespace CloudCompanion

int main(int argc, char **argv) {
  // qInstallMessageHandler(qtMsgOutput);

  qDebug("QLibraryInfo::path(QLibraryInfo::PluginsPath) %s",
         qPrintable(QLibraryInfo::path(QLibraryInfo::PluginsPath)));

  QGuiApplication app(argc, argv);
  QQmlApplicationEngine engine;

  QGuiApplication::setOrganizationName("Mind Power");
  QGuiApplication::setOrganizationDomain("mindpower.com");
  QGuiApplication::setApplicationName("Cloud Companion");
  QGuiApplication::setApplicationVersion(CLOUDCOMPANION_VERSION);

  qCDebug(CloudCompanion::CLOUDCOMPANIONSTANDALONE_MAIN_LOGGING_CATEGORY,
          "Register Application: "
          "Organization: %s; "
          "Domain: %s; "
          "ApplicationName: %s; "
          "Version: %s",
          qPrintable(QGuiApplication::organizationName()),
          qPrintable(QGuiApplication::organizationDomain()),
          qPrintable(QGuiApplication::applicationName()),
          qPrintable(QGuiApplication::applicationVersion()));

  CloudCompanion::SolutionsRepo solutionsRepo(QFile("solutions.json"),
                                               CloudCompanion::getActions());
  CloudCompanion::SshHandler sshHandler;

  CloudCompanion::InstancesTableModel tableModel;
  CloudCompanion::EksPodsTableModel eksPodsModel;
  CloudCompanion::DockerClient dockerClient(solutionsRepo);
  CloudCompanion::EcrListModel ecrListModel;
  ecrListModel.setDockerClient(&dockerClient);

  auto *rootContext = engine.rootContext();
  rootContext->setContextProperty("solutions_repo", &solutionsRepo);
  rootContext->setContextProperty("eks_pods_table_model", &eksPodsModel);
  rootContext->setContextProperty("instances_table_model", &tableModel);
  rootContext->setContextProperty("ecr_list_model", &ecrListModel);
  rootContext->setContextProperty("ssh_handler", &sshHandler);
  rootContext->setContextProperty("docker_client", &dockerClient);

  engine.addImportPath("qrc:/");
  qaterial::loadQmlResources();
  qaterial::registerQmlTypes();
  CloudCompanion::Logger::registerSingleton();

  Q_INIT_RESOURCE(CloudCompanionStandalone);

  qCInfo(CloudCompanion::CLOUDCOMPANIONSTANDALONE_MAIN_LOGGING_CATEGORY,
         "Qml Engine Load Main.qml");
  engine.load(QUrl("qrc:/CloudCompanionStandalone/Main.qml"));

  if (engine.rootObjects().isEmpty()) {
    qCWarning(CloudCompanion::CLOUDCOMPANIONSTANDALONE_MAIN_LOGGING_CATEGORY,
              "Error : Failed to load Main.qml");
    return -1;
  }
  qCInfo(CloudCompanion::CLOUDCOMPANIONSTANDALONE_MAIN_LOGGING_CATEGORY, "Start Qt Event Loop");

  return QGuiApplication::exec();
}

#if TARGET_OS_IPHONE
extern "C" int qt_main_wrapper(int argc, char *argv[],
                               int (*f)(int, char **));  // this function is inside libqios.dylib
extern "C" int main_wrapper(int argc, char **argv) {     // this is the entry point
  return qt_main_wrapper(argc, argv,
                         main);  // once finished, it will call into the real "main()" above
}
#endif