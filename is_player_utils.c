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
//In which octave the reference tone is located
const int REF_OCT = 4;
//int notearr[12 * 8];
int freq_map[256] = {-400};
int offset_map[256] = {-1000};
int octave_offset[256] = {-500};
double ref_time;

void initialize_array(int array[], int len, int val)
{
    int i;
    for (i=0; i<len; i++)
    {
        array[i] = val;
    }
}

void print_array(int array[], int len)
{
    int i;
    for (i=0; i<len; i++)
    {
        printf("%d\n", array[i]);
    }
}

//The octave here refers to the 'internal' octave.
int get_freq(int octave, int note_ind)
{
    double curr_ref_freq;
    curr_ref_freq = REF_FREQ + (octave - REF_OCT) * log(2);
    return (int)(exp(curr_ref_freq + note_ind * DLOG) * 1000 + 0.5);
}

void init_arrs()
{
    initialize_array(freq_map, 256, -400);
    initialize_array(offset_map, 256, -1000);
    initialize_array(octave_offset, 256, -500);
//    int i;
//    int j;
//    double curr_ref_freq;
//    for (i=0; i < 8; i++)
//    {
//        for (j=0; j < 12; j++)
//        {
//            curr_ref_freq = REF_FREQ + (i - 4) * log(2);
//            notearr[i * 12 + j] = (int)(exp(curr_ref_freq + j * DLOG) * 1000 + 0.5);
//        }
//    }
    freq_map['A'] = 0;
    freq_map['H'] = 2;
    freq_map['C'] = 3;
    freq_map['D'] = 5;
    freq_map['E'] = 7;
    freq_map['F'] = 8;
    freq_map['G'] = 10;
    octave_offset['A'] = 0;
    octave_offset['H'] = 0;
    octave_offset['C'] = -1;
    octave_offset['D'] = -1;
    octave_offset['E'] = -1;
    octave_offset['F'] = -1;
    octave_offset['G'] = -1;
    offset_map['s'] = 1;
    offset_map['f'] = -1;
    offset_map['#'] = 1;
    offset_map['f'] = -1;
}

void set_ref_time(double time)
{
    ref_time = time;
}

//freq must be 1000* the actual integer frequency. Duration is in ms
void play_note(int fp, int freq, int duration)
{
    ioctl(fp, KIOCSOUND, 1193180000/freq);
    usleep(duration * 1000);
    ioctl(fp, KIOCSOUND, 0);
}

void play_pause(int duration)
{
    usleep(duration * 1000);
}


//The interface function to the playing part of the program.
//Input:         
//      fp: file descriptor for the sound device
//      freqs, containing (int)frequency*1000 of the notes you want
//to play. 
//      duration, containing the duration of the note in ms.
//      num_els, total number of elements in freqs and duration arrays.
//Some integers in freqs are special codes for more advanced commands:
//-1: Play pause with the corresponding length in duration-array.
//
void play_sequence(int fp, int freqs[], double duration[], int num_els)
{
    int i;
    int duration_int;
    for (i=0; i<num_els; i++)
    {
        duration_int = (int)duration[i];
        if (freqs[i] == -1)
        {
            play_pause(duration_int);
        }
        else
        {
            play_note(fp, freqs[i], duration_int);
        }
    }
}

// Input: Command, a string containing the encoded note (or chord) information
// Currel: which element of the freq and duration arrays we should start at
// Output: freq, an array containing the index of the frequency to be
// played. If pause, the array element will be -1.
// duration: double array giving the duration of the tone in milliseconds.
// num_els: How many elements were actually added by this command (for chords,
// it will typically be several)
void resolve_command(char command[], int currel, int freq[], double duration[], int *num_els)
{
    int currpos = 0;
    int numfound;
    int searchlen;
    int numerator, denominator;
    int currdivision;
    int octave;
    int tone;
    int dum;
    int oct_offset;
    duration[currel] = 0;
    if (command[0] == 'P')
    {
        *num_els = 1;
        freq[currel] = -1;
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
        duration[currel] *= ref_time;
    }
    else
    {
        *num_els = 1;
        // We want a tone - must find its frequency and length
        octave = command[0] - '0';
//        printf("octave %d\n", octave);
        currpos++;
        tone = freq_map[command[currpos]];
        oct_offset = octave_offset[command[currpos]];
        if (tone == -400)
        {
            printf("Error in note resolution - note has invalid value\n");
            exit(0);
        }
//        printf("commandcurrpos %d\n", command[currpos]);
        currpos++;
        if (isalpha(command[currpos]))
        {
            // Means we want a 'sharp' or 'flat' tone
            dum = offset_map[command[currpos]];
            if (dum == -1000)
            {
                printf("Error in note resolution - sharp/flat has invalid value\n");
                exit(0);
            }
            tone += dum;
            currpos++;
        }
        freq[currel] = get_freq(octave + oct_offset, tone); 
        do
        {
            if (command[currpos] == '[')
            {
                numfound = sscanf(command + currpos, "[%d/%d]%n", &numerator, &denominator, &searchlen);
                if (numfound != 3)
                {
                    printf("Error in note resolution - number of arguments found are wrong\n");
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
        duration[currel] *= ref_time;
    }
}
