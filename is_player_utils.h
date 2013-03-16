void print_frequency(int frequency);
void init_arrs();
void set_ref_time(double time);
void play_note(int fp, int freq, int duration);
void play_pause(int duration);
void resolve_command(char command[], int currel, int freq_ind[], double duration[], int *num_els);
void play_sequence(int fp, int freqs[], double duration[], int num_els);
