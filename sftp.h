#ifndef MM_SFTP_H
#define MM_SFTP_H

#include <libssh/sftp.h>

#include "list.h"

sftp_session sftp_session_create(char * domain, char * user);
void sftp_session_free(sftp_session sftp);
int sftp_dir_foreach(
    sftp_session sftp,
    char * dir_path,
    void (*f) (sftp_attributes attr, void * any),
    void * any,
    int free_attributes
);
List sftp_dir_list(sftp_session sftp, char * dir_path);
List sftp_dir_list_names(sftp_session sftp, char * dir_path);

#endif

