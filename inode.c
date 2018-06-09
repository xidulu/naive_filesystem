#include "inode.h"
#include "bitmap.h"
#include "p5.h"
#include <stdlib.h>
#include <string.h>

// @author:Xi Wang
// Config for inode section is defined in inode.h
// Both load and dump operations
// use block-level api provided by block.c.
// When facing the (re)allocation of blocks,
// we use the following strategy:
//  If no block has been allocated before(first allocation),
//  just simply allocate some blocks, and link them to the index.
//  If we have already allocated blocks to the inode(reallocation),
//  we try to keep as many pre-allocated blocks as possible,
//  and then free the extra blocks(or allocate new blocks).


// Helper function,
// return the n_th block's block_number
static int find_block_address(memory_node *node, int n) {
    // FIXME!
    return 0;
}


// Helper function,
// reallocate the NODE's block to NUM blocks.
// (blocks are managed by MAP)
static void reallocate_blocks(memory_node* node, bitmap* map, int num) {
    // FIXME!
}


// Read the i_th inode from the disk,
// copy the value into NODE.
void load_inode(memory_node *node, int inode_num) {
    int offset = (inode_num % NODES_IN_BLOCK) * NODESIZE;
    int block_num = (inode_num / NODES_IN_BLOCK) + START_BLOCK;
    char buffer[1024];
    read_block(block_num, buffer);
    memcpy(node, buffer + offset, NODESIZE);
}


// Dump NODE as the i_th inode on the disk.
void dump_inode(memory_node node, int inode_num) {
    int offset = (inode_num % NODES_IN_BLOCK) * NODESIZE;
    int block_num = (inode_num / NODES_IN_BLOCK) + START_BLOCK;
    char buffer[1024];
    read_block(block_num, buffer);
    memcpy(buffer + offset, (char*)&node, sizeof(node));
    write_block(block_num, buffer);
}

// Write SRC to blocks managed by MAP
void write_inode(memory_node* node, char* src, bitmap* map) {
    int block_count = (strlen(src) / BLOCKSIZE) + 1; 
    reallocate_blocks(node, map, block_count);
    int i;
    char buffer[1024];
    for(i = 0; i < block_count; i++) {
        strncpy(buffer, src + (i * BLOCKSIZE), BLOCKSIZE);
        write_block(find_block_address(node, i),
                    buffer);
    }
}


// Read all the blocks that belong to NODE,
// store the result in SRC.
void read_inode(memory_node *node, char* src) {
    int size = node->block_count;
    char buffer[1024];
    int i;
    for(i = 0; i < size; i++) {
        int num = find_block_address(node, i);
        read_block(num, buffer);
        strcat(src, buffer);
    }
}