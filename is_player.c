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
    int fp;
    int currel = 0;
    int freq_ind[10] = {0};
    double duration[10]  = {0};
    int num_els;
    resolve_command("3Dm235", currel, freq_ind, duration, num_els);
    printf("currel, %d\n", currel);
    printf("freq_ind\n");
    int i;
    for (i=0; i<10; i++)
    {
        printf("%d\n", freq_ind[i]);
    }
    printf("duration\n", duration);
    for (i=0; i<10; i++)
    {
        printf("%e\n", duration[i]);
    }
    printf("num_els, %d\n", num_els);
    printf("freq_ind0, %d\n", freq_ind[0]);
    printf("Actual frequency\n");
    print_frequency(freq_ind[0]);

//    fp = open("/dev/tty0", O_WRONLY);
}
