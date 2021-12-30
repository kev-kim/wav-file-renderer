#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "io.h"

void fatal_error(const char *message) {
    //prints error message, then closes program.
    printf("Error: %s\n", message);
    exit(1);
}

void write_byte(FILE *out, char val) {
    //writes a single char 'val' into FILE *out
    if (fwrite(&val, sizeof(char), 1, out) != 1) {
        //calls on fatal error if it cannot write the character.
        fatal_error("write_byte cannot execute");
    }
}

void write_bytes(FILE *out, const char data[], unsigned n) {
    //writes the array of characters 'data' into FILE *out
    if (fwrite(data, 1, n, out) != n) {
        //calls on fatal error if it writes less than or more than
        //n, the number of characters in the array.
        fatal_error("write_bytes cannot execute");
    }
}

void write_u16(FILE *out, uint16_t value) {
    //writes a single uint16_t 'value' into FILE *out/
    if (fwrite(&value, sizeof(uint16_t), 1, out) != 1) {
        //calls on fatal error if it cannot write the uint16_t.
        fatal_error("write_u16 cannot execute");
    }
}

void write_u32(FILE *out, uint32_t value) {
     //writes a single uint32_t 'value' into FILE *out/
     if (fwrite(&value, sizeof(uint32_t), 1, out) != 1) {
         //calls on fatal error if it cannot write the uint32_t.
         fatal_error("write_u32 cannot execute");
     }
}

void write_s16(FILE *out, int16_t value) {
     //writes a single int16_t 'value' into FILE *out/
     if (fwrite(&value, sizeof(int16_t), 1, out) != 1) {
         //calls on fatal error if it cannot write the int16_t.
         fatal_error("write_s16 cannot execute");
     }
}

void write_s16_buf(FILE *out, const int16_t buf[], unsigned n) {
    //writes an array of n characters into FILE *out by repeatedly calling 
    //on the write_s16 function in a for loop.
    for (unsigned i = 0; i < n; i++) {
        write_s16(out, buf[i]);
    }
}

void read_byte(FILE *in, char *val) {
    //reads a single char from FILE *in and stores it to the pointer val.
    if (fread(val, sizeof(char), 1, in) != 1) {
        //calls on fatal error if it does not read exactly 1 char.
        fatal_error("read_byte cannot execute");
    }
}

void read_bytes(FILE *in, char data[], unsigned n) {
     //reads an array of chars from FILE *in and stores it to the
     //array data. We expect to read n characters.
     if (fread(data, 1, n, in) != n) {
         //calls on fatal error if it does not read exactly n chars.
         fatal_error("read_bytes cannot execute");
     }   
}

void read_u16(FILE *in, uint16_t *val) {
     //reads a single uint16_t from FILE *in and stores it to the pointer val.
     if (fread(val, sizeof(uint16_t), 1, in) != 1) {
         //calls on fatal error if it does not read exactly 1 uint16_t.
         fatal_error("read_u16 cannot execute");
     }   
}

void read_u32(FILE *in, uint32_t *val) {
     //reads a single uint32_t from FILE *in and stores it to the pointer val.
     if (fread(val, sizeof(uint32_t), 1, in) != 1) {
         //calls on fatal error if it does not read exactly 1 uint32_t.
         fatal_error("read_u32 cannot execute");
     }   
}

void read_s16(FILE *in, int16_t *val) {
     //reads a single int16_t from FILE *in and stores it to the pointer val.
     if (fread(val, sizeof(int16_t), 1, in) != 1) {
         //calls on fatal error if it does not read exactly 1 int16_t.
         fatal_error("read_s16 cannot execute");
     }   
}

void read_s16_buf(FILE *in, int16_t buf[], unsigned n) {
     //reads an array of n characters from FILE *in and stores them
     //in an array buf[], passed into the function.
     for (unsigned i = 0; i < n; i++) {
         read_s16(in, &buf[i]);
     }
}

