CC = gcc
CFLAGS=-std=c99 -pedantic -Wall -Wextra

render_tone: render_tone.o io.o wave.o
	$(CC) $(CFLAGS) -o render_tone render_tone.o io.o wave.o -lm

render_tone.o: render_tone.c io.h wave.h
	$(CC) $(CFLAGS) -c render_tone.c 

render_song: render_song.o io.o wave.o
	$(CC) $(CFLAGS) -o render_song render_song.o io.o wave.o -lm

render_song.o: render_song.c io.h wave.h
	$(CC) $(CFLAGS) -c render_song.c

io.o: io.c io.h
	$(CC) $(CFLAGS) -c io.c

wave.o: wave.c wave.h
	$(CC) $(CFLAGS) -c wave.c

render_echo: render_echo.o io.o wave.o
	$(CC) $(CFLAGS) -o render_echo render_echo.o io.o wave.o -lm

render_echo.o: render_echo.c io.h wave.h
	$(CC) $(CFLAGS) -c render_echo.c

clean:
	rm -f *.o main render_tone render_song render_echo *~
