#pragma once

// http://forums.fedoraforum.org/showthread.php?t=172337

// only needed for linux
#ifdef _WIN32
// nothing todo here since win has this feature already
#else

#include <iostream>
#include <sys/select.h>

int _kbhit(void) {
    struct timeval tv;
    fd_set read_fd;
    
    // disable waiting
    tv.tv_sec = 0;
    tv.tv_usec = 0;

    // before init read_fd
    FD_ZERO(&read_fd);

    // is input ready, 0 is stdin
    FD_SET(0, &read_fd);

    // param 1: largest file descriptor to check + 1
    // param 4: no writes, just read
    // param 5: use as set above
    if (select(1, &read_fd, NULL, NULL, &tv) == -1)
    {
        return 0;
    }

    // is char pending on stdin
    if (FD_ISSET(0, &read_fd))
    {
        return 1;
    }
    
    // nothing pending
    return 0;
}

int _getch_nolock(void)
{
    int ret = getchar();
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
    
    return ret;
}

#endif