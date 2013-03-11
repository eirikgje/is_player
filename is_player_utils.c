#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <linux/kd.h>
#include <math.h>
#include <fcntl.h>
#include <string.h>
#include <regex.h>
#include <ctype.h>

const double REF_FREQ = log(440.0);
const double DLOG = log(2) / 12;
int notearr[12 * 7];
int freq_map[256] = {-400};
int offset_map[256] = {-1000};
double ref_time;

void print_frequency(int freq_ind)
{
    printf("notearr\n");
    int i;
    for (i=0; i<12*7; i++)
    {
        printf("%d\n", notearr[i]);
    }
    printf("%d\n", notearr[freq_ind]);
}

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
            notearr[i * 12 + j] = (int)(exp(curr_ref_freq + j * DLOG) * 1000 + 0.5);
        }
    }
    freq_map['A'] = 0;
    freq_map['H'] = 2;
    freq_map['C'] = 3;
    freq_map['D'] = 5;
    freq_map['E'] = 7;
    freq_map['F'] = 8;
    freq_map['G'] = 10;
    offset_map['s'] = 1;
    offset_map['f'] = -1;
}

void set_ref_time(double time)
{
    ref_time = time;
}

void play_note(int fp, int freq_ind, int duration)
{
    ioctl(fp, KIOCSOUND, 1193180000/notearr[freq_ind]);
    usleep(duration * 1000);
    ioctl(fp, KIOCSOUND, 0);
}

void play_pause(int duration)
{
    usleep(duration * 1000);
}

// Input: Command, a string containing the encoded note (or chord) information
// Currel: which element of the freq_ind and duration arrays we should start at
// Output: freq_ind, an array containing the index of the frequency to be
// played. If pause, the array element will be -1.
// duration: double array giving the duration of the tone in milliseconds.
// num_els: How many elements were actually added by this command (for chords,
// it will typically be several)
void resolve_command(char command[], int currel, int freq_ind[], double duration[], int num_els)
{
    int currpos = 0;
    int numfound;
    int searchlen;
    int numerator, denominator;
    int currdivision;
    int octave;
    int dum;
    duration[currel] = 0;
    if (command[0] == 'P')
    {
        num_els = 1;
        freq_ind[currel] = -1;
        // We want a pause - must find its length
        currpos++;
        do
        {
            if (command[currpos] == '[')
            {
                numfound = sscanf(command + currpos, "[%d/%d]%n", &numerator, &denominator, &searchlen);
                if (numfound != 3)
                {
                    printf("Error in Pause resolution - number of arguments found are wrong\n");
                    printf("%d\n", numfound);
                    exit(0);
                }
                currpos += searchlen;
                duration[currel] += (double)numerator / (double)denominator;
            }
            else
            {
                //Input should be a number
                currdivision = command[currpos] - '0';
                duration[currel] += pow(0.5, currdivision);
                currpos++;
            }
        } while (command[currpos] != 0);
    }
    else
    {
        num_els = 1;
        // We want a tone - must find its frequency and length
        octave = command[0] - '0';
//        printf("octave %d\n", octave);
        currpos++;
        dum = freq_map[command[currpos]];
        if (dum == -400)
        {
            printf("Error in note resolution - note has invalid value\n");
            exit(0);
        }
//        printf("commandcurrpos %d\n", command[currpos]);
        freq_ind[currel] = 12 * octave + dum;
        currpos++;
        if (isalpha(command[currpos]))
        {
            // Means we want a 'sharp' or 'flat' tone
            dum = offset_map[command[currpos]];
            if (dum == -1000)
            {
                printf("Error in note resolution - sharp/flat has invalid value\n");
            }
            freq_ind[currel] += dum;
            currpos++;
        }
        do
        {
            if (command[currpos] == '[')
            {
                numfound = sscanf(command + currpos, "[%d/%d]%n", &numerator, &denominator, &searchlen);
                if (numfound != 3)
                {
                    printf("Error in Pause resolution - number of arguments found are wrong\n");
                    printf("%d\n", numfound);
                    exit(0);
                }
                currpos += searchlen;
                duration[currel] += (double)numerator / (double)denominator;
            }
            else
            {
                //Input should be a number
                currdivision = command[currpos] - '0';
                duration[currel] += pow(0.5, currdivision);
                currpos++;
            }
        } while (command[currpos] != 0);
    }
}
