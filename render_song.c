#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "io.h"
#include "wave.h"

typedef struct instrument_ {
    unsigned waveform;
    float pan;
    unsigned isADSR;
    float gain;
} Instrument;

void process_N(int16_t mono_buf[], int16_t mono_buf_copy[], int16_t stereo_buf[], Instrument arr_instruments[], unsigned inst_num_N, unsigned i_pos, unsigned end_pos, unsigned note, float gain) {
    
    float freq = 440.0 * pow(2.0, ((float) note - 69.0) / 12.0);
    unsigned num_samples = end_pos - i_pos;
    
    //find instrument waveform
    switch(arr_instruments[inst_num_N].waveform) {
        case 0:
            generate_sine_wave(&mono_buf[i_pos], num_samples, freq);
            break;
        case 1:
            generate_square_wave(&mono_buf[i_pos], num_samples, freq);
            break;
        case 2:
            generate_saw_wave(&mono_buf[i_pos], num_samples, freq);
            break;
        default:
            break;
    }

    //apply gain
    apply_gain(&mono_buf[i_pos], num_samples, gain * arr_instruments[inst_num_N].gain);

    //apply ADSR (if enabled)
    if (arr_instruments[inst_num_N].isADSR) {
       apply_adsr_envelope(&mono_buf[i_pos], num_samples);
    }
    
    //compute pan
    float pan[2];
    compute_pan(arr_instruments[inst_num_N].pan, pan);
    
    //apply pan
    for (int i = 0; i < (int) num_samples; i++) {
        mono_buf_copy[i + i_pos] = mono_buf[i + i_pos];
    }
    
    apply_gain(&mono_buf[i_pos], num_samples, pan[0]);
    apply_gain(&mono_buf_copy[i_pos], num_samples, pan[1]);

    //mix!
    mix_in(&stereo_buf[i_pos * 2], 0, &mono_buf[i_pos], num_samples);
    mix_in(&stereo_buf[i_pos * 2], 1, &mono_buf_copy[i_pos], num_samples);
}

int main(int argc, char ** argv) {
    //error handling
    if (argc!= 3) {
        printf("ERROR: incorrect number of arguments");
        return 1;
    }

    //open song input file
    FILE *songinput = fopen(argv[1], "r");

    //read number of samples from input file
    unsigned num_samples = 0;
    fscanf(songinput, " %u", &num_samples);
    
    //create and initalize array of instruments
    Instrument arr_instruments[16];
    for (int i = 0; i < 16; i++) {
        arr_instruments[i].waveform = 0;
        arr_instruments[i].pan = 0.0f;
        arr_instruments[i].isADSR = 0;
        arr_instruments[i].gain = 0.2f;
    }
    
    //variables for directive case 'N'
    unsigned inst_num_N = 0;
    unsigned i_pos = 0;
    unsigned end_pos = 0;
    unsigned note = 0;
    float gain = 0.0f;
    int16_t * mono_buf = calloc(num_samples, (int) sizeof(int16_t));
    int16_t * mono_buf_copy = calloc(num_samples, (int) sizeof(int16_t));
    int16_t *stereo_buf = calloc(num_samples * 2, (int) sizeof(int16_t));

    //dummy variables for fscanf (important, as fscanf doesn't work w/o these)
    unsigned temp_waveform = 0;
    float temp_pan = 0;
    unsigned temp_isADSR = 0;
    float temp_gain = 0.2f;

    //begin directive
    unsigned inst_num = 0;
    char directive = 'Z';
    
    int result = fscanf(songinput, " %c", &directive);
    while (result != EOF) {
       switch (directive) {
            case 'W':
                fscanf(songinput, " %u %u", &inst_num, &temp_waveform);
                arr_instruments[inst_num].waveform = temp_waveform;
                break;
            case 'P':
                fscanf(songinput, " %u %f", &inst_num, &temp_pan);
                arr_instruments[inst_num].pan = temp_pan;
                break;
            case 'E':
                fscanf(songinput, " %u %u", &inst_num, &temp_isADSR);
                arr_instruments[inst_num].isADSR = temp_isADSR;
                break;
            case 'G':
                fscanf(songinput, " %u %f", &inst_num, &temp_gain);
                arr_instruments[inst_num].gain = temp_gain;
                break;
            case 'N':
                fscanf(songinput, " %u %u %u %u %f", &inst_num_N, &i_pos, &end_pos, &note, &gain); 
                process_N(mono_buf, mono_buf_copy, stereo_buf, arr_instruments, inst_num_N, i_pos, end_pos, note, gain);
                break;
            default:
                fatal_error("malformed expression");
                break;
       }
       result = fscanf(songinput, " %c", &directive);
    }

    //open the output file and write wave header and wave info
    FILE *wavfileout = fopen(argv[2], "wb");
    write_wave_header(wavfileout, num_samples);
    write_s16_buf(wavfileout, stereo_buf, num_samples * 2);

    //free memory and close files
    free(mono_buf);
    free(mono_buf_copy);
    free(stereo_buf);
    fclose(songinput);
    fclose(wavfileout);

    return 0;
}
