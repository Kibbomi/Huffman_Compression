#include <iostream>
#include "Encoding_function.h"
#include "Decoding_function.h"
#include "Data.h"
#include <ctime>
using namespace std;

int main()
{
	int input;
	bool flag = true;

	while (flag) {
		cout << ">> 1. Compression  2. Decompression 3. Exit \n>>";
		cin >> input;
		clock_t start;
		string name;
		if (input == 1)
		{
			priority_queue<code, vector<code>, Mycomp_code> huffcode;
			cout << ">> Enter the file that you want to compress\n>>";
			cin >> name;

			start = clock();
			if (huffman_encode(name, huffcode) == false)	//error
			{
				printf("File open error!\n");
				return 0;
			}


			cout << "Compression Time : " << ((float)clock() - start) / CLOCKS_PER_SEC << "(s)\n\n";
		}
		else if (input == 2)
		{
			cout << ">> Enter the file that you want to decompress\n>>";
			cin >> name;
			start = clock();
			if (huffman_decode(name) == false)
			{
				printf("File open error!\n");
				return false;
			}
			cout << "Decode time : " << ((float)clock() - start) / CLOCKS_PER_SEC << "(s)\n\n";
		}
		else if(input == 3)
		{
			cout << ">> Exit...\n";
			flag = false;
		}
		else
		{
			cout << ">> Please enter 0 to 3 \n";
		}
	}
	return 0;
}