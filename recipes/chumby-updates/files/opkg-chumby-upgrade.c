#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>

#include "libopkg/opkg.h"
#include "libopkg/opkg_cmd.h"
#include "libopkg/opkg_upgrade.h"
#include "libopkg/opkg_configure.h"

static int find_socket(char *name, char *out, int outsize)
{
    struct dirent *de;
    DIR *dir;
    int found = 0;
    char name_tmp[strlen(name)+3];

    dir = opendir("/tmp/");
    if (!dir)
        return 1;

    name_tmp[0] = '-';
    memcpy(name_tmp+1, name, sizeof(name_tmp)-3);
    name_tmp[sizeof(name_tmp)-2] = '-';
    name_tmp[sizeof(name_tmp)-1] = '\0';

    while (!found && (de = readdir(dir))) {
        if (strstr(de->d_name, name_tmp) && !strstr(de->d_name, "-lockfile")) {
            snprintf(out, outsize, "%s/%s", "/tmp/", de->d_name);
            found = 1;
        }
    }

    closedir(dir);
    return !found;
}


static int send_message(char *sig, char *message, int length)
{
    static int sock = -1;
    int len;
    struct sockaddr_un remote;

    if (-1 == sock) {
        sock = socket(AF_UNIX, SOCK_STREAM, 0);
        if (-1 == sock) {
            perror("Unable to create socket");
            return 1;
        }


        remote.sun_family = AF_UNIX;
        if (find_socket(sig, remote.sun_path, sizeof(remote.sun_path))) {
            fprintf(stderr, "Unable to locate socket: File not found\n");
            close(sock);
            return 1;
        }
        len = strlen(remote.sun_path) + sizeof(remote.sun_family);


        /*
         * Set socket to non-blocking, as the destination might go away
         * during the course of upgrading.
         */
        if (fcntl(sock, F_SETFL, O_NONBLOCK) < 0) {
            perror("Couldn't set socket to non-blocking");
        }


        if (connect(sock, (struct sockaddr *)&remote, len) == -1) {
            perror("Unable to conncet to socket");
            close(sock);
            return 1;
        }
    }

    if (send(sock, message, length, 0) == -1) {
        perror("Unable to send message to socket");
        close(sock);
        sock = -1;
        return 1;
    }

    return 0;
}

/* Based on libopkg/opkg_cmd:opkg_upgrade_cmd() */
static int
chumby_upgrade_cmd()
{
    int i, r, err = 0;
    int pkg_percent;
    pkg_t *pkg;
    pkg_vec_t *installed;
    char progress_message[50];
    int progress_message_length;
    
    installed = pkg_vec_alloc();

    pkg_info_preinstall_check();

    pkg_hash_fetch_all_installed(installed);
    pkg_percent = installed->len * 100;

    /* Install/upgrade the packages */
    for (i = 0; i < installed->len; i++) {
        pkg = installed->pkgs[i];


        progress_message_length = snprintf(progress_message+4,
                                    sizeof(progress_message)-4,
                                    "NeTVBrowser|~|UpdateProgress|~|%d|~|%s",
                                    (i*100)/installed->len,
                                    pkg->name) +5;
        /*
         * Put the string's length, plus the NULL, plus the length size into
         * the first four bytes
         */
        ((long *)progress_message_length)[0] = htonl(progress_message_length);
        send_message("NeTVBr", progress_message, progress_message_length);

        if (opkg_upgrade_pkg(pkg))
            err = -1;
    }

    /* Configure the packages */
    for (i = 0; i < installed->len; i++) {

        progress_message_length = snprintf(progress_message+4,
                                    sizeof(progress_message)-4,
                                    "NeTVBrowser|~|ConfigureProgress|~|%d|~|%s",
                                    (i*100)/installed->len,
                                    pkg->name) +5;
        /*
         * Put the string's length, plus the NULL, plus the length size into
         * the first four bytes
         */
        ((long *)progress_message_length)[0] = htonl(progress_message_length);
        send_message("NeTVBr", progress_message, progress_message_length);

        pkg = installed->pkgs[i];
        if (pkg->state_status == SS_UNPACKED) {
            r = opkg_configure(pkg);
            if (r == 0) {
                pkg->state_status = SS_INSTALLED;
                pkg->parent->state_status = SS_INSTALLED;
                pkg->state_flag &= ~SF_PREFER;
            } else {
                if (!err)
                    err = r;
            }
        }
    }

    pkg_vec_free(installed);

    progress_message_length = snprintf(progress_message+4,
                                sizeof(progress_message)-4,
                                "NeTVBrowser|~|UpgradeComplete");
    /*
     * Put the string's length, plus the NULL, plus the length size into
     * the first four bytes
     */
    ((long *)progress_message_length)[0] = htonl(progress_message_length);
    send_message("NeTVBr", progress_message, progress_message_length);

    return err;
}


int main(int argc, char **argv) {
    return chumby_upgrade_cmd();
}
