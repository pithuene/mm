#include <libssh/libssh.h>
#include <libssh/sftp.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ssh.h"
#include "sftp.h"
#include "list.h"

sftp_session sftp_session_create(char * domain, char * user) {
    ssh_session session;
    session = connect_ssh(domain, user, 0);
    sftp_session sftp;
    int rc;
    sftp = sftp_new(session);
    rc = sftp_init(sftp);
    return sftp;
}

void sftp_session_free(sftp_session sftp) {
    sftp_free(sftp);
    ssh_disconnect(sftp->session);
    ssh_free(sftp->session);
    ssh_finalize();
}

int sftp_dir_foreach(
        sftp_session sftp,
        char * dir_path,
        void (*f) (sftp_attributes attr, void * any),
        void * any,
        int free_attributes
) {
    int rc;
    sftp_dir dir;
    sftp_attributes attributes;
    dir = sftp_opendir(sftp, dir_path);
    if(!dir) {
        fprintf(stderr, "Failed to open directory: %s\n", ssh_get_error(sftp->session));
        return SSH_ERROR;
    }

    while ((attributes = sftp_readdir(sftp, dir)) != NULL) {
        if (strcmp(attributes->name, ".") && strcmp(attributes->name, "..")) {
            (*f)(attributes, any);
            if (free_attributes) {
                sftp_attributes_free(attributes);
            }
        }
    }

    rc = sftp_closedir(dir);
    if(rc != SSH_OK) {
        fprintf(stderr, "Failed to close directory: %s\n", ssh_get_error(sftp->session));
        return rc;
    }
    return 0;
}

void list_append_attr(sftp_attributes attr, void * list) {
    list_append((struct list *) list, attr);
}

List sftp_dir_list(sftp_session sftp, char * dir_path) {
    List l = list_create();
    sftp_dir_foreach(sftp, dir_path, list_append_attr, l, 0);
    return l;
}

void list_append_name(sftp_attributes attr, void * list) {
    char * str = malloc(strlen(attr->name) + 1);
    strcpy(str, attr->name);
    list_append((struct list *) list, str);
}

List sftp_dir_list_names(sftp_session sftp, char * dir_path) {
    List l = list_create();
    sftp_dir_foreach(sftp, dir_path, list_append_name, l, 0);
    return l;
}
