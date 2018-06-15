#if !defined(BITMAP_H)
#define BITMAP_H

typedef struct bitmap bitmap;
struct bitmap {
    char *bits;
    int size;
    int offset;
};

bitmap *create_bitmap(int n, int offset);

void allocate_bits(bitmap *map, int *a, int n);

// void set_bit(bitmap *map, int n, int x);

int get_bit(bitmap *map, int index);

void free_bits(bitmap *map, int *a);

void free_bit(bitmap *map, int n);

void dump_bitmap(bitmap *map, int start, int len);

void load_bitmap(bitmap *map);

#endif // BITMAP_H
