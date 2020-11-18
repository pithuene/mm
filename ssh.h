#ifndef MM_SSH_H
#define MM_SSH_H

#include <libssh/libssh.h>

ssh_session connect_ssh(const char *host, const char *user,int verbosity);

#endif
