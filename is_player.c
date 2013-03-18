#include <stdio.h>
#include <sys/ioctl.h>
#include <linux/kd.h>
#include <math.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include "is_player_utils.h"

int fp;

void sig_handler(int signo)
{
    if (signo == SIGINT || signo == SIGTERM)
    {
        ioctl(fp, KIOCSOUND, 0);
        close(fp);
        exit(0);
    }
}

int main()
{
    struct sigaction act;
    memset(&act, '\0', sizeof(act));
    act.sa_handler = &sig_handler;
//    act.sa_sigaction = &sig_handler;
//    act.sa_flags = SA_SIGINFO;
    sigemptyset(&act.sa_mask);
    sigaddset(&act.sa_mask, SIGINT);
    sigaddset(&act.sa_mask, SIGTERM);
    sigaction(SIGTERM, &act, NULL);
    sigaction(SIGINT, &act, NULL);
    init_arrs();
    // 2 seconds is the length of a whole note now
    set_ref_time(2000.0);
    int currel = 0;
    int freq[100];
    double duration[100];
    char command[100];
    int ch;
    int num_els;
    int res;
    fscanf(stdin, "%s", command);
    do
    {
        resolve_command(command, currel, freq, duration, &num_els);
        currel += num_els;
        res = fscanf(stdin, "%s", command);
        ch = getc(stdin);
    } while (ch != EOF);
    fp = open("/dev/tty0", O_WRONLY);
    play_sequence(fp, freq, duration, currel);
    close(fp);
    return 0;
}
