#include <QtGui/QDesktopServices>
#include <actions/implementations/OpenUrl.hpp>
#ifndef __ANDROID__
#  include <QUrl>
#endif

void CloudCompanion::OpenUrl::run(QString parameter) {
#ifndef __ANDROID__
  QDesktopServices::openUrl(QUrl(parameter));
#endif
}

CloudCompanion::OpenUrl::OpenUrl() : Action("open_url", "Open URL") {}
