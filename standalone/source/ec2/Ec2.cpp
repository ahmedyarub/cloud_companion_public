#include "ec2/Ec2.hpp"

#include <aws/ec2/EC2Client.h>
#include <aws/ec2/model/CreateTagsRequest.h>
#include <aws/ec2/model/DescribeInstancesRequest.h>
#include <aws/ec2/model/RunInstancesRequest.h>
#include <aws/ec2/model/StartInstancesRequest.h>
#include <aws/ec2/model/StopInstancesRequest.h>

#include <QDebug>
#include <algorithm>
#include <gsl/util>

#include "authentication/AuthUtils.hpp"

QVector<CloudCompanion::InstanceInfo> CloudCompanion::describeInstances(
    const QVector<QString> &instanceIds) {
  Aws::Client::ClientConfiguration client_config;

  // TODO make this configurable
  client_config.region = "us-west-2";
  client_config.verifySSL = false;

  const Aws::EC2::EC2Client ec2(AuthUtils::getCredentialsProvider("EC2Client"), client_config);
  Aws::EC2::Model::DescribeInstancesRequest describeInstancesRequest;

  if (!instanceIds.empty()) {
    Aws::Vector<Aws::String> instanceIdsTmp;
    instanceIdsTmp.reserve(instanceIds.size());

    std::transform(
        std::begin(instanceIds), std::end(instanceIds), std::back_inserter(instanceIdsTmp),
        [](const auto &instanceId) { return static_cast<Aws::String>(instanceId.toStdString()); });

    describeInstancesRequest.SetInstanceIds(instanceIdsTmp);
  }

  auto done = false;

  QVector<InstanceInfo> ec2_instances{};

  while (!done) {
    auto outcome = ec2.DescribeInstances(describeInstancesRequest);
    if (outcome.IsSuccess()) {
      const auto &result = outcome.GetResult();
      const auto &reservations = result.GetReservations();

      for (const auto &reservation : reservations) {
        const auto &instances = reservation.GetInstances();
        for (const auto &instance : instances) {
          InstanceInfo tmpInstance;

          tmpInstance.instanceState
              = Aws::EC2::Model::InstanceStateNameMapper::GetNameForInstanceStateName(
                    instance.GetState().GetName())
                    .c_str();

          tmpInstance.type = Aws::EC2::Model::InstanceTypeMapper::GetNameForInstanceType(
                                 instance.GetInstanceType())
                                 .c_str();

          tmpInstance.name = "Unknown";

          const auto &tags = instance.GetTags();
          auto nameIter = std::find_if(
              tags.cbegin(), tags.cend(),
              [](const Aws::EC2::Model::Tag &tag) { return tag.GetKey() == "Name"; });
          if (nameIter != tags.cend()) {
            tmpInstance.name = nameIter->GetValue().c_str();
          }

          tmpInstance.instanceId = instance.GetInstanceId().c_str();
          tmpInstance.imageId = instance.GetImageId().c_str();
          tmpInstance.publicIp = instance.GetPublicIpAddress().c_str();

          ec2_instances.push_back(std::move(tmpInstance));
        }
      }

      if (!outcome.GetResult().GetNextToken().empty()) {
        describeInstancesRequest.SetNextToken(result.GetNextToken());
      } else {
        done = true;
      }
    } else {
      qCritical() << QString("Failed to describe ec2 instances: %1")
                         .arg(outcome.GetError().GetMessage().c_str());
      done = true;
    }
  }

  // qInfo() << QString("Found: %1 instance(s)!").arg(gsl::narrow<uint32_t>(ec2_instances.size()));

  return ec2_instances;
}

