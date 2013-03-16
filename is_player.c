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
    int freq[10];
    double duration[10];
    int num_els;
//    resolve_command("3Dm235", currel, freq_ind, duration, num_els);
    resolve_command("3Af2", currel, freq, duration, &num_els);
    printf("numels %d\n", num_els);
//    printf("Freq, %d\n", freq[currel]);
//    printf("duration, %e\n", duration[currel]);
    currel += num_els;
    resolve_command("P2", currel, freq, duration, &num_els);
    currel += num_els;
    resolve_command("4Cf2", currel, freq, duration, &num_els);
    currel += num_els;
//    printf("Freq, %d\n", freq[currel]);
//    printf("duration, %e\n", duration[currel]);

    fp = open("/dev/tty0", O_WRONLY);
    play_sequence(fp, freq, duration, currel);
//    play_note(fp, freq[0], 1000 * duration[0]);
    close(fp);
    return 0;
}
