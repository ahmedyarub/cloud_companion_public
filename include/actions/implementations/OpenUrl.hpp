#ifndef CLOUDCOMPANION_OPENURL_HPP
#define CLOUDCOMPANION_OPENURL_HPP

#include "actions/Action.hpp"

namespace CloudCompanion {
  class OpenUrl : public Action {
  public:
    OpenUrl();

    void run(QString) override;
  };
}  // namespace CloudCompanion

#endif  // CLOUDCOMPANION_OPENURL_HPP
