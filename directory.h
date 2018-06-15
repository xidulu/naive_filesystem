#if !defined(DIRECTORY_H)
#define DIRECTORY_H

char *addfile(char *directory, char *filename, int inode_num);

char *removefile(char *directory, char *filename);

int search_file(char *directory, char *filename);

int is_empty(char *directory);

#endif // DIRECTORY_H
