#ifndef IO_H
#define IO_H

#include <stdint.h>

void fatal_error(const char *message);
void write_byte(FILE *out, char val);
void write_bytes(FILE *out, const char data[], unsigned n);
void write_u16(FILE *out, uint16_t value);
void write_u32(FILE *out, uint32_t value);
void write_s16(FILE *out, int16_t value);
void write_s16_buf(FILE *out, const int16_t buf[], unsigned n);

void read_byte(FILE *in, char *val);
void read_bytes(FILE *in, char data[], unsigned n);
void read_u16(FILE *in, uint16_t *val);
void read_u32(FILE *in, uint32_t *val);
void read_s16(FILE *in, int16_t *val);
void read_s16_buf(FILE *in, int16_t buf[], unsigned n);

#endif /* IO_H */
