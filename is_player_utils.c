#include <stdio.h>
#include <sys/ioctl.h>
#include <linux/kd.h>
#include <math.h>
#include <fcntl.h>
#include <string.h>
#include <regex.h>
#include <ctype.h>

const double REF_FREQ = log(440.0);
const int NOTEARR[12 * 7];
const double DLOG = log(2) / 12;
const int FREQ_MAP[256] = {-400};
const int OFFSET_MAP[256] = {-1000};
double ref_time;

void init_arrs()
{
    int i;
    int j;
    double curr_ref_freq;
    for (i=0; i < 7; i++)
    {
        for (j=0; j < 12; j++)
        {
            curr_ref_freq = REF_FREQ + ((i + 1) - 4) * log(2);
            NOTEARR[i * 12 + j] = (int)(exp(curr_ref_freq + j * DLOG) * 1000 + 0.5);
        }
    }
    FREQ_MAP['A'] = 0;
    FREQ_MAP['H'] = 2;
    FREQ_MAP['C'] = 3;
    FREQ_MAP['D'] = 5;
    FREQ_MAP['E'] = 7;
    FREQ_MAP['F'] = 8;
    FREQ_MAP['G'] = 10;
    OFFSET_MAP['s'] = 1;
    OFFSET_MAP['f'] = -1;
}

void set_ref_time(double time)
{
    ref_time = time
}

void play_note(int fp, int freq_ind, int duration)
{
    ioctl(fp, KIOCSOUND, 1193180000/notearr[freq_ind]);
    usleep(duration * 1000);
    ioctl(fp, KIOCSOUND, 0);
}

void play_pause(int duration)
{
    usleep(duration * 1000)
}

void resolve_command(int fp, char *command)
{
//    regex_t regex;
//    int reti;
    int currpos = 0;
    float totlength = 0;
    int numfound;
    int searchlen;
    int numerator, denominator;
    int currdivision;
    int freq_ind;
    int octave;
    int dum;
    if (command[0] == 'P')
    {
        // We want a pause - must find its length
        currpos++
        do while (command[currpos] != 0)
        {
            if (command[currpos] == '[')
            {
                numfound = sscanf(command + currpos, "[%d/%d]%n", &numerator, &denominator, &searchlen);
                if (numfound != 3)
                {
                    printf("Error in Pause resolution - number of arguments found are wrong");
                    printf(numfound)
                    exit(0);
                }
                currpos += searchlen;
                totlength += (float)numerator / float(denominator)
            }
            else
            {
                //Input should be a number
                currdivision = atoi(command[currpos]);
                totlength += (0.5) ** currdivision
                currpos++
            }
        }
    }
    else
    {
        // We want a tone - must find its frequency and length
        octave = atoi(command[0]);
        currpos++;
        dum = FREQ_MAP[command[currpos]];
        if (dum == -400)
        {
            printf("Error in note resolution - note has invalid value");
            exit(0);
        }
        freq_ind = 12 * octave + command[currpos];
        currpos++;
        if (isalpha(command[currpos]))
        {
            dum = OFFSET_MAP[command[currpos]];
            if (dum == -1000)
            {
                printf("Error in note resolution - sharp/flat has invalid value")
            }
            freq_ind += OFFSET[command[currpos]];
            currpos++
        }
        do while (command[currpos] != 0)
        {
            if (command[currpos] == '[')
            {
                numfound = sscanf(command + currpos, "[%d/%d]%n", &numerator, &denominator, &searchlen);
                if (numfound != 3)
                {
                    printf("Error in Pause resolution - number of arguments found are wrong");
                    printf(numfound)
                    exit(0);
                }
                currpos += searchlen;
                totlength += (float)numerator / float(denominator)
            }
            else
            {
                //Input should be a number
                currdivision = atoi(command[currpos]);
                totlength += (0.5) ** currdivision
                currpos++
            }
        }
    }
}
