#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <signal.h>
#include <strings.h>
#include <stdlib.h>

#include "libopkg/opkg.h"
#include "libopkg/opkg_cmd.h"
#include "libopkg/opkg_upgrade.h"
#include "libopkg/opkg_configure.h"
#include "libopkg/opkg_conf.h"

static int debug = 0;

void sigpipe_happened(int sig) {
    if (debug)
        fprintf(stderr, "Got sigpipe\n");
}



void
sprintf_alloc(char **str, const char *fmt, ...)
{
    va_list ap;
    int n;
    unsigned int size = 0;

    *str = NULL;

    for (;;) {
        va_start(ap, fmt);
        n = vsnprintf (*str, size, fmt, ap);
        va_end(ap);

        if (n < 0) {
            fprintf(stderr, "%s: encountered an output or encoding"
                    " error during vsnprintf.\n",
                    __FUNCTION__);
            exit(EXIT_FAILURE);
        }

        if (n < size)
            break;

        /* Truncated, try again with more space. */
        size = n+1;
        *str = realloc(*str, size);
    }
}



int
rm_r(const char *path)
{
    int ret = 0;
    DIR *dir;
    struct dirent *dent;

    if (path == NULL) {
        opkg_perror(ERROR, "Missing directory parameter");
        return -1;
    }

    dir = opendir(path);
    if (dir == NULL) {
        opkg_perror(ERROR, "Failed to open dir %s", path);
        return -1;
    }

    if (fchdir(dirfd(dir)) == -1) {
        opkg_perror(ERROR, "Failed to change to dir %s", path);
        closedir(dir);
        return -1;
    }

    while (1) {
        errno = 0;
        if ((dent = readdir(dir)) == NULL) {
            if (errno) {
                opkg_perror(ERROR, "Failed to read dir %s",
                        path);
                ret = -1;
            }
            break;
        }

        if (!strcmp(dent->d_name, ".") || !strcmp(dent->d_name, ".."))
            continue;

#ifdef _BSD_SOURCE
        if (dent->d_type == DT_DIR) {
            if ((ret = rm_r(dent->d_name)) == -1)
                break;
            continue;
        } else if (dent->d_type == DT_UNKNOWN)
#endif
        {
            struct stat st;
            if ((ret = lstat(dent->d_name, &st)) == -1) {
                opkg_perror(ERROR, "Failed to lstat %s",
                        dent->d_name);
                break;
            }
            if (S_ISDIR(st.st_mode)) {
                if ((ret = rm_r(dent->d_name)) == -1)
                    break;
                continue;
            }
        }

        if ((ret = unlink(dent->d_name)) == -1) {
            opkg_perror(ERROR, "Failed to unlink %s", dent->d_name);
            break;
        }
    }

    if (chdir("..") == -1) {
        ret = -1;
        opkg_perror(ERROR, "Failed to change to dir %s/..", path);
    }

    if (rmdir(path) == -1 ) {
        ret = -1;
        opkg_perror(ERROR, "Failed to remove dir %s", path);
    }

    if (closedir(dir) == -1) {
        ret = -1;
        opkg_perror(ERROR, "Failed to close dir %s", path);
    }

    return ret;
}


int
xsystem(const char *argv[])
{
    int status;
    pid_t pid;

    pid = vfork();

    switch (pid) {
    case -1:
        opkg_perror(ERROR, "%s: vfork", argv[0]);
        return -1;
    case 0:
        /* child */
        execvp(argv[0], (char*const*)argv);
        _exit(-1);
    default:
        /* parent */
        break;
    }

    if (waitpid(pid, &status, 0) == -1) {
        opkg_perror(ERROR, "%s: waitpid", argv[0]);
        return -1;
    }

    if (WIFSIGNALED(status)) {
        opkg_msg(ERROR, "%s: Child killed by signal %d.\n",
            argv[0], WTERMSIG(status));
        return -1;
    }

    if (!WIFEXITED(status)) {
        /* shouldn't happen */
        opkg_msg(ERROR, "%s: Your system is broken: got status %d "
            "from waitpid.\n", argv[0], status);
        return -1;
    }

    return WEXITSTATUS(status);
}



struct opkg_intercept
{
    char *oldpath;
    char *statedir;
};

typedef struct opkg_intercept *opkg_intercept_t;

static opkg_intercept_t
opkg_prep_intercepts(void)
{
    opkg_intercept_t ctx;
    char *newpath;

    ctx = calloc(1, sizeof (*ctx));
    ctx->oldpath = strdup(getenv("PATH"));
    sprintf_alloc(&newpath, "%s/opkg/intercept:%s", "/var/lib", ctx->oldpath);
    sprintf_alloc(&ctx->statedir, "%s/opkg-intercept-XXXXXX", conf->tmp_dir);

    if (mkdtemp(ctx->statedir) == NULL) {
        opkg_perror(ERROR,"Failed to make temp dir %s", ctx->statedir);
    free(ctx->oldpath);
    free(ctx->statedir);
        free(newpath);
    free(ctx);
    return NULL;
    }

    setenv("OPKG_INTERCEPT_DIR", ctx->statedir, 1);
    setenv("PATH", newpath, 1);
    free(newpath);

    return ctx;
}

