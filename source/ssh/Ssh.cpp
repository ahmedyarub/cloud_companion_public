#include "ssh/Ssh.hpp"
// #include <QDebug>

int CloudCompanion::vsSshAuthenticate(ssh_session bs, const char* filename) {
  auto auth = -1;
  ssh_key privateKey;

  // qDebug() << "Loading private key";

  if (ssh_pki_import_privkey_file(filename, nullptr, nullptr, nullptr, &privateKey) == SSH_OK) {
    auth = ssh_userauth_publickey(bs, nullptr, privateKey);
  }

  return auth;
}

int CloudCompanion::executeSshCommand(ssh_session session) {
  //    char buffer[256];
  //    int nbytes;
  // qDebug() << QString("Executing SSH command: %1").arg("");

  auto* const channel = ssh_channel_new(session);
  if (channel == nullptr) return SSH_ERROR;

  auto rc = ssh_channel_open_session(channel);
  if (rc != SSH_OK) {
    ssh_channel_free(channel);
    return rc;
  }

  rc = ssh_channel_request_exec(
      channel,
      "$(aws ecr get-login --no-include-email) && docker run -d --name hello-world --publish "
      "8000:8080 infrastructureascode/hello-world:latest");
  if (rc != SSH_OK) {
    // qCritical() << QString("Command error: %1").arg(ssh_get_error(session));

    ssh_channel_close(channel);
    ssh_channel_free(channel);
    return rc;
  }

  /*
  nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);
  while (nbytes > 0) {
      if (fwrite(buffer, 1, nbytes, stdout) != (unsigned int) nbytes) {
          ssh_channel_close(channel);
          ssh_channel_free(channel);
          return SSH_ERROR;
      }
      nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);
  }

  if (nbytes < 0) {
      ssh_channel_close(channel);
      ssh_channel_free(channel);
      return SSH_ERROR;
  }


  system("pause");

  ssh_channel_send_eof(channel);
  ssh_channel_close(channel);
  ssh_channel_free(channel);
*/

  // qDebug() << "SSH command executed!";

  return SSH_OK;
}