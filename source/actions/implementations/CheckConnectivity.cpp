#include <actions/implementations/CheckConnectivity.hpp>
#include <stdexcept>

void CloudCompanion::CheckConnectivity::run(QString parameter) {
  throw std::logic_error("Not implemented!");
}

CloudCompanion::CheckConnectivity::CheckConnectivity()
    : Action("check_connectivity", "Check connectivity to host and port") {}
