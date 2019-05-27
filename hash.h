#include "getpkt.h"


class hash
{
private:
	u_int64 seed;
	u_int64 rows;
	u_int64 hasher;
	u_int64 scale;
	u_int64 hardener;
	u_int64 bits_per_item;
public:
	hash(){}
	hash(u_int64 bits_per_item, u_int64 rows)
	{
		this->rows = rows;
		this->bits_per_item = bits_per_item;
		seed = 0;
		hasher = GenHashSeed(3752863345);
		scale = GenHashSeed(3752863346);
		hardener = GenHashSeed(3752863347);
	}
	~hash(){}
	u_int64 AwareHash(u_char* data, u_int64 n, u_int64 hasher, u_int64 scale, u_int64 hardener);
	void mangle(const u_char* key, u_char* ret_key, int nbytes);
	u_int64 GenHashSeed(int index);
	int is_prime(int num);
	int calc_next_prime(int num);
	u_int64 genFingerprint(fiveTuple_t pkt);
	u_int64 IndexHash(fiveTuple_t pkt);
};


u_int64 hash::AwareHash(u_char* data, u_int64 n, u_int64 hasher, u_int64 scale, u_int64 hardener) 
{

	while (n) 
	{
		hasher *= scale;
		hasher += *data++;
		n--;
	}
	return (hasher ^ hardener);
}


void hash::mangle(const u_char* key, u_char* ret_key, int nbytes) 
{
	for (int i=0; i<nbytes; ++i) 
	{
		ret_key[i] = key[nbytes-i-1];
	}

    if (nbytes == 13) 
    {
		ret_key[0] = key[5];
		ret_key[1] = key[11];
		ret_key[2] = key[7];
		ret_key[3] = key[6];
		ret_key[4] = key[1];
		ret_key[5] = key[9];
		ret_key[6] = key[10];
		ret_key[7] = key[4];
		ret_key[8] = key[2];
		ret_key[9] = key[8];
		ret_key[10] = key[12];
		ret_key[11] = key[0];
		ret_key[12] = key[3];
    }
}

u_int64 hash::GenHashSeed(int index) 
{
   
    if (seed == 0) 
    {
        seed = rand();
    }
    u_int64 x, y = seed + index;
    mangle((const u_char*)&y, (u_char*)&x, 8);
    return AwareHash((u_int8*)&y, 8, 388650253, 388650319, 1176845762);
}

int hash::is_prime(int num) 
{
    int i;
    for (i=2; i<num; i++)
    {
        if ((num % i) == 0)
        {
            break;
        }
    }
    if (i == num) 
    {
        return 1;
    }
    return 0;
}

int hash::calc_next_prime(int num) 
{
    while (!is_prime(num)) 
    {
        num++;
    }
    return num;
}

u_int64 hash::genFingerprint(fiveTuple_t pkt)
{
	u_int64 temp = AwareHash(pkt.str, 13, hasher, scale, hardener);
	u_int64 tag;
	tag = temp & ((1ULL << bits_per_item) - 1);
	printf("fingerprint is %u\n", tag);
	return tag;
}


u_int64 hash::IndexHash(fiveTuple_t pkt)
{
	u_int64 temp = AwareHash(pkt.str, 13, hasher, scale, hardener);
	printf("index is %u\n", (temp >> 32) % rows);
	return (temp >> 32) % rows;
}