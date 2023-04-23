#include "ecr/Ecr.hpp"

#include <aws/ecr/ECRClient.h>
#include <aws/ecr/model/DescribeImagesRequest.h>
#include <aws/ecr/model/GetAuthorizationTokenRequest.h>

#include <QDebug>
#include <gsl/util>

#include "authentication/AuthUtils.hpp"

QVector<CloudCompanion::EcrImageInfo> CloudCompanion::describeImages() {
  Aws::Client::ClientConfiguration client_config;

  client_config.region = "us-east-1";
  client_config.verifySSL = false;

#ifdef EMSCRIPTEN
  client_config.proxyHost = "127.0.0.1";
  client_config.proxyPort = 7000;
  // This proxy scheme needs to be added to AWS C++ SDK before compilation
  client_config.proxyScheme = Aws::Http::Scheme::SOCKS5H;
#endif

  const Aws::ECR::ECRClient ecr(AuthUtils::getCredentialsProvider("ECRClient"), client_config);

  Aws::ECR::Model::DescribeImagesRequest describeImagesRequest;

  describeImagesRequest.SetRepositoryName("hello-world");

  auto done = false;
  QVector<EcrImageInfo> ecrImageInfo{};

  while (!done) {
    auto outcome = ecr.DescribeImages(describeImagesRequest);
    if (outcome.IsSuccess()) {
      const auto &describeImagesResult = outcome.GetResult();

      for (const auto &image : describeImagesResult.GetImageDetails()) {
        for (const auto &tag : image.GetImageTags()) {
          EcrImageInfo tmpEcrImageInfo;

          tmpEcrImageInfo.name = image.GetRepositoryName().c_str();
          tmpEcrImageInfo.repositoryId = image.GetRegistryId().c_str();
          tmpEcrImageInfo.tag = tag.c_str();
          tmpEcrImageInfo.pushedAt.setMSecsSinceEpoch(image.GetImagePushedAt().Millis());
          tmpEcrImageInfo.size = image.GetImageSizeInBytes();

          ecrImageInfo.push_back(std::move(tmpEcrImageInfo));
        }
      }
      if (!outcome.GetResult().GetNextToken().empty()) {
        describeImagesRequest.SetNextToken(outcome.GetResult().GetNextToken());
      } else {
        done = true;
      }
    } else {
      qCritical() << QString("Failed to describe ecr images: %1")
                         .arg(outcome.GetError().GetMessage().c_str());
      done = true;
    }
  }

  qDebug() << QString("Found: %1 image(s)!").arg(gsl::narrow_cast<uint32_t>(ecrImageInfo.size()));

  return ecrImageInfo;
}

QString CloudCompanion::getEcrToken() {
  const Aws::ECR::ECRClient ecr;

  const Aws::ECR::Model::GetAuthorizationTokenRequest request;

  auto outcome = ecr.GetAuthorizationToken(request);

  return outcome.GetResult().GetAuthorizationData().at(0).GetAuthorizationToken().c_str();
}