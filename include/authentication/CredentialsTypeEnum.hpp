#ifndef CLOUD_COMPANION_CREDENTIALSTYPEENUM_HPP
#define CLOUD_COMPANION_CREDENTIALSTYPEENUM_HPP

namespace CloudCompanion {
  enum CredentialsTypeEnum : int { DefaultAwsAuthentication, IdAndKey, STS, SAMLOkta };
}

#endif  // CLOUD_COMPANION_CREDENTIALSTYPEENUM_HPP
