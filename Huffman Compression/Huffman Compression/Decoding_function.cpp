#include "Decoding_function.h"

unsigned char* search_code(vector<code> &v, string s)
{
	for (code item : v)
	{
		if (s == item.huffcode)
			return item.name;
	}
	return NULL;
}
void huffman_decode(string name)
{
	vector<code> v;	//만들어 내야함.

	FILE *file = fopen("test.bin", "rb");
	FILE *decoded = fopen("decoded.txt", "wt");
	char msb;
	char codenum;

	fscanf(file, "%c", &msb);
	fscanf(file, "%c", &codenum);

	for (int i = 0; i < codenum; ++i)
	{
		code item;
		char validbit;

		fscanf(file,"%c", &item.name[0]);

		if (item.name[0] > 127)
			fscanf(file, "%c", &item.name[1]);
		else
			item.name[1] = 0;

		fscanf(file, "%c", &validbit);
		
		BYTE buffer = 0;
		while (validbit > 0)
		{
			fscanf(file, "%c", &buffer);

			for (int j = 7; j >= 0; --j){
				if (validbit <= 0)
					break;
				char bitdata = (buffer >> j) & 1;

				item.huffcode.push_back(bitdata + '0');
				--validbit;
			}
		}
		v.push_back(item);
	}

	for (code item : v)
	{
		printf("(%c%c, %s)\n", item.name[0],item.name[1], item.huffcode.c_str());
	}

	BYTE buffer, EOFcheck;
	int cnt = 0;
	string huffcode;
	while (fscanf(file, "%c", &buffer) != EOF)
	{
		if (fscanf(file, "%c", &EOFcheck) == EOF)
		{
			for (int i = 7; i >= 7 - msb; --i)	//3번자리 까지..유효 즉 4개
			{
				char bitdata = (buffer >> i) & 1;
				huffcode.push_back(bitdata + '0');

				BYTE write_word[2] = { 0 };
				bool found = false;
				for (code item : v)
				{
					if (huffcode == item.huffcode)
					{
						write_word[0] = item.name[0];
						write_word[1] = item.name[1];
						found = true;
					}
				}

				if (found)
				{
					fprintf(decoded, "%c", write_word[0]);
					printf("%c", write_word[0]);
					if (write_word[0] > 127) {
						fprintf(decoded, "%c", write_word[1]);
						printf("%c", write_word[1]);
					}
					huffcode.clear();
					break;
				}
			}
		}
		else
		{
			fseek(file, -1, SEEK_CUR);
			for (int i = 7; i >= 0; --i)
			{
				char bitdata = (buffer >> i) & 1;
				huffcode.push_back(bitdata + '0');

				BYTE write_word[2] = { 0 };
				bool found = false;
				for (code item : v)
				{
					if (huffcode == item.huffcode)
					{
						write_word[0] = item.name[0];
						write_word[1] = item.name[1];
						found = true;
					}
				}

				if (found)
				{
					fprintf(decoded, "%c", write_word[0]);
					printf("%c", write_word[0]);
					if (write_word[0] > 127) {
						fprintf(decoded, "%c", write_word[1]);
						printf("%c", write_word[1]);
					}
					huffcode.clear();
				}
			}
		}
	}

	fclose(file);
	fclose(decoded);

	return;
}