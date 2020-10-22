#include "Data.h"
#include "Encoding_function.h"
#include <cstdio>
#include <string>
using namespace std;

const bool ERROR = false;

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

	/*while (!huffcode.empty())
	{
		code item = huffcode.top();
		huffcode.pop();
		if (item.name[0] > 127)
			printf("Word : %c%c, Code : %s\n", item.name[0], item.name[1], item.huffcode.c_str());
		else
			printf("Word : %c, Code : %s\n", item.name[0], item.huffcode.c_str());
	}*/
	
	convert_binary(file, huffcode);

	return true;
}

//header : code개수(unsigned short 2byte), {code값, code유효bit(unsigned short 2byte), codedata},
//body : 인코딩된 데이터, 마지막 bit 유효 bit수.

void convert_binary(string filename, priority_queue<code, vector<code>, Mycomp_code> & huffcode) 
{
	FILE *readfile = fopen(filename.c_str(), "rt");
	
	filename.erase(filename.length() - 4, 4);
	string savefilename = filename + ".bin";

	FILE *writefile = fopen(savefilename.c_str(), "wb");

	fprintf(writefile,"%hd", (unsigned short)huffcode.size());

	while (!huffcode.empty())
	{
		code item = huffcode.top();
		huffcode.pop();

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

		
		fprintf(writefile, "%hd", (unsigned short)item.huffcode.length());
		
		int len = (int)item.huffcode.length();	
		int remainder = item.huffcode.length() % 8;
		//code의 개수는 len*8 + remainder.
		//이걸 byte에
		
		for (int i = 0; i < len; ++i) {
			
			BYTE buffer;

			for (int j = 0; j < 8; ++j){
				buffer = buffer | item.huffcode[i * 8 + j];
				buffer = buffer << 1;
			}

			fprintf(writefile, "%c", buffer);
		}

		BYTE buffer;
		for (int i = 0; i < remainder; ++i){
			buffer = buffer | item.huffcode[len * 8 + i];
			buffer = buffer << 1;
		}

		for (int i = 0; i + remainder < 8; ++i)
			buffer = buffer << 1;
		
		fprintf(writefile, "%c", buffer);	
	}
	//--encoding is finished--

	fclose(readfile);
	fclose(writefile);

	return;
}