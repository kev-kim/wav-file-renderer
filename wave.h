#ifndef WAVE_H
#define WAVE_H

#include <stdint.h>

#define PI                 3.14159265358979323846
#define SAMPLES_PER_SECOND 44100u
#define NUM_CHANNELS       2u
#define BITS_PER_SAMPLE    16u

// waveforms
#define SINE          0
#define SQUARE        1
#define SAW           2
#define NUM_WAVEFORMS 3 // one greater than maximum legal waveform

// timing characteristics for ADSR envelope
#define ATTACK_NUM_SAMPLES  882
#define DECAY_NUM_SAMPLES   882
#define RELEASE_NUM_SAMPLES 882

// Functions for writing and reading a WAVE header.
void write_wave_header(FILE *out, unsigned num_samples);
void read_wave_header(FILE *in, unsigned *num_samples);

// Compute appropriate gains for a stereo pan at given angle in radians.
// Left and right channel gains are stored in the channel_gain array.
void compute_pan(float angle, float channel_gain[]);

// Basic full-amplitude wave generation into a mono sample buffer
void generate_sine_wave(int16_t mono_buf[], unsigned num_samples, float freq_hz);
void generate_square_wave(int16_t mono_buf[], unsigned num_samples, float freq_hz);
void generate_saw_wave(int16_t mono_buf[], unsigned num_samples, float freq_hz);

// Attenuate each sample in a mono sample buffer by specified factor
void apply_gain(int16_t mono_buf[], unsigned num_samples, float gain);

// Apply an ADSR envelope to a mono sample buffer. This should only
// be done *after* attenuating the contents of the buffer to a reasonable
// level, since the peak of the attack will actually increase the
// signal's amplitude.
void apply_adsr_envelope(int16_t mono_buf[], unsigned num_samples);

// Mix a mono sample buffer into a stereo stream.
// channel should be 0 (left) or 1 (right).
// stereo_buf should be pointing to a left-channel sample.
void mix_in(int16_t stereo_buf[], unsigned channel, const int16_t mono_buf[], unsigned num_samples);

#endif // WAVE_H
