#include "Data.h"
#include "Encoding_function.h"
#include <cstdio>
using namespace std;

const bool ERROR = false;

string search_code(vector<code> &v, BYTE buffer[2])
{
	//korean
	if (buffer[0] > 127)
	{
		for (code item : v) {
			if (item.name[0] == buffer[0] && item.name[1] == buffer[1])
				return item.huffcode;
		}
	}
	//alphabet
	else
	{
		for (code item : v) {
			if (item.name[0] == buffer[0])
				return item.huffcode;
		}
	}
	return string("NULL");
}

bool cal_frequency(string filename, int  freq[][94], int *freqascii)
{
	//ANSI로 저장되어 있어야 함.
	FILE * file = fopen(filename.c_str(), "rt");

	if (file == NULL) {
		printf("File open error!\n");
		return ERROR;//실패
	}

	unsigned char index[2] = { 0 };

	while (fscanf(file, "%c", &index[0]) != EOF)
	{
		if (index[0] > 127)	//한글
		{
			fscanf(file, "%c", &index[1]);
			++freq[index[0] - 0xB0][index[1] - 0xA1];
		}
		else//아스키
		{
			++freqascii[index[0]];
		}

	}

	fclose(file);
	return true;	//성공
}

void make_node(int freq[][94], int *freqascii, priority_queue<tree, vector<tree>, Mycomp_tree> &pq)
{
	//한글처리
	for (int i = 0; i < 25; i++)
		for (int j = 0; j < 94; j++)
			if (freq[i][j] != 0)	//존재하는 글자만
			{
				tree item(i + 0xB0, j + 0xA1, freq[i][j]);
				pq.push(item);
			}
	
	//아스키코드 처리
	for (unsigned char i = 0; i < 128; i++)
		if (freqascii[i] != 0)	//존재하는 글자만 추출	
		{
			tree item(i, 0, freqascii[i]);
			pq.push(item);
		}

	return;
}

void make_tree(priority_queue<tree, vector<tree>, Mycomp_tree>  &pq)
{
	while (pq.size() >= 2)
	{
		tree left = pq.top();
		pq.pop();
		tree right = pq.top();
		pq.pop();

		tree parent;
		parent.left_child.push_back(left);
		parent.right_child.push_back(right);
		parent.freq = left.freq + right.freq;

		pq.push(parent);
	}
	return;
}

bool make_code(const priority_queue<tree, vector<tree>, Mycomp_tree>& pq, priority_queue<code, vector<code>, Mycomp_code> &huffcode)
{

	inorder(pq.top(), huffcode,"");

	return true;
}

void inorder(const tree & root, priority_queue<code, vector<code>, Mycomp_code> &huffcode, string cur_code)
{

	if(!root.left_child.empty())
		inorder(root.left_child.front(), huffcode, cur_code + "0");
	
	//is leaf,
	if (root.left_child.empty() && root.right_child.empty()) {
		code item;
		
		if (root.name[0] > 127)	//korean
		{
			item.name[0] = root.name[0];
			item.name[1] = root.name[1];
			item.huffcode = cur_code;
			huffcode.push(item);
		}
		else
		{
			item.name[0] = root.name[0];
			item.name[1] = 0;
			item.huffcode = cur_code;
			huffcode.push(item);
		}
	}

	if (!root.right_child.empty())
	inorder(root.right_child.front(), huffcode, cur_code + "1");

	return;
}

bool huffman_encode(string file, priority_queue<code,vector<code>,Mycomp_code> &huffcode)
{
	int freq[25][94] = { 0 };	//frequency of 한글
	int freq_ascii[128] = { 0 };	//alphabet
	priority_queue<tree, vector<tree>, Mycomp_tree>  pq;

	
	//------부호화시작----------
	if(cal_frequency(file, freq, freq_ascii) == ERROR)
		return false;

	make_node(freq, freq_ascii, pq);

	make_tree(pq);
	//pq의 top이 huffman tree
	make_code(pq, huffcode);

	priority_queue<code, vector<code>, Mycomp_code> test = huffcode;
	while (!test.empty())
	{
		code item = test.top();
		test.pop();
		if (item.name[0] > 127)
			printf("Word : %c%c, Code : %s\n", item.name[0], item.name[1], item.huffcode.c_str());
		else
			printf("Word : %c, Code : %s\n", item.name[0], item.huffcode.c_str());
	}
	
	convert_binary(file, huffcode);

	return true;
}

//header : 오른쪽부터 마지막 bit 유효 bit 수, code개수, {code값, code유효bit, codedata (x byte)},
//body : 인코딩된 데이터

void convert_binary(string filename, priority_queue<code, vector<code>, Mycomp_code> & huffcode) 
{
	FILE *readfile = fopen(filename.c_str(), "rt");
	
	filename.erase(filename.length() - 4, 4);
	string savefilename = filename + ".bin";

	FILE *writefile = fopen(savefilename.c_str(), "wb");

	int dummy = 0;
	fprintf(writefile, "%c", dummy);
	fprintf(writefile,"%c", (char)huffcode.size());
	
	/*fclose(readfile);
	fclose(writefile);

	return;*/

	//본문 encoding용 복사.
	int idx = 0;
	vector<code> v(huffcode.size());

	while (!huffcode.empty())
	{
		code item = huffcode.top();
		huffcode.pop();
		v[idx++] = item;

		//korean
		if (item.name[0] > 127)
		{
			fprintf(writefile, "%c%c", item.name[0], item.name[1]);
		}
		//alphabet
		else
		{
			fprintf(writefile, "%c", item.name[0]);
		}

		
		fprintf(writefile, "%c",(char)item.huffcode.length());
		
		BYTE buffer = 0;
		int msb = -1;

		for (int i = 0; i < item.huffcode.length(); ++i)
		{
			if (msb == 7) {
				fprintf(writefile, "%c", buffer);
				buffer = 0;
				msb = -1;
			}

			buffer = buffer << 1;
			buffer = buffer | item.huffcode[i] - '0';
			++msb;
		}

		if (msb != -1) {
			while (msb != 7) {
				buffer = buffer << 1;
				msb++;
			}
			fprintf(writefile, "%c", buffer);
		}
	}
	//--Header encoding is finished--

	BYTE word[2];
	BYTE buffer = 0;
	int msb = -1;	//most significant bit

	while (fscanf(readfile, "%c", &word[0]) != EOF) {

		if (word[0] > 127)
			fscanf(readfile, "%c", &word[1]);

		//buffer의 글자에 해당하는 code를 획득
		string write_code = search_code(v, word);
		

		for (int i = 0; i < write_code.length(); ++i)
		{
			if (msb == 7) {
				fprintf(writefile, "%c", buffer);
				buffer = 0;
				msb = -1;
			}

			buffer = buffer << 1;
			buffer = buffer | write_code[i] - '0';
			++msb;

		}
	}
	//last byte
	int lastbit = msb;
	while (lastbit != 7) {
		buffer = buffer << 1;
		lastbit++;
	}

	fprintf(writefile, "%c", buffer);
	fseek(writefile, 0, SEEK_SET);
	fprintf(writefile, "%c", (char)msb);

	fclose(readfile);
	fclose(writefile);

	return;
}