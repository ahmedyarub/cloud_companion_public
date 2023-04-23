#ifndef CLOUD_COMPANION_SSH_HPP
#define CLOUD_COMPANION_SSH_HPP

#include <libssh/libssh.h>

namespace CloudCompanion {
  int vsSshAuthenticate(ssh_session bs, const char* filename);

  int executeSshCommand(ssh_session session);
}  // namespace CloudCompanion

#endif  // CLOUD_COMPANION_SSH_HPP
