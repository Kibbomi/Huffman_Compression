#ifndef KIBBOMI_Data
#define KIBBOMI_Data
#define MAX_WORD 2478	//최대 글자개수
#include <queue>	//for priority queue
#include <list>
#include <string>
using namespace std;

class tree
{
public :
	tree(){}
	tree(unsigned char name1_, unsigned char name2_, int freq_) { name[0] = name1_; name[1] = name2_;  freq = freq_;  left_child = list<tree>(); right_child = list<tree>(); }
	//data
	
	unsigned char name[2];	//한글 2bytes
	int freq;	//frequency

	//child
	list<tree>  left_child, right_child;
};

class code
{
public:

	//data
	unsigned char name[2];
	string huffcode;	//허프만부호화코드가 담길곳
};

struct Mycomp_tree {
	bool operator()(const tree& l, const tree& r) {
		return l.freq > r.freq;	//ascending . ordered by freq
	}
};

struct Mycomp_code {
	bool operator()(const code& l, const code& r) {
		return l.huffcode > r.huffcode;	//ascending . ordered by string
	}
};

#endif