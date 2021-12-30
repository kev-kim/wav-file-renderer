#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "io.h"
#include "wave.h"

void write_wave_header(FILE *out, unsigned num_samples) {
  //
  // See: http://soundfile.sapp.org/doc/WaveFormat/
  //

  uint32_t ChunkSize, Subchunk1Size, Subchunk2Size;
  uint16_t NumChannels = NUM_CHANNELS;
  uint32_t ByteRate = SAMPLES_PER_SECOND * NumChannels * (BITS_PER_SAMPLE/8u);
  uint16_t BlockAlign = NumChannels * (BITS_PER_SAMPLE/8u);

  // Subchunk2Size is the total amount of sample data
  Subchunk2Size = num_samples * NumChannels * (BITS_PER_SAMPLE/8u);
  Subchunk1Size = 16u;
  ChunkSize = 4u + (8u + Subchunk1Size) + (8u + Subchunk2Size);

  // Write the RIFF chunk descriptor
  write_bytes(out, "RIFF", 4u);
  write_u32(out, ChunkSize);
  write_bytes(out, "WAVE", 4u);

  // Write the "fmt " sub-chunk
  write_bytes(out, "fmt ", 4u);       // Subchunk1ID
  write_u32(out, Subchunk1Size);
  write_u16(out, 1u);                 // PCM format
  write_u16(out, NumChannels);
  write_u32(out, SAMPLES_PER_SECOND); // SampleRate
  write_u32(out, ByteRate);
  write_u16(out, BlockAlign);
  write_u16(out, BITS_PER_SAMPLE);

  // Write the beginning of the "data" sub-chunk, but not the actual data
  write_bytes(out, "data", 4);        // Subchunk2ID
  write_u32(out, Subchunk2Size);
}

void read_wave_header(FILE *in, unsigned *num_samples) {
  char label_buf[4];
  uint32_t ChunkSize, Subchunk1Size, SampleRate, ByteRate, Subchunk2Size;
  uint16_t AudioFormat, NumChannels, BlockAlign, BitsPerSample;

  read_bytes(in, label_buf, 4u);
  if (memcmp(label_buf, "RIFF", 4u) != 0) {
    fatal_error("Bad wave header (no RIFF label)");
  }

  read_u32(in, &ChunkSize); // ignore

  read_bytes(in, label_buf, 4u);
  if (memcmp(label_buf, "WAVE", 4u) != 0) {
    fatal_error("Bad wave header (no WAVE label)");
  }

  read_bytes(in, label_buf, 4u);
  if (memcmp(label_buf, "fmt ", 4u) != 0) {
    fatal_error("Bad wave header (no 'fmt ' subchunk ID)");
  }

  read_u32(in, &Subchunk1Size);
  //printf("Subchunk1Size=%u\n", Subchunk1Size);
  if (Subchunk1Size != 16u) {
    fatal_error("Bad wave header (Subchunk1Size was not 16)");
  }

  read_u16(in, &AudioFormat);
  if (AudioFormat != 1u) {
    fatal_error("Bad wave header (AudioFormat is not PCM)");
  }

  read_u16(in, &NumChannels);
  if (NumChannels != NUM_CHANNELS) {
    fatal_error("Bad wave header (NumChannels is not 2)");
  }

  read_u32(in, &SampleRate);
  if (SampleRate != SAMPLES_PER_SECOND) {
    fatal_error("Bad wave header (Unexpected sample rate)");
  }

  read_u32(in, &ByteRate); // ignore

  read_u16(in, &BlockAlign); // ignore

  read_u16(in, &BitsPerSample);
  if (BitsPerSample != BITS_PER_SAMPLE) {
    fatal_error("Bad wave header (Unexpected bits per sample)");
  }

  read_bytes(in, label_buf, 4u);
  if (memcmp(label_buf, "data", 4u) != 0) {
    fatal_error("Bad wave header (no 'data' subchunk ID)");
  }

  // finally we're at the Subchunk2Size field, from which we can
  // determine the number of samples
  read_u32(in, &Subchunk2Size);
  *num_samples = Subchunk2Size / NUM_CHANNELS / (BITS_PER_SAMPLE/8u);
}

