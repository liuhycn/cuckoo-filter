#include "cuckoofilter.h"
#include "getpkt.h"


#include <assert.h>
#include <math.h>
#include <string>
#include <iostream>
#include <vector>




using namespace std;

using cuckoofilter::CuckooFilter;



__int128 toLongint(fiveTuple_t pkt)
{
  //printf("1\n");
  __int128 ans = 0;
  for (int i = 0; i<13; i++)
  {
    //printf("%d ",pkt.str[i]);
    ans = ans + (__int128)(pkt.str[i]);
    if (i < 12)
    {
      ans = ans << 8;
    }
    
  }
  //printf("\n");
  return ans;
}


fiveTuple_t pktBuff[500001];


int main() {
  
  srand (time(NULL));
  size_t total_items = 20000;
  extracter a;
  char file[] = "test2.pcap";
  a.extract(file, pktBuff, 2*total_items);
  CuckooFilter<__int128, 15> filter(total_items);
  size_t num_inserted = 0;

  for (size_t i = 1;i<=total_items;i++,num_inserted++)
  {
    __int128 item = toLongint(pktBuff[i]);
    if (filter.Add(item) != cuckoofilter::Ok) 
    {
      break;
    }
  }
  printf("totol = %u\n", num_inserted);

  size_t total_queries = 0;
  size_t false_queries = 0;
  for (size_t i = total_items+1; i <= 2 * total_items; i++) 
  {
  	__int128 item = toLongint(pktBuff[i]);
    if (filter.Contain(item) == cuckoofilter::Ok)
    {
      false_queries++;
    }
    total_queries++;
  }
  //printf("%u\n", total_queries);
  std::cout << "false positive rate is "
            << 100.0 * false_queries / total_queries << "%\n";

  
  return 0;
}