static int
opkg_finalize_intercepts(opkg_intercept_t ctx)
{
    DIR *dir;
    int err = 0;

    setenv ("PATH", ctx->oldpath, 1);
    free (ctx->oldpath);

    dir = opendir (ctx->statedir);
    if (dir) {
    struct dirent *de;
    while (de = readdir (dir), de != NULL) {
        char *path;

        if (de->d_name[0] == '.')
        continue;

        sprintf_alloc (&path, "%s/%s", ctx->statedir, de->d_name);
        if (access (path, X_OK) == 0) {
        const char *argv[] = {"sh", "-c", path, NULL};
        xsystem (argv);
        }
        free (path);
    }
        closedir(dir);
    } else
    opkg_perror(ERROR, "Failed to open dir %s", ctx->statedir);

    rm_r(ctx->statedir);
    free (ctx->statedir);
    free (ctx);

    return err;
}



static void
write_status_files_if_changed(void)
{
     if (opkg_state_changed && !conf->noaction) {
      opkg_msg(INFO, "Writing status file.\n");
      opkg_conf_write_status_files();
      pkg_write_changed_filelists();
      sync();
     } else {
      opkg_msg(DEBUG, "Nothing to be done.\n");
     }
}




/* For package pkg do the following: If it is already visited, return. If not,
   add it in visited list and recurse to its deps. Finally, add it to ordered
   list.
   pkg_vec all contains all available packages in repos.
   pkg_vec visited contains packages already visited by this function, and is
   used to end recursion and avoid an infinite loop on graph cycles.
   pkg_vec ordered will finally contain the ordered set of packages.
*/
static int
opkg_recurse_pkgs_in_order(pkg_t *pkg, pkg_vec_t *all,
                               pkg_vec_t *visited, pkg_vec_t *ordered)
{
    int j,k,l,m;
    int count;
    pkg_t *dep;
    compound_depend_t * compound_depend;
    depend_t ** possible_satisfiers;
    abstract_pkg_t *abpkg;
    abstract_pkg_t **dependents;

    /* If it's just an available package, that is, not installed and not even
       unpacked, skip it */
    /* XXX: This is probably an overkill, since a state_status != SS_UNPACKED
       would do here. However, if there is an intermediate node (pkg) that is
       configured and installed between two unpacked packages, the latter
       won't be properly reordered, unless all installed/unpacked pkgs are
       checked */
    if (pkg->state_status == SS_NOT_INSTALLED)
        return 0;

    /* If the  package has already been visited (by this function), skip it */
    for(j = 0; j < visited->len; j++)
        if ( ! strcmp(visited->pkgs[j]->name, pkg->name)) {
            opkg_msg(DEBUG, "pkg %s already visited, skipping.\n", pkg->name);
            return 0;
        }

    pkg_vec_insert(visited, pkg);

    count = pkg->pre_depends_count + pkg->depends_count + \
        pkg->recommends_count + pkg->suggests_count;

    opkg_msg(DEBUG, "pkg %s.\n", pkg->name);

    /* Iterate over all the dependencies of pkg. For each one, find a package
       that is either installed or unpacked and satisfies this dependency.
       (there should only be one such package per dependency installed or
       unpacked). Then recurse to the dependency package */
    for (j=0; j < count ; j++) {
        compound_depend = &pkg->depends[j];
        possible_satisfiers = compound_depend->possibilities;
        for (k=0; k < compound_depend->possibility_count ; k++) {
            abpkg = possible_satisfiers[k]->pkg;
            dependents = abpkg->provided_by->pkgs;
            l = 0;
            if (dependents != NULL)
                while (l < abpkg->provided_by->len && dependents[l] != NULL) {
                    opkg_msg(DEBUG, "Descending on pkg %s.\n",
                                 dependents [l]->name);

                    /* find whether dependent l is installed or unpacked,
                     * and then find which package in the list satisfies it */
                    for(m = 0; m < all->len; m++) {
                        dep = all->pkgs[m];
                        if ( dep->state_status != SS_NOT_INSTALLED)
                            if ( ! strcmp(dep->name, dependents[l]->name)) {
                                opkg_recurse_pkgs_in_order(dep, all,
                                                           visited, ordered);
                                /* Stop the outer loop */
                                l = abpkg->provided_by->len;
                                /* break from the inner loop */
                                break;
                            }
                    }
                    l++;
                }
        }
    }

    /* When all recursions from this node down, are over, and all
       dependencies have been added in proper order in the ordered array, add
       also the package pkg to ordered array */
    pkg_vec_insert(ordered, pkg);

    return 0;

}



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


static int send_message(char *sig, void *message, int length)
{
    static int sock;
    int len;
    struct sockaddr_un remote;
    struct timeval tv;

    if (debug)
        fprintf(stderr, "Sending %d-byte message: %s\n", ntohl(((int *)message)[0]), (char *)(((int *)message)+1));

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


    /* Set socket to timeout after one second. */
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    if (setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv)) < 0)
        perror("Unable to set socket timeout");


    if (connect(sock, (struct sockaddr *)&remote, len) == -1) {
        perror("Unable to conncet to socket");
        close(sock);
        return 1;
    }

    if (send(sock, message, length, 0) == -1) {
        perror("Unable to send message to socket");
        close(sock);
        return 1;
    }

    close(sock);
    return 0;
}


