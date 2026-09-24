#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/resource.h>
#include <ulimit.h>
#include <errno.h>
#include <string.h>
#include <limits.h>

extern char **environ;

static long get_num(const char *s){
    char *end;
    long v;
    errno = 0;
    v = strtol(s, &end, 10);
    if (errno != 0 || end == s || *end != '\0' || v < 0){
        fprintf(stderr, "bad value: %s\n", s);
        exit(1);
    }
    return v;
}

static void show_ids(void){
    printf("uid=%ld euid=%ld gid=%ld egid=%ld\n",
           (long)getuid(), (long)geteuid(),
           (long)getgid(), (long)getegid());
}

static void make_leader(void){
    if (setpgid(0, 0) == -1)
        perror("setpgid");
    else
        printf("pgid=%ld\n", (long)getpgrp());
}

static void show_pids(void){
    printf("pid=%ld ppid=%ld pgid=%ld\n",
           (long)getpid(), (long)getppid(), (long)getpgrp());
}

static void show_ulimit(void){
    long v = ulimit(UL_GETFSIZE, 0);
    if (v == -1 && errno != 0)
        perror("ulimit");
    else
        printf("ulimit=%ld\n", v);
}

static void set_ulimit(const char *s){
    if (ulimit(UL_SETFSIZE, get_num(s)) == -1)
        perror("ulimit");
}

static void show_core(void){
    struct rlimit r;
    if (getrlimit(RLIMIT_CORE, &r) == -1){
        perror("getrlimit");
        return;
    }
    if (r.rlim_cur == RLIM_INFINITY)
        printf("core=unlimited\n");
    else
        printf("core=%lu\n", (unsigned long)r.rlim_cur);
}

static void set_core(const char *s){
    struct rlimit r;
    if (getrlimit(RLIMIT_CORE, &r) == -1){
        perror("getrlimit");
        return;
    }
    r.rlim_cur = get_num(s);
    if (setrlimit(RLIMIT_CORE, &r) == -1)
        perror("setrlimit");
}

static void show_cwd(void){
    char buf[PATH_MAX];

    if (getcwd(buf, sizeof(buf)) == NULL)
        perror("getcwd");
    else
        printf("cwd=%s\n", buf);
}

static void show_env(void){
    char **e;
    for (e = environ; *e != NULL; e++)
        printf("%s\n", *e);
}

static void set_env(char *s){
    if (strchr(s, '=') == NULL || s[0] == '='){
        fprintf(stderr, "bad env: %s\n", s);
        return;
    }
    if (putenv(s) == -1)
        perror("putenv");
}

int main(int argc, char **argv){
    int opt;
    opterr = 0;
    while ((opt = getopt(argc, argv, ":ispuU:cC:dvV:")) != -1){
        if (opt == '?'){
            fprintf(stderr, "unknown: -%c\n", optopt);
            return 1;
        }
        if (opt == ':'){
            fprintf(stderr, "need arg: -%c\n", optopt);
            return 1;
        }
        switch (opt){
        case 'i': show_ids(); break;
        case 's': make_leader(); break;
        case 'p': show_pids(); break;
        case 'u': show_ulimit(); break;
        case 'U': set_ulimit(optarg); break;
        case 'c': show_core(); break;
        case 'C': set_core(optarg); break;
        case 'd': show_cwd(); break;
        case 'v': show_env(); break;
        case 'V': set_env(optarg); break;
        }
    }
    return 0;
}