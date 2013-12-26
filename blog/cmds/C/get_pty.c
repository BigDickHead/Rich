#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include "pty.h"
#include "util.h"


#ifndef LINUX
#define DEVICE_NAME_SIZE	12
#define PTY_DEVICE_NAME		"/dev/ptyxy"
#define TTY_DEVICE_NAME		"/dev/ttyxy"
#else
#define PT_MASTER_DEVICE_NAME	"/dev/ptmx"
#endif

/*  get a pseudo terminal */
int	get_pty(int *mfd, int *sfd);



#ifndef LINUX

static const char *cmap[] = {
    "pqrs",
    "0123456789abcdefghijklmnopqrstuv"
};


int
get_pty(int *mfd, int *sfd)
{
    char mdev[DEVICE_NAME_SIZE + 1];
    char sdev[DEVICE_NAME_SIZE + 1];
    char *mp, *sp;
    int dx, dy;
    
    strcpy(mdev, PTY_DEVICE_NAME);
    mp = mdev + strlen(PTY_DEVICE_NAME) - 2;

    strcpy(sdev, TTY_DEVICE_NAME);
    sp = sdev + strlen(TTY_DEVICE_NAME) - 2;

    for (dx = 0; dx < strlen(cmap[0]); dx++) {
	mp[0] = cmap[0][dx];
	for (dy = 0; dy < strlen(cmap[1]); dy++) {
	    mp[1] = cmap[1][dy];
	    *mfd = open(mdev, O_RDWR);
	    if (*mfd != -1) {
		sp[0] = mp[0];
		sp[1] = mp[1];
		*sfd = open(sdev, O_RDWR);
		if (*sfd != -1) {
		    debug_printf("attached %s (%i), %s (%i)\n", mdev, *mfd, sdev, *sfd);
		    return 1;
		} else {
		    sys_err_printf("get_pty: open");
		}
	    } else {
		debug_printf("get_pty: open");
	    }
	} 
    }
    return -1;
}

#else
int
get_pty(int *mfd, int *sfd)
{
    char sdev[64];

    debug_printf("opening master device: %s\n", PT_MASTER_DEVICE_NAME);
    *mfd = open(PT_MASTER_DEVICE_NAME, O_RDWR);
    if(*mfd != -1) {
	grantpt(*mfd);
	unlockpt(*mfd);
	if (ptsname_r(*mfd, sdev, sizeof(sdev)) != 0) {
	    sys_err_printf("get_pty: ptsname");
	    return -1;
	}
	debug_printf("opening slave device: %s\n", sdev);
	*sfd = open(sdev, O_RDWR);
	if (*sfd != -1) {
	    debug_printf("attached %s (%i), %s (%i)\n", PT_MASTER_DEVICE_NAME, *mfd, sdev, *sfd);
	    return 1;
	} else {
	    sys_err_printf("get_pty: open slave");
	}
    } else {
        sys_err_printf("get_pty: open master");
    }
		
    return -1;
}

#endif
