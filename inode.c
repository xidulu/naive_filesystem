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


// A buffer for writing index block num to the disk,
// the queue will be dumped into disk if full.
static char _buffer_queue[BLOCKSIZE];
static int _buffer_queue_num = -1;
static int _buffer_queue_dumped = 0;

static char _two_layer_index_cache[1024] = {0};
static int  _two_layer_index_cache_num = -1;

// Helper function,
// add a new block B(block number) to NODE's index,
// the procedure is similar with FIND_BLOCK_ADDRESS().
// New index block will be allocated if needed,
// lazy policy is applied here:
//  Once allocated, an index block will not be released
//  until the inode is deleted.
//  (e.g: a file shrink from 40Mb to 1Kb, two layer index blocks will be kept).
static void link_block2node(memory_node *node, bitmap *map, int b) {
    int count = node->block_count;
    if (count < DIRECT_INDEX_NUM) {
        node->direct_index[count] = b;
        node->block_count++;
        return;
    }
    if (count < DIRECT_INDEX_NUM + ONE_LAYER_INDEX_NUM) {
        if (node->one_layer_index == -1) {
            int a[1];
            allocate_bits(map, a, 1);
            node->one_layer_index = a[0];
        }
        int offset = (count - DIRECT_INDEX_NUM) * sizeof(int);
        char buffer[1024];
        read_block(node->one_layer_index, buffer);
        int address;
        memcpy(buffer + offset,
                &b,
                4);
        write_block(node->one_layer_index, buffer);
        node->block_count++;
    } else {
        if (node->two_layer_index == -1) {
            int a[257];
            allocate_bits(map, a, 257);
            node->two_layer_index = a[0];
            char buffer[1024];
            int i;
            for(i = 1; i < 257; i++) {
                memcpy(buffer + (i - 1) * sizeof(int),
                        a + i, sizeof(int));
            }
            write_block(node->two_layer_index, buffer);
        }
        int offset = count - DIRECT_INDEX_NUM - ONE_LAYER_INDEX_NUM;
        int index_num = offset / ONE_LAYER_INDEX_NUM;
        int index_offset = offset % ONE_LAYER_INDEX_NUM;
        char buffer[1024];
        if (_two_layer_index_cache_num == node->two_layer_index) {
            memcpy(buffer, _two_layer_index_cache, BLOCKSIZE);
        } else {
            read_block(node->two_layer_index, buffer);
            memcpy(_two_layer_index_cache, buffer, BLOCKSIZE);
            _two_layer_index_cache_num = node->two_layer_index;
        }
        int index_address;
        memcpy(&index_address,
                buffer + (index_num) * sizeof(int),
                4);
        if (_buffer_queue_num == index_address) {
            memcpy(_buffer_queue + (index_offset) * sizeof(int),
                   &b,
                   4);
            if (index_offset == (ONE_LAYER_INDEX_NUM - 1)) {
                write_block(_buffer_queue_num, _buffer_queue);
                memset(_buffer_queue, 0, sizeof(_buffer_queue));
                _buffer_queue_dumped = 1;
            }
        } else {
            _buffer_queue_dumped = -1;
            read_block(index_address, _buffer_queue);
            _buffer_queue_num = index_address;
            memcpy(_buffer_queue + (index_offset) * sizeof(int),
                   &b,
                   4);
        }
        node->block_count++;
    }
}

// Helper function,
// return the n_th block's block_number,
// return -1 if not found.
static int find_block_address(memory_node *node, int n) {
    if (n < DIRECT_INDEX_NUM) {
        return node->direct_index[n];
    }
    if (n < DIRECT_INDEX_NUM + ONE_LAYER_INDEX_NUM) {
        int offset = (n - DIRECT_INDEX_NUM) * sizeof(int);
        char buffer[1024];
        read_block(node->one_layer_index, buffer);
        int address;
        memcpy(&address,
                buffer + offset,
                4);
        return address;
    } else {
        int offset = n - DIRECT_INDEX_NUM - ONE_LAYER_INDEX_NUM;
        int index_num = offset / ONE_LAYER_INDEX_NUM;
        int index_offset = offset % ONE_LAYER_INDEX_NUM;
        char buffer[1024];
        read_block(node->two_layer_index, buffer);
        int index_address;
        memcpy(&index_address,
                buffer + (index_num) * sizeof(int),
                4);
        read_block(index_address, buffer);
        int block_address;
        memcpy(&block_address,
                buffer + (index_offset) * sizeof(int),
                4);
        return block_address;
    }

    return -1;
}


// Helper function,
// reallocate number of the NODE's block to NUM blocks.
// (blocks are managed by MAP)
// We are trying to allocate as few new blocks as possible.
static void reallocate_blocks(memory_node* node, bitmap* map, int num) {
    int current_block_count = node->block_count;
    if (num < current_block_count) {
        int i;
        for(i = num; i < current_block_count; i++) {
            int address = find_block_address(node, i);
            free_bit(map, address);
        }
        return;
    }
    if (num > current_block_count) {
        int t = num - current_block_count;
        // node->block_count = num;
        int extra_blocks[MAX_BLOCK_COUNT];
        allocate_bits(map, extra_blocks, t);
        int i;
        for(i = 0; i < t; i++) {
            link_block2node(node, map, extra_blocks[i]);
        }
        if (_buffer_queue_dumped == -1) {
            write_block(_buffer_queue_num, _buffer_queue);
        }
        return;
    }
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
        memcpy(buffer, src + (i * BLOCKSIZE), BLOCKSIZE);
        write_block(find_block_address(node, i),
                    buffer);
    }
}

// Write n bytes of src.
void writen_inode(memory_node *node, char *src, bitmap *map, int n) {
    int block_count = (n / BLOCKSIZE) + 1;
    reallocate_blocks(node, map, block_count);
    int i;
    char buffer[1024];
    
    for (i = 0; i < block_count; i++)
    {
        memcpy(buffer, src + (i * BLOCKSIZE), BLOCKSIZE);
        int block = find_block_address(node, i);
        write_block(block,
                    buffer);
    }
}

// Read all the blocks that belong to NODE,
// store the result in SRC.
void read_inode(memory_node *node, char* src) {
    int size = node->block_count;
    char buffer[1024];
    src[0] = '\0';
    int i;
    for(i = 0; i < size; i++) {
        int num = find_block_address(node, i);
        read_block(num, buffer);
        memcpy(src + i * BLOCKSIZE, buffer, BLOCKSIZE);
    }
    return;
}

// Read n bytes from the node,
// basically a wrapper of READ_INODE()
void readn_inode(memory_node *node, char* src, int n) {
    read_inode(node, src);
    memcpy(src, src, n);
}

// Intialization of NODE,
// set index to -1 (not allocated).
void initial_inode(memory_node *node, bitmap* map) {
    node->block_count = 0;
    node->one_layer_index = -1;
    node->two_layer_index = -1;
}