#include <aws/core/utils/HashingUtils.h>
#include <aws/sts/STSClient.h>

#include <QDebug>

#include "authentication/AuthUtils.hpp"
#include "sts/StsClient.h"

std::string CloudCompanion::StsClient::getToken(const std::string& basePath) {
  std::string token;
  if (basePath.find("eks") != std::string::npos) {
    // TODO make this configurable
    Aws::Client::ClientConfiguration client_config;
    client_config.region = "us-west-2";
    client_config.verifySSL = false;

    Aws::STS::STSClient stsClient(AuthUtils::getCredentialsProvider("STSClient"), client_config);

    Aws::Http::URI uri("https://sts.us-west-2.amazonaws.com/");
    uri.AddQueryStringParameter("Action", "GetCallerIdentity");
    uri.AddQueryStringParameter("Version", "2011-06-15");

    Aws::Http::HeaderValueCollection headers;
    headers.emplace(Aws::Http::HeaderValuePair("x-k8s-aws-id", "eks-dev-gs"));

    auto presignedUrl = stsClient.GeneratePresignedUrl(uri, Aws::Http::HttpMethod::HTTP_GET,
                                                       "us-west-2", headers, 60);

    presignedUrl.insert(presignedUrl.find('?'), "/");

    qDebug() << presignedUrl.c_str();

    token = "k8s-aws-v1."
            + Aws::Utils::HashingUtils::Base64Encode(Aws::Utils::ByteBuffer(
                (unsigned char*)presignedUrl.c_str(), presignedUrl.length()));

    std::replace(token.begin(), token.end(), '=', '\0');
  }

  return token;
}
