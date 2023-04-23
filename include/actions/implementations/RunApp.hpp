#ifndef CLOUDCOMPANION_RUNAPP_HPP
#define CLOUDCOMPANION_RUNAPP_HPP

#include "actions/Action.hpp"

namespace CloudCompanion {
  class RunApp : public Action {
  public:
    RunApp();

    void run(QString) override;
  };
}

#endif //CLOUDCOMPANION_RUNAPP_HPP