static int
chumby_configure_cmd()
{
     pkg_vec_t *all, *ordered, *visited;
     int i;
     pkg_t *pkg;
     opkg_intercept_t ic;
     int r, err = 0;
    long progress_message[50];
    int progress_message_length;

     all = pkg_vec_alloc();

     pkg_hash_fetch_available(all);

     /* Reorder pkgs in order to be configured according to the Depends: tag
        order */
     ordered = pkg_vec_alloc();
     visited = pkg_vec_alloc();
     for(i = 0; i < all->len; i++) {
         pkg = all->pkgs[i];
         opkg_recurse_pkgs_in_order(pkg, all, visited, ordered);
     }

     ic = opkg_prep_intercepts();
     if (ic == NULL) {
         err = -1;
         goto error;
     }

     for(i = 0; i < ordered->len; i++) {
      pkg = ordered->pkgs[i];

      if (pkg->state_status == SS_UNPACKED) {
           opkg_msg(NOTICE, "Configuring %s.\n", pkg->name);

           progress_message_length = snprintf((char *)(progress_message+1),
                                    sizeof(progress_message)-sizeof(progress_message[0]),
                                    "NeTVBrowser|~|ConfigureProgress|~|%d|~|%s",
                                    (i*100)/ordered->len,
                                    pkg->name) +5;
           /*
            * Put the string's length, plus the NULL, plus the length size into
            * the first four bytes
            */
           ((long *)progress_message)[0] = htonl(progress_message_length);
           send_message("NeTVBr", progress_message, progress_message_length);

           r = opkg_configure(pkg);
           if (r == 0) {
            pkg->state_status = SS_INSTALLED;
            pkg->parent->state_status = SS_INSTALLED;
            pkg->state_flag &= ~SF_PREFER;
            opkg_state_changed++;
           } else {
            err = -1;
           }
      }
     }

     if (opkg_finalize_intercepts (ic))
     err = -1;

error:
     pkg_vec_free(all);
     pkg_vec_free(ordered);
     pkg_vec_free(visited);

     return err;
}

/* Based on libopkg/opkg_cmd:opkg_upgrade_cmd() */
static int
chumby_upgrade_cmd()
{
    int i, err = 0;
    long long bytes_total, bytes_done;
    pkg_t *pkg;
    pkg_vec_t *installed;
    long progress_message[50];
    int progress_message_length;
    
    installed = pkg_vec_alloc();
    pkg_info_preinstall_check();
    pkg_hash_fetch_all_installed(installed);


    for (bytes_total = i = 0; i < installed->len; i++)
        bytes_total += installed->pkgs[i]->size;


    if (debug)
        fprintf(stderr, "There are %d packages installed\n", installed->len);
    /* Install/upgrade the packages */
    for (bytes_done = i = 0; i < installed->len; i++) {
        pkg = installed->pkgs[i];


        if (debug)
            fprintf(stderr, "Upgrading %s (%d/%d) - %lld%%\n", pkg->name, i, installed->len, (bytes_done*100)/bytes_total);

        if (opkg_upgrade_pkg(pkg))
            err = -1;

        /*
         * Put the string's length, plus the NULL, plus the length size into
         * the first four bytes
         */
        bytes_done += pkg->size;
        progress_message_length = snprintf((char *)(progress_message+1),
                                    sizeof(progress_message)-sizeof(progress_message[0]),
                                    "NeTVBrowser|~|UpdateProgress|~|%d|~|%s|~|%lld",
                                    (i*100)/installed->len,
                                    pkg->name,
                                    (bytes_done*100)/bytes_total
                                    ) +5;
        ((long *)progress_message)[0] = htonl(progress_message_length);
        send_message("NeTVBr", progress_message, progress_message_length);
    }
    pkg_vec_free(installed);



    chumby_configure_cmd();


    write_status_files_if_changed();


    progress_message_length = snprintf((char *)(progress_message+1),
                                sizeof(progress_message)-sizeof(progress_message[0]),
                                "NeTVBrowser|~|UpgradeComplete") + 5;
    ((long *)progress_message)[0] = htonl(progress_message_length);
    send_message("NeTVBr", progress_message, progress_message_length);


    return err;
}


int main(int argc, char **argv) {
    char *cache_str = "/var/lib/opkg/tmp";

    if (argc > 1)
        debug = 1;

    if (opkg_conf_init())
        goto err0;

    conf->cache = malloc(strlen(cache_str)+1);
    strcpy(conf->cache, cache_str);

    if (opkg_conf_load())
        goto err0;

    if (pkg_hash_load_feeds())
        goto err1;

    if (pkg_hash_load_status_files())
        goto err1;

    signal(SIGPIPE, sigpipe_happened);

    chumby_upgrade_cmd();

err1:
    opkg_conf_deinit();

err0:
    print_error_list();
    free_error_list();

    return 0;
}
