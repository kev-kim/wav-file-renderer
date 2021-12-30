#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "io.h"
#include "wave.h"

int main(int argc, char** argv) {
    
    //error handling
    if (argc != 6) {
        printf("ERROR: incorrect number of arguments");
        return 1;
    }
    
    //store all parameters into variables
    int waveform = atoi(argv[1]);
    float freq_hz = atof(argv[2]);
    float gain = atof(argv[3]);
    unsigned num_samples = atof(argv[4]);
    int16_t *mono_buf = malloc(num_samples * sizeof(int16_t));
    if (mono_buf == NULL) printf("ERROR: memory allocation failed");
    
    //generate waveform and apply gain
    switch (waveform) {
        case 0: //sine wave
            generate_sine_wave(mono_buf, num_samples, freq_hz);
            break;
        case 1: //square wave
            generate_square_wave(mono_buf, num_samples, freq_hz);
            break;
        case 2: //sawtooth wave
            generate_saw_wave(mono_buf, num_samples, freq_hz);
            break;
        default:
            break;
    }
    apply_gain(mono_buf, num_samples, gain);
    
    //make the stereo buffer and mix the audio to both the left and right channels
    int16_t *stereo_buf = calloc(num_samples * 2, (int) sizeof(int16_t));
    mix_in(stereo_buf, 0, mono_buf, num_samples);
    mix_in(stereo_buf, 1, mono_buf, num_samples);

    //open the output file, call write_wave_header
    FILE *wavfileout = fopen(argv[5], "wb");
    write_wave_header(wavfileout, num_samples);
    
    //write the wave into the file
    for (int i = 0; i < (int) num_samples * 2; i++) {
        write_u16(wavfileout, stereo_buf[i]);
    }
    
    //free memory and close file
    free(stereo_buf);
    free(mono_buf);
    fclose(wavfileout);

    return 0;
}
