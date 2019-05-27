#include <iostream>
#include <string.h>
#include <stdio.h>

#include "hash.h"

using namespace std;

typedef unsigned char u_char;

struct tagNode
{
	size_t bits_per_tag;
	u_char* buf;
	size_t length;
	tagNode()
	{
		bits_per_tag = 0;
		buf = NULL;
		length = 0;
	}
	void setNode(size_t bits)
	{
		size_t temp = (bits + 7) >> 3;
		length = temp;
		buf = new u_char[length];
		memset(buf, 0, sizeof(buf));
	}
	void setTag(size_t tag)
	{
		for (int i = 0;i<length;i++)
		{
			size_t mask = (1ULL << 8) - 1;
			buf[i] = (u_char)(mask & tag);
			tag = tag >> 8;
		}
	}
};


class table
{
private:
	size_t kTagsPerBucket = 4;
	size_t bits_per_tag;
	size_t total_item;
	size_t rows;
	tagNode** Table;
public:
	table(){}
	table(size_t bits_per_tag, size_t total_item)
	{
		this->bits_per_tag = bits_per_tag;
		this->total_item = total_item;
		this->rows = (total_item + (kTagsPerBucket - 1)) / kTagsPerBucket;
		Table = new tagNode*[rows];
		for (int i = 0;i<rows;i++)
		{
			Table[i] = new tagNode[kTagsPerBucket];
			for (int j = 0;j<kTagsPerBucket;j++)
			{
				Table[i][j].setNode(bits_per_tag);
			}
		}
	}
	size_t Info()
	{
		printf("fingger is %u\n", bits_per_tag);
		printf("rows is %u\n", rows);
		printf("every finggerprint uses %u blocks of char\n", Table[0][0].length);
		return rows;
	}
};


class cuckoofilter
{
private:
	size_t kMaxCuckooCount = 500;
	table T;
public:
	cuckoofilter(){}
	cuckoofilter(size_t bits_per_tag, size_t total_item)
	{
		T = table(bits_per_tag, total_item);
	}
	size_t Info()
	{
		size_t temp = T.Info();
		return temp;
	}
	
};

struct fiveTuple_t pktTuplebuf[38000001];

int main()
{
	srand(time(NULL));
	int x,y;
	printf("fingger + total\n");
	scanf("%d%d",&x,&y);
	cuckoofilter cf(x,y);
	size_t rows = cf.Info();
	hash hh(x,rows);

	char * fname = "test2.pcap";
	extracter a;
	a.extract(fname,pktTuplebuf,3);

	for (int i = 1;i<=3;i++)
	{
		hh.genFingerprint(pktTuplebuf[i]);
		hh.IndexHash(pktTuplebuf[i]);
	}

	return 0;
}