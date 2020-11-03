#ifndef KIBBOMI_DECODE
#define KIBBOMI_DECODE

#include "Data.h"

bool huffman_decode(string);
bool search_code(vector<code> &, string &, BYTE [2]);
bool MySort(const code &,const code &);

#endif
