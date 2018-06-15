#ifndef INODE_H
#define INODE_H

#include "bitmap.h"

// inode section start from block 28
#define START_BLOCK 28
// 16 nodes in one block
#define NODES_IN_BLOCK 16
// Each inode is 64Bytes
#define NODESIZE 64
// Number of addresses in one layer index block
#define ONE_LAYER_INDEX_NUM 256
// Number of direct indexed blocks
#define DIRECT_INDEX_NUM 13
// Max block number an inode can have
#define MAX_BLOCK_COUNT 65536

typedef
struct {

} disk_node;

typedef
struct {
    int block_count;
    int direct_index[DIRECT_INDEX_NUM];
    int one_layer_index;
    int two_layer_index;
} memory_node;

void initial_inode(memory_node *node, bitmap *map);

void load_inode(memory_node *node, int inode_num);

void dump_inode(memory_node node, int inode_num);

void write_inode(memory_node *node, char *src, bitmap *map);

void writen_inode(memory_node *node, char *src, bitmap *map, int n);

void read_inode(memory_node *node, char *src);

void readn_inode(memory_node *node, char *src, int n);

#endif