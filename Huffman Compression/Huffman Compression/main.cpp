#include <iostream>
#include <string>
#include "Encoding_function.h"
#include "Data.h"
using namespace std;

int main()
{
	priority_queue<code, vector<code>, Mycomp_code> huffcode;

	string name("test.txt");
	//cin >> name;

	huffman_encode(name, huffcode);
	

	return 0;
}