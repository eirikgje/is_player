#include <stdio.h>
#include <sys/ioctl.h>
#include <linux/kd.h>
#include <math.h>
#include <fcntl.h>
#include <string.h>

int main()
{
    init_notearr();
    int fp;
    fp = open("/dev/tty0", O_WRONLY);
}
