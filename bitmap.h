#if !defined(BITMAP_H)
#define BITMAP_H

typedef struct bitmap bitmap;
struct bitmap
{
    char* bits;
    int size;
};

bitmap* create_bitmap(int n);

void allocate_bits(bitmap *map, int *a, int n);

void set_bit(bitmap *map, int n, int x);

int get_bit(bitmap *map, int index);

int free_bits(bitmap *map, int *a);

void dump_bitmap(bitmap *map, int start, int len);

void load_bitmap(bitmap *map);

#endif // BITMAP_H
