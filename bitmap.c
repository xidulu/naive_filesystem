#include "bitmap.h"
#include "p5.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// Create a bitmap struct with n bits
// return a pointer to the bitmap.
bitmap* create_bitmap(int n) {
    int actual_size = sizeof(char) * (n / 8);
    bitmap* new_map = (bitmap*)malloc(sizeof(bitmap));
    new_map->bits = (char*)malloc(actual_size); 
    new_map->size = n;
    return new_map;
}


// Allocate n bits in the bitmap
// with a certain policy,
// save the result in a given array.
void allocate_bits(bitmap* map, int* a, int n) {
    int* list = (int*)malloc(n * sizeof(int));
    int size = map->size;
    int count = 0;
    int i;
    printf("size: %d \n", size);
    for(i = 0; i < size; i++) {
        if (!get_bit(map, i)) {
            a[count++] = i;
            set_bit(map, i, 1);
        }
        if (count == n) {
            break;
        }
    }
}

// Set the n_th bit to x.
void set_bit(bitmap* map, int n, int bit) {
    int byte = n >> 3;
    int i = sizeof(n) * 8 - 3;
    int offset = ((unsigned) n << i) >> i;
    if (bit) {
        map->bits[byte] |= 1 << (7 - offset);
    }
    else {
        map->bits[byte] &= ~(1 << (7 - offset));
    }
}

// Return the value or the index_th bit.
int get_bit(bitmap* map, int index) {
    int byte = index >> 3;
    int n = sizeof(index) * 8 - 3;
    int offset = ((unsigned)index << n) >> n;
    if (map->bits[byte] & (1 << (7 - offset))) {
        return 1;
    }
    else{
        return 0;
    }
}


// Free bits in a given position.
int free_bits(bitmap* map, int* a) {
    size_t len = sizeof(a) / sizeof(int);
    size_t i;
    for(i = 0; i < len; i++) {
        set_bit(map, a[i], 0);
    }
}

// Serialize the bitmap into given blocks,
// using block api.
// Wirte "len" blocks starting from "start"
void dump_bitmap(bitmap* map, int start, int len) {

}


// Deserialize the bitmap. 
void load_bitmap(bitmap* map) {
    
}