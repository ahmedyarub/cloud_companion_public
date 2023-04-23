#ifndef CLOUDCOMPANIONSTANDALONE_STSCLIENT_H
#define CLOUDCOMPANIONSTANDALONE_STSCLIENT_H

#include <QString>

namespace CloudCompanion {
  class StsClient {
  public:
    static std::string getToken(const std::string& basePath);
  };
}  // namespace CloudCompanion

#endif  // CLOUDCOMPANIONSTANDALONE_STSCLIENT_H
