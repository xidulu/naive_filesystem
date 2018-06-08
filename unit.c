#include "bitmap.h"
#include "directory.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Unit test for bitmap module
void test_bitmap()
{
    bitmap *test = create_bitmap(1024);
    int i;
    for (i = 0; i < 20; i = i + 2)
    {
        set_bit(test, i, 1);
    }
    int a[15];
    allocate_bits(test, a, 15);
    for(i = 0; i < 15; i++) {
        printf("%d ", a[i]);
    }
    printf("\n");
}

void test_directory() {
    char *test_directory = "taco,16,burrito,9,";
    char *test_filename = "fajita";
    char *result = addfile(test_directory, test_filename, 15);
    printf("Testing add:\n%s\n", result);
    int i = search_file(result, "burrito");
    printf("Testing search:\ninode: %d\n", i);
    char *result2 = removefile(result, "fajita");
    printf("Testing remove:\n%s\n", result);
    char *result3 = removefile(result2, "taco");
    char *result4 = removefile(result3, "burrito");
    printf("Testing empty check:\n%d\n", is_empty(result4));
    free(result);
    free(result2);
    free(result3);
    free(result4);
}

int main(int argc, char const *argv[])
{
    //test_bitmap();
    test_directory();
    return 0;
}