void CloudCompanion::startInstance(const QString &instance_id) {
  const Aws::EC2::EC2Client ec2(AuthUtils::getCredentialsProvider("EC2Client"));

  Aws::EC2::Model::StartInstancesRequest start_request;
  start_request.AddInstanceIds(instance_id.toStdString().c_str());
  start_request.SetDryRun(true);

  const auto dry_run_outcome = ec2.StartInstances(start_request);
  assert(!dry_run_outcome.IsSuccess());
  if (dry_run_outcome.GetError().GetErrorType() != Aws::EC2::EC2Errors::DRY_RUN_OPERATION) {
    qCritical() << QString("Failed dry run to start instance %1: %2")
                       .arg(instance_id)
                       .arg(dry_run_outcome.GetError().GetMessage().c_str());
    return;
  }

  start_request.SetDryRun(false);
  const auto start_instancesOutcome = ec2.StartInstances(start_request);

  if (!start_instancesOutcome.IsSuccess()) {
    qWarning() << QString("Failed to start instance %1: %2")
                      .arg(instance_id)
                      .arg(start_instancesOutcome.GetError().GetMessage().c_str());
  } else {
    qInfo() << QString("Successfully started instance %1").arg(instance_id);
  }
}

void CloudCompanion::stopInstance(const QString &instance_id) {
  const Aws::EC2::EC2Client ec2(AuthUtils::getCredentialsProvider("EC2Client"));
  Aws::EC2::Model::StopInstancesRequest request;
  request.AddInstanceIds(instance_id.toStdString().c_str());
  request.SetDryRun(true);

  const auto dry_run_outcome = ec2.StopInstances(request);
  assert(!dry_run_outcome.IsSuccess());

  if (dry_run_outcome.GetError().GetErrorType() != Aws::EC2::EC2Errors::DRY_RUN_OPERATION) {
    qCritical() << QString("Failed dry run to stop instance %1: %2")
                       .arg(instance_id)
                       .arg(dry_run_outcome.GetError().GetMessage().c_str());
    return;
  }

  request.SetDryRun(false);
  const auto outcome = ec2.StopInstances(request);
  if (!outcome.IsSuccess()) {
    qCritical() << QString("Failed to stop instance %1: %2")
                       .arg(instance_id)
                       .arg(outcome.GetError().GetMessage().c_str());
  } else {
    qInfo() << QString("Successfully stopped instance %1").arg(instance_id);
  }
}

void CloudCompanion::createInstance(const QString &instanceName, const QString &ami_id) {
  const Aws::EC2::EC2Client ec2(AuthUtils::getCredentialsProvider("EC2Client"));

  Aws::EC2::Model::RunInstancesRequest run_request;
  run_request.SetImageId(ami_id.toStdString().c_str());
  run_request.SetInstanceType(Aws::EC2::Model::InstanceType::t1_micro);
  run_request.SetMinCount(1);
  run_request.SetMaxCount(1);

  auto run_outcome = ec2.RunInstances(run_request);
  if (!run_outcome.IsSuccess()) {
    qCritical() << QString("Failed to start ec2 instance %1 based on AMI %2: %3")
                       .arg(instanceName)
                       .arg(ami_id)
                       .arg(run_outcome.GetError().GetMessage().c_str());
    return;
  }

  const auto &instances = run_outcome.GetResult().GetInstances();
  if (instances.empty()) {
    qCritical() << QString("Failed to start ec2 instance %1 based on AMI %2: %3")
                       .arg(instanceName)
                       .arg(ami_id)
                       .arg(run_outcome.GetError().GetMessage().c_str());
    return;
  }

  const auto instance_id = instances[0].GetInstanceId();

  Aws::EC2::Model::Tag name_tag;
  name_tag.SetKey("Name");
  name_tag.SetValue(instanceName.toStdString().c_str());

  Aws::EC2::Model::CreateTagsRequest create_request;
  create_request.AddResources(instance_id);
  create_request.AddTags(name_tag);

  const auto create_outcome = ec2.CreateTags(create_request);
  if (!create_outcome.IsSuccess()) {
    qCritical() << QString("Failed to tag ec2 instance %1 with name %2: %3")
                       .arg(instance_id.c_str())
                       .arg(instanceName)
                       .arg(create_outcome.GetError().GetMessage().c_str());
    return;
  }

  qInfo() << QString("Successfully started ec2 instance %1 based on AMI %2")
                 .arg(instanceName)
                 .arg(ami_id);
}