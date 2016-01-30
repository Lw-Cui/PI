#ifndef _OPERATION_H_
#define _OPERATION_H_
const int SHIFT = 32;
const unsigned long long BASE = (unsigned long long)1 << SHIFT;
extern int VALID;
extern int LEN;

void Pi_carry(unsigned long long Pi[]);
void add(unsigned long long dest[], unsigned src[]);
void minus(unsigned long long dest[], unsigned src[]);
void multiply(unsigned dest[], unsigned num);
void divide(unsigned dest[], unsigned num);
void output(unsigned long long array[]);
void print(unsigned num);
void shift_right(unsigned sub[], unsigned num);

#endif
