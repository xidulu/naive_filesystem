#include "bitmap.h"
#include "directory.h"
#include "inode.h"
#include "p5.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char buffer5[50 * 1024 - 100];
char buffer6[51 * 1024];

static char *parse_path(char *path, char *buffer) {
    char *temp = path;
    buffer[0] = '\0';
    if (temp[0] == '/') {
        temp++;
    }
    while ((temp[0] != '/') && (temp[0] != '\0')) {
        strncat(buffer, temp, 1);
        temp++;
    }
    return temp;
}

// Unit test for bitmap module
void test_bitmap() {
    bitmap *test = create_bitmap(1024, 5);
    int i;

    for (i = 0; i < 15; i++) {
        int a[1];
        allocate_bits(test, a, 1);
        printf("%d ", a[0]);
    }
    printf("\n");
}

void test_directory() {
    char *test_directory = "bar,2";
    char *test_filename = "bar";
    int i = search_file(test_directory, test_filename);
    printf("Testing search:\ninode: %d\n", i);
}

void test_inode() {
    dev_open();
    memory_node a;
    bitmap *test = create_bitmap(60000, 0);
    initial_inode(&a, test);
    char *stuff = "suck my dick";
    write_inode(&a, stuff, test);
    char src[100];
    read_inode(&a, src);
    printf("%s \n", src);
    int i;
    for (i = 0; i < 50 * 1024 - 100; i++) {
        buffer5[i] = 'F';
    }
    printf("%d\n", strlen(buffer5));
    write_inode(&a, buffer5, test);
    printf("%d\n", a.block_count);
    read_inode(&a, buffer6);
    printf("%d\n", strlen(buffer6));
    printf("%d\n", strcmp(buffer5, buffer6));
    // memory_node *t = (memory_node *)malloc(sizeof(memory_node));
    // dump_inode(a, 0);
    // load_inode(t, 0);
    // printf("%d %d \n", t->one_layer_index, t->two_layer_index);
}

int main(int argc, char const *argv[]) {
    //test_bitmap();
    //test_directory();
    //test_inode();
    // char* path="/root/taco/nacho";
    // char buffer[1024];
    // char* temp = path;
    // while(1) {
    // temp = parse_path(temp, buffer);
    // printf("%s\n", buffer);
    // if (temp[0] == '\0') {
    // break;
    // }
    // }
    // dev_open();
    // my_mkfs();
    // int i = my_mkdir("/foo");
    // printf("1%d\n", i);
    // i = my_mkdir("/bar/foo");
    // printf("2%d\n", i);
    // i = my_mkdir("/foo/qcqp");
    // printf("3%d\n", i);
    // i = my_rmdir("/foo/qcqp");
    // printf("4 %d\n", i);
    // i = my_mkdir("/foo/qcqp/temp");
    // printf("5 %d\n", i);
    // i = my_mkdir("/foo/qcqp");
    // printf("6 %d\n", i);
    // // i = my_mkdir("/foo/ellipsoid");
    // // printf("%d\n", i);
    // i = my_creat("/foo/temp.txt");
    // printf("7 %d\n", i);
    // i = my_creat("/foo/temp2.txt");
    // printf("8 %d\n", i);
    // i = my_creat("/foo/temp3.txt");
    // printf("9 %d\n", i);
    // i = my_open("/foo/temp4.txt");
    // printf("10 %d\n", i);
    // return 0;
    // test_bitmap();
    test_directory();
    return 0;
}