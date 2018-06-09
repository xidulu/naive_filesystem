#ifndef INODE_H
#define INODE_H

#include "bitmap.h"

// inode section start from block 4
#define START_BLOCK 4
// 16 nodes in one block
#define NODES_IN_BLOCK 16
// Each inode is 64Kb
#define NODESIZE 64

typedef 
struct
{
    
} disk_node;

typedef 
struct
{
    int block_count;
    int direct_index[13];
    int one_layer_index;
    int two_layer_index;
} memory_node;


void load_inode(memory_node* node, int inode_num);

void dump_inode(memory_node node, int inode_num);

void write_inode(memory_node* node, char* src, bitmap* map);

char* read_inode(memory_node* node, int bytes);

#endif