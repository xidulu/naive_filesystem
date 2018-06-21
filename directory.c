#include "directory.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define d 256

// @author:Xi Wang
// Directory in this filesystem is basically a string,
// with special format.
// To add/create/remove/modify entries in a directory,
// we only need to load the string, modify it, and dump it back.
// Format:
//     ...,len(filename),filename,inode numer,...
//     (comma included)
//     (integer stored as raw string)


// Helper functoin: string matching
// based on Rabin-Karp algorithm.
// copy from geekforgeeks
static int pattern_search(char *pat, char *txt) {
    int M = strlen(pat);
    int N = strlen(txt);
    int i, j;
    int q = 79; // magic prime number
    int p = 0; // hash value for pattern
    int t = 0; // hash value for txt
    int h = 1;
    // The value of h would be "pow(d, M-1)%q"
    for (i = 0; i < M - 1; i++)
        h = (h * d) % q;
    // Calculate the hash value of pattern and first
    // window of text
    for (i = 0; i < M; i++) {
        p = (d * p + pat[i]) % q;
        t = (d * t + txt[i]) % q;
    }
    // Slide the pattern over text one by one
    for (i = 0; i <= N - M; i++) {
        // Check the hash values of current window of text
        // and pattern. If the hash values match then only
        // check for characters on by one
        if (p == t) {
            /* Check for characters one by one */
            for (j = 0; j < M; j++) {
                if (txt[i + j] != pat[j])
                    break;
            }

            // if p == t and pat[0...M-1] = txt[i, i+1, ...i+M-1]
            if (j == M)
                return i;
        }
        // Calculate hash value for next window of text: Remove
        // leading digit, add trailing digit
        if (i < N - M) {
            t = (d * (t - txt[i] * h) + txt[i + M]) % q;
            // We might get negative value of t, converting it
            // to positive
            if (t < 0)
                t = (t + q);
        }
    }
    return -1;
}

// Input a directory, filename and its inode number,
// return a new directory string describing the new
// directory. 
// (This function is non-destructive)
char *addfile(char *directory, char *filename, int inode_num) {
    char buffer[500];
    size_t origin_len = strlen(directory);
    size_t concat_len = sprintf(buffer,
                                "%s,%u,",
                                filename,
                                inode_num);
    size_t new_len = origin_len + concat_len + 1;
    char *new_directory = (char *) malloc(new_len);
    memset(new_directory, 0, new_len);
    sprintf(new_directory, "%s%s", directory, buffer);
    //free(directory);
    return new_directory;
}

// Input a directory, filename,
// remove the corresponding entry in the string.
// (This function is non-destructive)
char *removefile(char *directory, char *filename) {
    char *new_directory = (char *) malloc(strlen(directory));
    memset(new_directory, 0, sizeof(strlen(directory)));
    int pos = pattern_search(filename, directory);
    int end = pos + strlen(filename) + 1;
    while (directory[end] != ',') {
        end++;
    }
    strncpy(new_directory, directory, pos);
    strcat(new_directory, directory + end + 1);
    return new_directory;
}


// Search for FILENAME in the current directory,
// return its inode number.(non-recursive)
int search_file(char *directory, char *filename) {
    char pattern[500] = "";
    char *comma = ",";
    int inode_num;
    strcpy(pattern, filename);
    strcat(pattern, comma);
    int pos = pattern_search(pattern, directory);
    if (pos == -1) {
        return -1;
    }
    strcat(pattern, "%d");
    sscanf(directory + pos, pattern, &inode_num);
    return inode_num;
}

int is_empty(char *directory) {
    int len = strlen(directory);
    if (len == 0) {
        return 1;
    } else {
        return 0;
    }
}