#include <stdio.h>
//knuths hash function
unsigned int hash(unsigned int x) {
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x);
    return x;
}
int hashKnuth(int i){

	return (i*2654435761) % 2^32;
}