void compute_pan(float angle, float channel_gain[]){
    // these formulas are defined in the project description.
    channel_gain[0] = pow(2.0, -0.5) * (cos(angle) + sin(angle));
    channel_gain[1] = pow(2.0, -0.5) * (cos(angle) - sin(angle));
}

void generate_sine_wave(int16_t mono_buf[], unsigned num_samples, float freq_hz) {
    for (unsigned i = 0; i < num_samples; i++) {
        //sine wave that cycles between [-32768, 32767]
        mono_buf[i] =(int16_t)((32767.5) * sin(((float) i / SAMPLES_PER_SECOND) * freq_hz * 2.0 * PI) - 0.5);
    }
}
void generate_square_wave(int16_t mono_buf[], unsigned num_samples, float freq_hz) {
    for (unsigned i = 0; i < num_samples; i++) {
        //we will use this sine wave to determine whether the wave should be CEIL or FLOOR
        int16_t sine_wave = (int16_t)((32767.5) * sin(((float) i / SAMPLES_PER_SECOND) * freq_hz * 2.0 * PI) - 0.5);
        if (sine_wave > 0) {
            mono_buf[i] = 32767.0;
        } else {
            mono_buf[i] = -32768.0;
        }
    }
}
void generate_saw_wave(int16_t mono_buf[], unsigned num_samples, float freq_hz) {
    for (unsigned i = 0; i < num_samples; i++) {
        //first find the position at the wave (divided by cycle length)
        float wavpos = ((float) i / SAMPLES_PER_SECOND) * freq_hz;
        //then find position within that cycle by subtracting the FLOOR of wavpos
        float cyclepos = wavpos - floor(wavpos);
        //increase linearly (cyclepos will cycle from 0-1)
        mono_buf[i] = (int16_t) (-32768.0 + (32768.0 + 32767.0) * (cyclepos));
    }
}

void apply_gain(int16_t mono_buf[], unsigned num_samples, float gain) {
    for (unsigned i = 0; i < num_samples; i++) {
        //multiply all values by specified gain
        mono_buf[i] *= gain;
    }
}

void apply_adsr_envelope(int16_t mono_buf[], unsigned num_samples) {
    //start from beginning of mono_buf, then increase for ATTACK_NUM_SAMPLES times
    int i = 0;
    while (i < ATTACK_NUM_SAMPLES) {
        mono_buf[i] *= (int16_t) (1.2 * (float) i / ATTACK_NUM_SAMPLES);
        i++;
    }
    //start from ATTACK_NUM_SAMPLES, then decrease for DECAY_NUM_SAMPLES times
    while (i < ATTACK_NUM_SAMPLES + DECAY_NUM_SAMPLES) {
        mono_buf[i] *= (int16_t) (1.2 - 0.2 * (float) i / DECAY_NUM_SAMPLES);
        i++;
    }
    //start from end of mono_buf, then increase backwards (from 0)
    int j = 0;
    while (j < RELEASE_NUM_SAMPLES) {
        mono_buf[num_samples - j] *= ((float) j / RELEASE_NUM_SAMPLES);
        j++;
    }
}

void mix_in(int16_t stereo_buf[], unsigned channel, const int16_t mono_buf[], unsigned num_samples) {
    for (int i = 0; i < (int)num_samples; i++) {
        if (((int32_t)stereo_buf[2 * i] + mono_buf[i]) >= 32767) {
            //clip MAX
            stereo_buf[(2 * i) + channel] = 32767;
        } else if ((int32_t)stereo_buf[2 * i] + mono_buf[i] <= -32768) {
            //clip MIN
            stereo_buf[(2 * i) + channel] = -32768;
        } else {
            //otherwise put in respective channels
            stereo_buf[(2 * i) + channel] += mono_buf[i];
        }
    }
}
