#ifndef _BUCKETSORT_H_
#define _BUCKETSORT_H_ 

#include <vector>
#include <mutex>

struct bucket_t{
    std::vector<int> lst;
    int range_start;
    int range_end;
    // std::mutex *lck;
    // bool locked;
};

void print_buckets(std::vector<bucket_t*> *lst); 

std::vector<bucket_t*>* balance_buckets(std::vector<bucket_t*> *lst, int nbuckets);
std::vector<bucket_t*>* sortlist_into_buckets2(std::vector<int> *lst, int nbuckets, int min, int max); 


#endif