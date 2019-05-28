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
	size_t toTag()
	{
		size_t ans = 0;
		for (int i = length - 1 ; i >= 0; i--)
		{
			ans = ans + buf[i];
			if (i != 0)
			{
				ans = ans << 8;
			}
		}
		return ans;
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
	bool empty(size_t index);
	size_t randKick(size_t index, size_t tag);
};

bool table::Inserttobucket(size_t index, size_t tag)
{
	for (int i = 0; i < kTagsPerBucket; i++)
	{
		if (Table[index][i].empty())
		{
			Table[index][i].setTag(tag);
			printf("%u %u is empty.\n", index, i);
			printf("\n");
			return true;
		}
	}
	return false;
}

size_t table::randKick(size_t index, size_t tag)
{
	size_t randj = rand() % kTagsPerBucket;
	size_t kickedTag = Table[index][randj].toTag();
	Table[index][randj].setTag(tag);
	return kickedTag;
}

class cuckoofilter
{
private:
	size_t kMaxCuckooCount = 500;
	size_t rows;
	table T;
	size_t counter;
	hash Hashfamily;
public:
	cuckoofilter(){}
	cuckoofilter(size_t bits_per_tag, size_t total_item)
	{
		T = table(bits_per_tag, total_item);
		rows = T.getRows();
		Hashfamily = hash(bits_per_tag, rows);
		counter = 0;
	}
	void Info()						//get rows of table
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

	size_t index2 = Hashfamily.AlterIndexHash(index1, tag);
	printf("index1 is %u, index2 is %u\n", index1, index2);
	
	if (T.Inserttobucket(index1, tag))
	{
		counter++;
		return true;
	}
	if (T.Inserttobucket(index2, tag))
	{
		counter++;
		return true;
	}
	size_t randi;
	//must relocate buckets
	int rand = random() % 2;
	switch (rand)
	{
		case 0 : randi = index1; break;
		case 1 : randi = index2; break;
	}

	for (size_t count = 0; count < kMaxCuckooCount; count++)
	{
		//printf("kicked!!! no.%u time\n", count);
		tag = T.randKick(randi, tag);
		randi = Hashfamily.AlterIndexHash(randi, tag);
		if (T.Inserttobucket(randi, tag))
		{
			counter++;
			return true;
		}
	}
	return false;
}


struct fiveTuple_t pktTuplebuf[38000001];

int main()
{
	srand(time(NULL));
	int cnt = 0;
	int x,y;
	printf("fingger + total\n");
	//scanf("%d%d",&x,&y);
	x = 12;
	y = 10000;
	cuckoofilter cf(x,y);
	cf.Info();
	char * fname = "test2.pcap";
	extracter a;
	a.extract(fname,pktTuplebuf,10000);

	for (int i = 1; i <= 10000; i++)
	{
		if (cf.Insert(pktTuplebuf[i]))
		{
			cnt++;
		}
		else
		{
			break;
		}
	}

	printf("%d\n", cnt);
	return 0;
}