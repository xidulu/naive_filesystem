
#include "bitmap.h"
#include "directory.h"
#include "inode.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define SUPERBLOCK_START 0
#define SUPERBLOCK_SIZE 1
#define NODEMAP_START 1
#define NODEMAP_SIZE 1
#define BITMAP_START 2
#define BITMAP_SIZE 25
#define DATA_BLOCK_START 410
#define ROOT_DIRECTORY_NODE 0;

static char *signature = "MAY THE FORCE BE WITH YOU";
static bitmap *blockmap;
static bitmap *nodemap;
int _filetablesize = 0;
static memory_node* _filetable[100];
char general_buffer[1024 * 50];
memory_node *root_directory;
static int end_of_path = 0;
static int _offsettable[100];

// Parse a path-like string,
// save the result in buffer,
// return a pointer to the next token.
static char *parse_path(char *path, char *buffer)
{
  char *temp = path;
  buffer[0] = '\0';
  if (temp[0] == '/')
  {
    temp++;
  }
  while ((temp[0] != '/') && (temp[0] != '\0'))
  {
    strncat(buffer, temp, 1);
    if (temp[0] == '\0')
    {
      end_of_path = 1;
    }
    temp++;
  }
  return temp;
}

/* open an exisiting file for reading or writing */
int my_open(char *path)
{
  char *temp = path;
  char buffer[2048];
  int cwd_node_num = ROOT_DIRECTORY_NODE;
  memory_node *cwd_node = (memory_node *)malloc(sizeof(memory_node));
  while (1)
  {
    temp = parse_path(temp, buffer);
    load_inode(cwd_node, cwd_node_num);
    read_inode(cwd_node, general_buffer);
    if (temp[0] == '\0')
    {
      int find = search_file(general_buffer, buffer);
      if (find == -1) {
        return -1;
      } else {
        _filetable[_filetablesize++] = find;
        return (_filetablesize - 1);
      }
    }
    else
    {
      int find = search_file(general_buffer, buffer);
      if (find == -1)
      {
        return -1;
      }
      else
      {
        cwd_node_num = find;
      }
    }
  }
  return -1;
}

/* open a new file for writing only */
int my_creat(char *path)
{
  char *temp = path;
  char buffer[2048];
  int cwd_node_num = ROOT_DIRECTORY_NODE;
  memory_node *cwd_node = (memory_node *)malloc(sizeof(memory_node));
  while (1)
  {
    temp = parse_path(temp, buffer);
    load_inode(cwd_node, cwd_node_num);
    read_inode(cwd_node, general_buffer);
    if (temp[0] == '\0')
    {
      if (search_file(general_buffer, buffer) != -1)
      {
        return -1;
      }
      memory_node *new_node = (memory_node *)malloc(sizeof(memory_node));
      int a[1];
      allocate_bits(nodemap, a, 1);
      initial_inode(new_node, blockmap);
      dump_inode(*new_node, a[0]);
      char *new_directory;
      new_directory = addfile(general_buffer, buffer, a[0]);
      write_inode(cwd_node, new_directory, blockmap);
      dump_inode(*cwd_node, cwd_node_num);
      // free(new_directory);
      // free(cwd_node);
      _filetable[_filetablesize++] = a[0];
      // free(new_directory);
      return _filetablesize - 1;
    }
    else
    {
      int find = search_file(general_buffer, buffer);
      if (find == -1)
      {
        return -1;
      }
      else
      {
        cwd_node_num = find;
      }
    }
  }
  return -1;
}

/* sequentially read from a file */
int my_read(int fd, void *buf, int count)
{
  char* buf2 = (char*)buf;
  int node_num = _filetable[fd];
  memory_node *node = (memory_node *)malloc(sizeof(memory_node));
  load_inode(node, node_num);
  read_inode(node, general_buffer);
  int start = _offsettable[fd];
  int i = start;
  for (; i < start + count; i++)
  {
    buf2[i - start] = general_buffer[i];
  }
  _offsettable[fd] += count;
  return count;
}

/* sequentially write to a file */
int my_write(int fd, void *buf, int count)
{
  int node_num = _filetable[fd];
  memory_node* node = (memory_node*)malloc(sizeof(memory_node));
  load_inode(node, node_num);
  memset(general_buffer, 0, sizeof(general_buffer));
  read_inode(node, general_buffer);
  int start = _offsettable[fd];
  int i = start;
  for(; i < start + count; i++) {
    general_buffer[i] = (char)*((char*)buf + i - start);
  }
  writen_inode(node, general_buffer, blockmap, count);
  dump_inode(*node, node_num);
  _offsettable[fd] += count;
  return count;
}

int my_close(int fd)
{
  if (_filetable[fd] == -1)
  {
    return -1;
  }
  else
  {
    _filetable[fd] = -1;
    return 1;
  }
}

