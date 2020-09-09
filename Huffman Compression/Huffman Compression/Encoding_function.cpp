#include "Data.h"
#include "Encoding_function.h"
#include <cstdio>
#include <string>
using namespace std;

const bool ERROR = false;

//filename -> string?
bool cal_frequency(const char * filename, int  freq[][94], int *freqascii)
{
	FILE * file = fopen(filename, "rt");

	if (file == NULL) 
		return ERROR;//실패

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

bool huffman_encode(const char * file, priority_queue<code,vector<code>,Mycomp_code> &huffcode)
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
	make_code(pq,huffcode);

	while (!huffcode.empty())
	{
		code item = huffcode.top();
		huffcode.pop();
		if (item.name[0] > 127)
			printf("Word : %c%c, Code : %s\n", item.name[0], item.name[1], item.huffcode.c_str());
		else
			printf("Word : %c, Code : %s\n", item.name[0], item.huffcode.c_str());
	}
	//convert_binary(file, code);

	return true;
}