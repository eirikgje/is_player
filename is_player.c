#include <stdio.h>
#include <sys/ioctl.h>
#include <linux/kd.h>
#include <math.h>
#include <fcntl.h>
#include <string.h>
#include "is_player_utils.h"

int main()
{
    init_arrs();
    // 2 seconds is the length of a whole note now
    set_ref_time(2000.0);
    int fp;
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
//    printf("currel, %d\n", currel);
//    printf("freq\n");
//    print_array(freq, currel);
//    printf("duration\n");
//    print_darray(duration, currel);
//    return 0;
    fp = open("/dev/tty0", O_WRONLY);
    play_sequence(fp, freq, duration, currel);
    close(fp);
    return 0;
}
