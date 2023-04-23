#include "authentication/AuthUtils.hpp"

#include <aws/core/auth/AWSCredentialsProviderChain.h>

#include <QSettings>
#include <QtCore>
#include <authentication/Okta.hpp>

#include "authentication/CredentialsTypeEnum.hpp"

std::shared_ptr<Aws::Auth::AWSCredentialsProvider>
CloudCompanion::AuthUtils::getCredentialsProvider(const char *allocationTag) {
  const QSettings settings;

  switch (settings.value("settings/credentials/type").toInt()) {
    case IdAndKey:
      return Aws::MakeShared<Aws::Auth::SimpleAWSCredentialsProvider>(
          allocationTag,
          static_cast<Aws::String>(
              settings.value("settings/credentials/accessKeyId").toString().toStdString()),
          static_cast<Aws::String>(
              settings.value("settings/credentials/secretKey").toString().toStdString()));
    case STS:
      return Aws::MakeShared<Aws::Auth::SimpleAWSCredentialsProvider>(
          allocationTag,
          static_cast<Aws::String>(
              settings.value("settings/credentials/accessKeyId").toString().toStdString()),
          static_cast<Aws::String>(
              settings.value("settings/credentials/secretKey").toString().toStdString()),
          static_cast<Aws::String>(
              settings.value("settings/credentials/sessionToken").toString().toStdString()));

    case SAMLOkta:
      return Okta::oktaAuth(allocationTag);
  }

  return Aws::MakeShared<Aws::Auth::DefaultAWSCredentialsProviderChain>(allocationTag);
}