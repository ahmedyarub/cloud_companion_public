#ifndef CLOUDCOMPANION_CHECKCONNECTIVITY_HPP
#define CLOUDCOMPANION_CHECKCONNECTIVITY_HPP

#include "actions/Action.hpp"

namespace CloudCompanion {
  class CheckConnectivity : public Action {
  public:
    CheckConnectivity();

    void run(QString) override;
  };
}  // namespace CloudCompanion

#endif  // CLOUDCOMPANION_CHECKCONNECTIVITY_HPP