int my_remove(char *path)
{
  char *temp = path;
  char buffer[2048];
  int cwd_node_num = ROOT_DIRECTORY_NODE;
  memory_node *cwd_node = (memory_node *)malloc(sizeof(memory_node));
  while (1)
  {
    temp = parse_path(temp, buffer);
    load_inode(cwd_node, cwd_node_num);
    read_inode(cwd_node, general_buffer);
    if (temp[0] == '\0')
    {
      int find = search_file(general_buffer, buffer);
      if (find == -1)
      {
        return -1;
      }
      else
      {
        char *new_directory;
        new_directory = removefile(general_buffer, buffer);
        write_inode(cwd_node, new_directory, blockmap);
        free(new_directory);
        free(cwd_node);
        return find;
      }
    }
    else
    {
      int find = search_file(general_buffer, buffer);
      if (find == -1)
      {
        return -1;
      }
      else
      {
        cwd_node_num = find;
      }
    }
  }
  return -1;
}

int my_rename(char *old, char *new)
{
  int node_num = my_remove(old);
  if (node_num == -1) {
    return -1;
  }
  char *temp = new;
  char buffer[2048];
  int cwd_node_num = ROOT_DIRECTORY_NODE;
  memory_node *cwd_node = (memory_node *)malloc(sizeof(memory_node));
  while (1)
  {
    temp = parse_path(temp, buffer);
    load_inode(cwd_node, cwd_node_num);
    read_inode(cwd_node, general_buffer);
    if (temp[0] == '\0')
    {
      if (search_file(general_buffer, buffer) != -1)
      {
        return -1;
      }
      memory_node *new_node = (memory_node *)malloc(sizeof(memory_node));
      initial_inode(new_node, blockmap);
      dump_inode(*new_node, node_num);
      char *new_directory;
      new_directory = addfile(general_buffer, buffer, node_num);
      write_inode(cwd_node, new_directory, blockmap);
      dump_inode(*cwd_node, cwd_node_num);
      free(new_directory);
      free(cwd_node);
      return 1;
    }
    else
    {
      int find = search_file(general_buffer, buffer);
      if (find == -1)
      {
        return -1;
      }
      else
      {
        cwd_node_num = find;
      }
    }
  }
  return ;
}

/* only works if all but the last component of the path already exists */
int my_mkdir(char *path)
{
  char *temp = path;
  char buffer[2048];
  int cwd_node_num = ROOT_DIRECTORY_NODE;
  memory_node *cwd_node = (memory_node*)malloc(sizeof(memory_node));
  while (1)
  {
    temp = parse_path(temp, buffer);
    load_inode(cwd_node, cwd_node_num);
    read_inode(cwd_node, general_buffer);
    if (temp[0] == '\0')
    {
      if (search_file(general_buffer, buffer) != -1)
      {
        return -1;
      }
      memory_node *new_node = (memory_node *)malloc(sizeof(memory_node));
      int a[1];
      allocate_bits(nodemap, a, 1);
      initial_inode(new_node, blockmap);
      dump_inode(*new_node, a[0]);
      char *new_directory;
      new_directory = addfile(general_buffer, buffer, a[0]);
      write_inode(cwd_node, new_directory, blockmap);
      dump_inode(*cwd_node, cwd_node_num);
      // free(new_directory);
      // free(cwd_node);
      return 1;
    }
    else
    {
      int find = search_file(general_buffer, buffer);
      if (find == -1)
      {
        return -1;
      }
      else
      {
        cwd_node_num = find;
      }
    }
  }
  printf("wtf?\n");
  return -1;
}


// Free empty node currently not implemented.
int my_rmdir(char *path)
{
  char *temp = path;
  char buffer[2048];
  int cwd_node_num = ROOT_DIRECTORY_NODE;
  memory_node *cwd_node = (memory_node*)malloc(sizeof(memory_node));
  while (1)
  {
    temp = parse_path(temp, buffer);
    load_inode(cwd_node, cwd_node_num);
    read_inode(cwd_node, general_buffer);
    if (temp[0] == '\0')
    {
      char *new_directory;
      new_directory = removefile(general_buffer, buffer);
      write_inode(cwd_node, new_directory, blockmap);
      // FIXME!
      // FREE NODES!
      // free(new_directory);
      // free(cwd_node);
      return 1;
    }
    else
    {
      int find = search_file(general_buffer, buffer);
      if (find == -1)
      {
        return -1;
      }
      else
      {
        cwd_node_num = find;
      }
    }
  }
  printf("WTF?\n");
  return -1;
}

/* check to see if the device already has a file system on it,
 * and if not, create one. */
void my_mkfs()
{
  dev_open();
  char buffer[2048];
  read_block(SUPERBLOCK_START, buffer);
  if (strcmp(buffer, signature) == 0)
  {
    // deserialize bitmaps should be done here,
    // but it is not necessary for this project.
    printf("It's a trap!\n");
    return;
  }
  nodemap = create_bitmap(6000, 0);
  blockmap = create_bitmap(200 * 1024, DATA_BLOCK_START);
  root_directory = (memory_node *)malloc(sizeof(memory_node));
  initial_inode(root_directory, blockmap);
  dump_inode(*root_directory, 0);
  char *empty = "";
  write_inode(root_directory, empty, blockmap);
  memset(general_buffer, 0, sizeof(general_buffer));
  memset(_filetable, 1, sizeof(_filetable));
  memset(_offsettable, 0, sizeof(_offsettable));
}
