#include <cstdio>
#include "Encoding_function.h"
#include "Data.h"
using namespace std;

int main()
{
	priority_queue<code, vector<code>, Mycomp_code> huffcode;
	const char * name = "test.txt";
	huffman_encode(name, huffcode);
	

	return 0;
}