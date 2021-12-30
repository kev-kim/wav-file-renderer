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
    if (argc != 5) {
        printf("ERROR: incorrect number of arguments");
        return 1;
    }
    
    //create wavfile for echo, then read_wave_header
    FILE * wavfilein = fopen(argv[1], "rb");
    if (wavfilein == NULL) {
        printf("ERROR: File could not be opened");
        return 1;
    }
    unsigned num_samples_stereo = 0;
    unsigned * num_samp = &num_samples_stereo;
    read_wave_header(wavfilein, num_samp);
    unsigned num_samples = num_samples_stereo * 2;

    //store wavfile information into array
    int16_t *stereo_buf_nodelay = calloc(num_samples, (int) sizeof(int16_t));
    read_s16_buf(wavfilein, stereo_buf_nodelay, num_samples);
    
    //calloc a new array to include delay
    int delay = atoi(argv[3]) * 2;
    num_samples += delay;
    
    int16_t *stereo_buf = calloc(num_samples, (int) sizeof(int16_t));
    for (int i = 0; i < (int) num_samples - delay; i++) {
        stereo_buf[i] = stereo_buf_nodelay[i];
    }
    
    //make a copy of the array
    int16_t *stereo_buf_copy = calloc(num_samples, (int) sizeof(int16_t));
    for (int i = 0; i < (int) num_samples; i++) {
        stereo_buf_copy[i] = stereo_buf[i];
    }

    //add echo effect
    float gain = atof(argv[4]);
    for (int i = 0; i < (int) num_samples - delay; i++) {
        stereo_buf[i + delay] += (int16_t) (gain * stereo_buf_copy[i]);
    }
    
    //open the output file, call write_wave_header
    FILE *wavfileout = fopen(argv[2], "wb");
    write_wave_header(wavfileout, num_samples / 2);
    
    write_s16_buf(wavfileout, stereo_buf, num_samples);
    
    //free memory and close files
    free(stereo_buf);
    free(stereo_buf_copy);
    free(stereo_buf_nodelay);
    fclose(wavfilein);
    fclose(wavfileout);
   
    return 0;
}
