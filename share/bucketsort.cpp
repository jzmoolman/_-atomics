#include <vector>
#include <thread>
#include <barrier>

#include "../include/bucketsort.h"

// std::vector<std::thread*> threads;
// std::barrier<> *bar; 
// std::mutex *lck;

std::vector<bucket_t*> buckets; 

void print_buckets(std::vector<bucket_t*> *lst) {
    for (int i = 0; i < lst->size(); i++) {
        bucket_t *bucket = lst->at(i);
        // printf("Bucket %d [%d,%d]\n", i, bucket->range_start, bucket->range_end);
        // for (int j = 0; j < bucket->lst.size(); j++){
        //     printf("%d,",bucket->lst.at(j));
        // }
        // printf("\n");
    }
}

void sortlist_into_buckets(std::vector<int> *lst, int nbuckets) {
    // int bucket_size = INT_MAX % nbuckets;
    int bucket_size = 10 / nbuckets;
    printf("%d\n", bucket_size);
    bucket_t *bucket;
    for ( int i = 0; i < nbuckets; i++ ) {
        bucket = new bucket_t;
        bucket->range_start = i*bucket_size;
        bucket->range_end = i*bucket_size + bucket_size-1;
        buckets.push_back(bucket);
    }

    // printf("%d\n",buckets.size());
    for ( int i = 0; i < lst->size(); i++) {
        int value;
        int idx;
        value = lst->at(i);
        idx = value / bucket_size;
        bucket = buckets.at(idx);
        bucket->lst.push_back(value);
    }
}

int getbucketIndex(int val) {
    for ( int i = 0; i < buckets.size(); i++) {
        if ( val >= buckets.at(i)->range_start && val <= buckets.at(i)->range_end  ) {
            return i;
        }
    }
    return -1;
}


std::vector<bucket_t*>* remove_empty_buckets() {
    for ( int i = buckets.size()-1; i >= 0; i--) {
       if (buckets.at(i)->lst.size() == 0)  {
        buckets.erase(buckets.begin()+i,buckets.begin()+i+1);
       }
    }
    return &buckets;
}

std::vector<bucket_t*>* balance_buckets(std::vector<bucket_t *> *lst, int nbuckets) {


    printf("%d %d\n", lst->size(), nbuckets);
    if (lst->size() > 0 && lst->size() < nbuckets ) {
        bucket_t *_bucket;
        std::vector<int> *_lst;

        _bucket = lst->at(0);
        _lst = &_bucket->lst;
        lst->erase(lst->begin(), lst->begin()+1);
        sortlist_into_buckets2(_lst, nbuckets,  _bucket->range_start, _bucket->range_end);
        print_buckets(lst);
        balance_buckets(lst, nbuckets);
    }
    return lst;
}

std::vector<bucket_t*>* sortlist_into_buckets2(std::vector<int> *lst, int nbuckets, int min, int max) {
    int bucket_size;
    bucket_t *bucket;
    long _max = max;
    long _min = min;
    
    bucket_size = ((_max-_min+1) + nbuckets-1) / nbuckets; // this doesnt work with large numbers
    // bucket_size = (max-min) / bucket_size;
    // if ((max - min) % bucket_size > 0) {
    //     bucket_size +=1;
    // }

    printf("%d\n", bucket_size);
    for ( int i = 0; i < nbuckets; i++ ) {
        bucket = new bucket_t;
        bucket->range_start = min +(i*bucket_size);
        bucket->range_end = min +(i*bucket_size + bucket_size-1);
        if (bucket->range_end < -1)
          bucket->range_end = max;
        buckets.push_back(bucket);
    }

    for ( int i = 0; i < lst->size(); i++) {
        int value;
        int idx;
        value = lst->at(i);
        idx = getbucketIndex(value);
        if (idx == -1 ) {
            //Value not in range
            continue;
        }
        bucket = buckets.at(idx);
        bucket->lst.push_back(value);
    }

    remove_empty_buckets();

    return &buckets;
}

//Only one thread can do this must be protected by mutex


// void* thread_func_bucketsort(int  tid) { 
	
	
//     //Sync Threads
// 	bar->arrive_and_wait();
// 	if (args->tid == 0) {
// 		clock_gettime(CLOCK_MONOTONIC, &_startTime);
//     }


//     // slipt up into buckets
//     _lk->lock();

//     merged = 0;
//     if (buckets.size() == 0) {
//         //printf("first thread sort input into buckets %lu\n", args->tid);
//         _bucket_sort(args->list,args->cnt);
//         //printf(" Balance buckets %lu\n", args->tid);
//         _balance_buckets(args->cnt);

//     }
//     _lk->unlock();

// 	_bar->arrive_and_wait();

//     // sort section
//     int sorted_busy = 0;
//     while (sorted_busy < buckets.size() ) {
        
//         _lk->lock();
//         for ( int i = 0; i < buckets.size(); i++) {
//             bucket_info_t *b;
//             b = &buckets.at(i);
//             if (b->lk->try_lock()) {
//                 if (b->sorted != 1) {
//                     b->sorted = 1;
//                     quick_sort(&b->list, 0, b->list.size()-1);
//                     sorted_busy++;
//                 } else  {
//                    sorted_busy++;
//                 }
//                 b->lk->unlock();
//             } else {
//                 // some other thread is busy
//                 sorted_busy++;
//             };
//         }
//         _lk->unlock();
//     }
      
//     // merge section
//     _lk->lock();
//     if (merged == 0) {
//         args->list->clear();
//         for (int i = 0; i < buckets.size()-1; i++) {
//             for ( int j = i+1; j < buckets.size(); j++) {
//                 if (buckets.at(i).range_start > buckets.at(j).range_start) {
//                     //swap
//                     bucket_info_t tmp;
//                     tmp = buckets.at(i);
//                     buckets.at(i) = buckets.at(j);
//                     buckets.at(j) = tmp;
//                 }
//             }
//         }
//         // printf("new order %lu\n", args->tid);
//         // print_buckets(&buckets);

//         for (int i = 0; i < buckets.size(); i++) {
//             for ( int j = 0; j < buckets.at(i).list.size(); j++) {
//                 args->list->push_back(buckets.at(i).list.at(j));
//             }
//         }
//     }
//     merged = 1;
//     _lk->unlock();
	
//     //End Sync Threads
// 	_bar->arrive_and_wait();
// 	if(args->tid==0){
// 		clock_gettime(CLOCK_MONOTONIC,&_endTime);
// 	}
// 	_local_cleanup();
	
// 	return 0;
// }
