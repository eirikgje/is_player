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
    int freq[10];
    double duration[10];
    int num_els;
//    resolve_command("3Dm235", currel, freq_ind, duration, num_els);
    resolve_command("3Af2", currel, freq, duration, &num_els);
    printf("numels %d\n", num_els);
    printf("Freq, %d\n", freq[currel]);
    printf("duration, %e\n", duration[currel]);
    currel += num_els;
    resolve_command("P245", currel, freq, duration, &num_els);
    printf("Freq, %d\n", freq[currel]);
    printf("duration, %e\n", duration[currel]);
//    printf("currel, %d\n", currel);
//    printf("freq_ind\n");
//    int i;
//    for (i=0; i<10; i++)
//    {
//        printf("%d\n", freq_ind[i]);
//    }
//    printf("frequency \n");
//    print_frequency(freq_ind[0]);
//    printf("duration\n", duration);
//    for (i=0; i<10; i++)
//    {
//        printf("%e\n", duration[i]);
//    }
//    printf("num_els, %d\n", num_els);
//    printf("freq_ind0, %d\n", freq_ind[0]);
//    printf("Actual frequency\n");
//    print_frequency(freq_ind[0]);

    printf("Freq, %d\n", freq[0]);
    printf("duration %e\n", duration[0]);
    fp = open("/dev/tty0", O_WRONLY);
    play_note(fp, freq[0], 1000 * duration[0]);
    close(fp);
    return 0;
}
