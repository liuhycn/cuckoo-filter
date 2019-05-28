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
	bool empty()
	{
		bool ans = true;
		for (int i = 0;i<length;i++)
		{
			if (buf[i] != 0)
			{
				ans = false;
				break;
			}
		}
		return ans;
	}
};


class table
{
private:
	size_t kTagsPerBucket = 5;
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
	void Info()
	{
		printf("fingger is %u\n", bits_per_tag);
		printf("rows is %u\n", rows);
		printf("every finggerprint uses %u blocks of char\n", Table[0][0].length);
	}
	size_t getRows()
	{
		return rows;
	}
	bool Inserttobucket(size_t index, size_t tag);
};

bool table::Inserttobucket(size_t index, size_t tag)
{
	for (int i = 0; i < kTagsPerBucket; i++)
	{
		if (Table[index][i].empty())
		{
			Table[index][i].setTag(tag);
			return true;
		}
	}
	return false;
}

class cuckoofilter
{
private:
	size_t kMaxCuckooCount = 500;
	size_t rows;
	table T;
	hash Hashfamily;
public:
	cuckoofilter(){}
	cuckoofilter(size_t bits_per_tag, size_t total_item)
	{
		T = table(bits_per_tag, total_item);
		rows = T.getRows();
		Hashfamily = hash(bits_per_tag, rows);
	}
	size_t Info()						//get rows of table
	{
		T.Info();
	}
	bool Insert(fiveTuple_t pkt);
	
};


bool cuckoofilter::Insert(fiveTuple_t pkt)
{
	size_t tag;
	size_t index1;
	

	Hashfamily.genIndexTagHash(pkt, &tag, &index1);

	size_t curindex = index1;
  	size_t curtag = tag;

	size_t index2 = Hashfamily.AlterIndexHash(index1, tag);
	
	for (int i = 0;i<5;i++)
	{
		curindex = Hashfamily.AlterIndexHash(curindex, curtag);
		printf("%u\n", curindex);
	}

	printf("index1 = %u, index2 = %u.\n", index1, index2);


}


struct fiveTuple_t pktTuplebuf[38000001];

int main()
{
	srand(time(NULL));
	int x,y;
	printf("fingger + total\n");
	scanf("%d%d",&x,&y);
	cuckoofilter cf(x,y);

	char * fname = "test2.pcap";
	extracter a;
	a.extract(fname,pktTuplebuf,10);

	for (int i = 1; i <= 10; i++)
	{
		cf.Insert(pktTuplebuf[i]);
	}

	return 0;
}