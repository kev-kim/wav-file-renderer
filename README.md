# WAVE File Renderer

This is a suite of C programs that allow the user to read WAVE files into C arrays, generate simple audio waveforms (sine, square, saw), and then perform manipulations on these files such as an echo, or combining the waves into full songs.

## Usage

There are three main programs:

1. render_tone
2. render_song
3. render_echo

To run any of these, run `make target` where *target* is the program desired.

---

### render_tone

This program is invoked as such: ```./render_tone waveform frequency amplitude numsamples wavfileout```

- **waveform** is 0 for sine wave, 1 for square wave, and 2 for saw wave
- **frequency** is a float value that speicifies the frequency of the tone in Hz
- **amplitude** is a float value between 0 and 1, which decides on the volume of the wave
- **numsamples** is the number of stereo samples generated (44,100 samples is 1 second)
- **wavfileout** is the name of the WAVE file generated with the specified tone

For example, to generate a sine wave of 440 Hz that lasts 1 second (with a reasonable volume), the program would be called like this: ```./render_tone 0 440.0 0.1 44100 a440_sine.wav```

---

### render_song

This program is invoked as such: ```./render_song songinput waveoutput```

- **songinput** is a text file that has a series of instructions (more on that below)
- **waveoutput** is the name of the WAVE file generated with the input file

The *songinput* file utilizes is a text file that begins with the number of samples, then each line contains a series of directives followed by a set of numbers.

- N instrument start end note gain: render a note using specified instrument, starting at sample number start, ending just before sample end, playing MIDI note number note, with specified gain factor
- W instrument waveform: set the waveform of specified instrument, 0 for sine, 1 for square, 2 for sawtooth
- P instrument angle: set the stereo pan angle (in radians) of specified instrument
- E instrument enable: enable or disable ADSR envelope on specified instrument (0 to disable, 1 to enable)
- G instrument gain: set the gain factor (0.0 to 1.0) of specified instrument

Here is a simple song input file:
```
352800

W 0 1
E 0 1
P 0 -0.4

W 1 2
E 1 1
P 1 0.4

N 0 0 8820 63 0.8
N 0 0 8820 66 0.8
N 1 22050 30870 61 0.8
N 1 22050 30870 65 0.8
N 0 44100 52920 63 0.8
N 0 44100 52920 66 0.8
N 1 66150 74970 61 0.8
N 1 66150 74970 65 0.8
N 0 88200 97020 63 0.8
N 0 88200 97020 66 0.8
N 1 110250 119070 61 0.8
N 1 110250 119070 65 0.8
N 0 132300 141120 63 0.8
N 0 132300 141120 66 0.8
N 1 154350 163170 61 0.8
N 1 154350 163170 65 0.8

N 0 176400 194040 66 0.8
N 0 176400 194040 70 0.8
N 1 220500 238140 66 0.8
N 1 220500 238140 70 0.8
N 0 264600 282240 66 0.8
N 0 264600 282240 70 0.8
```

---

### render_echo

The program is invoked as such: `./render_echo wavfilein wavfileout delay amplitude`

- **wavfilein** is the name of the input WAVE file
- **wavfileout** is the name of the output WAVE file
- **delay** is an integer for the number of samples to delay the echo by
- **amplitude** is a float between 0 and 1 that specifies the relative amplitude of the echo

For example, render_echo can be called on linux.wav in the test folder (the famous audio by Linus Torvalds himself!) with a 0.5 second delay like this: `./render_echo test/linux.wav linux_echo.wav 22050 0.4`

---

The resulting audio and inputs for all the programs are in the test folder.

Created with Joshua Hahn of Columbia University.
