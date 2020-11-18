#ifndef MM_KNOWNHOST_H
#define MM_KNOWNHOST_H

#include <libssh/libssh.h>

int verify_knownhost(ssh_session session);

#endif
