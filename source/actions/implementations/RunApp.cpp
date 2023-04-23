#include <actions/implementations/RunApp.hpp>
#include <stdexcept>

void CloudCompanion::RunApp::run(QString parameter) { throw std::logic_error("Not implemented!"); }

CloudCompanion::RunApp::RunApp() : Action("run_app", "Execute an external application") {}
