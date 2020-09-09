#ifndef KIBBOMI_ENCODE
#define KIBBOMI_ENCODE

#include <queue>
#include "Data.h"
using namespace std;

bool cal_frequency(char * , int[][94], int*);
void make_node(int[][94], int*, priority_queue<tree, vector<tree>, Mycomp_tree> &);

void make_tree(priority_queue<tree, vector<tree>, Mycomp_tree>  &);

void inorder(const tree & , priority_queue<code, vector<code>, Mycomp_code> &, string );
bool make_code(const priority_queue<tree, vector<tree>, Mycomp_tree>&, priority_queue<code, vector<code>, Mycomp_code> &);
bool huffman_encode(const char *, priority_queue<code, vector<code>, Mycomp_code> &);

/*
unsigned char * search_code(struct code*, unsigned char*);
void convert_binary(char *, struct code *);
*/





#endif
