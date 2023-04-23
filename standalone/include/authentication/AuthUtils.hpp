#ifndef CLOUD_COMPANION_AUTHUTILS_HPP
#define CLOUD_COMPANION_AUTHUTILS_HPP

#include <aws/core/auth/AWSCredentialsProvider.h>

namespace CloudCompanion {
  class AuthUtils {
  public:
    static std::shared_ptr<Aws::Auth::AWSCredentialsProvider> getCredentialsProvider(
        const char *allocationTag);
  };
}  // namespace CloudCompanion

#endif  // CLOUD_COMPANION_AUTHUTILS_HPP
