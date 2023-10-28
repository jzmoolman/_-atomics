
#include <iostream>
#include <getopt.h> /* struct option  */
#include <thread>
#include <barrier>

#include "./include/bucketsort.h"

#include "./include/zbarrier.h"
#include "./include/pthreadbarrier.h" // wrapper for c++ barrier class
#include "./include/sensebarrier.h"

#include "./include/zutils.h"
#include "./include/zlock.h"
#include "./include/pthreadlock.h"   // Wrapper for c++ mutex class
#include "./include/taslock.h"
#include "./include/ttaslock.h"
#include "./include/ticketlock.h"
#include "./include/ticketlock.h"
#include "./include/mcslock.h"
#include "./include/petersonlock.h"
#include "./include/petersonrellock.h"

#include "./include/quicksort.h"

using namespace std;

// Lock type 
enum {
    lt_none,
    lt_pthread,
    lt_tas,
    lt_ttas,
    lt_ticket,
    lt_mcs,
    lt_peterson,
    lt_tasrel,
    lt_ttasrel,
    lt_mcsrel,
    lt_petersonrel
};

enum {
    bt_none,
    bt_pthread,
    bt_sense,
    bt_senserel
};

#define INPUT_BUFFER_SIZE 128
vector<thread*> threads;
struct timespec ts_start, ts_end;

// Program stuff
const char *program_name;  
const char *name = "Zach Moolman";
char* opt_in_file = NULL;
char* opt_out_file = NULL;
size_t nthreads = 5;     // default threads including the main thread?? 
int locktype = lt_none;
int barriertype = bt_none;
int niterations = 1000;

vector<int> inputs;
vector<int> outputs;
vector<bucket_t*> *mybuckets;
vector<bucket_t*> sorted_buckets;

// Lock & barriers to test
ZBarrier *bar;
ZLock *lck; 
ZLock *sorted_buckets_lck; 
MCSLock *mcs;
PetersonLock *peterson;
PetersonrelLock *petersonrel;

/* 
 * Argument passing plumming code.
 */

void handle_args(int argc, char** argv){
    const char *cp; /* pointer to end of  prgram last '/' */
	if ((cp = strrchr(argv[0], '/')) != NULL)
		program_name = cp + 1;
	else
		program_name = argv[0];

    int c;

    // `mysort [--name] [-i source.txt] [-o out.txt] 
    // [-t NUM_THREADS] [--bar=<sense,pthread>] 
    // [--lock=<tas,ttas,ticket,mcs,pthread,petersonseq,petersonrel>]`
    while (1) {
        static struct option long_options[] = {
            {"name", no_argument, 0, 'a'},
            {"in", required_argument, 0, 'i'},
            {"out", required_argument, 0, 'o'},
			{"thread", required_argument, 0, 't'},
            {"bar", required_argument, 0, 'b'},
            {"lock", required_argument, 0, 'l'},
            {0, 0, 0, 0}
        };

        int option_index = 0;
        c = getopt_long ( argc, argv, "ai:o:t:b:l:", long_options, &option_index);

        if ( c == -1) 
            break;

        switch (c) {
            case 'a':
                printf("option -a with value `%s'\n", name);
                exit(0);
                break;
            case 'n':
                niterations = atoi(optarg);
                break;
            case 'i':
                opt_in_file = optarg;
                break;
            case 'o':
                opt_out_file = optarg;
                break;
			case 't':
	            nthreads = atoi(optarg);
				if((nthreads > 150) || (nthreads == 0)) {
					printf("ERROR: invalid number of threads\n");
				    abort();
				}
				break;	
            case 'b':
                if (strcmp(optarg, "pthread") == 0 ) {
                    barriertype = bt_pthread;
                } else if (strcmp(optarg, "sense") == 0) {
                    barriertype = bt_sense;
                } else if (strcmp(optarg, "senserel") == 0) {
                    barriertype = bt_senserel;
                }
                break;
            case 'l':
                if (strcmp(optarg, "pthread") == 0)
                   locktype = lt_pthread;
                else if (strcmp(optarg, "tas") == 0)
                   locktype = lt_tas;
                else if (strcmp(optarg, "ttas") == 0)
                   locktype = lt_ttas;
                else if (strcmp(optarg, "ticket") == 0)
                   locktype = lt_ticket;
                else if (strcmp(optarg, "mcs") == 0)
                   locktype = lt_mcs;
                else if (strcmp(optarg, "peterson") == 0)
                    locktype = lt_peterson;
                else if (strcmp(optarg, "tasrel") == 0)
                   locktype = lt_tasrel;
                else if (strcmp(optarg, "ttasrel") == 0)
                   locktype = lt_tasrel;
                else if (strcmp(optarg, "mcsrel") == 0)
                   locktype = lt_mcsrel;
                else if (strcmp(optarg, "petersonrel") == 0)
                    locktype = lt_petersonrel;
                else  {
                    printf("ERROR: lock not recognised `%s'\n", optarg);
                    abort();
                }
                break;
            case '?':
                break;
            default:
                abort();
        }
        
    }

    if ( optind < argc) {
        printf("ERROR: non-option ARGV-elemets: ");
        while ( optind < argc) 
            printf("%s ", argv[optind++]);
        puts("\n");
    }

    if ( opt_in_file == NULL) {
        printf("Input file not specified\n");
        // abort();
        static char* in_file_buffer = "./data/321.txt";
        opt_in_file = in_file_buffer;
    }

    if ( opt_out_file == NULL) {
        printf("Output file not specified\n");
         // abort();
        static char* out_file_buffer = "./data/123.txt";
        opt_out_file = out_file_buffer;

    }


    if ( locktype == lt_none ) {
        printf("No lock specified default to pthread lock\n");
        locktype = lt_pthread;

    }
    if ( barriertype == bt_none ) {
        printf("No barrier specified default to pthread barrier\n");
        barriertype = bt_pthread;

    }
}



void thread_func_lock(int tid) {
    //Sync Threads
	bar->wait();
	if (tid == 1) {
		clock_gettime(CLOCK_MONOTONIC, &ts_start);
        printf("Thread %d getting time_start %d.%d\n", tid,ts_start.tv_sec, ts_start.tv_nsec);
        /*
         * sort into buckets
         */

        mybuckets = sortlist_into_buckets2(&inputs, nthreads, 0, 2147483647);
        mybuckets = balance_buckets(mybuckets, nthreads);
        print_buckets(mybuckets);
    }
    bar->wait();
    /*
     * Critiacal Section
     */
    {
        bucket_t *bucket;
        while (mybuckets->size() > 0 ) {
            lck->lock();
            bucket = mybuckets->at(0);
            mybuckets->erase(mybuckets->begin(), mybuckets->begin()+1);
            sorted_buckets.push_back(bucket);
            lck->unlock();

            quick_sort(&bucket->lst, 0, bucket->lst.size()-1);

        }
    }

    bar->wait();
    if (tid == 1) {
        //Merge

        for (int i = 0; i < sorted_buckets.size()-1; i++) {
            for ( int j = i+1; j < sorted_buckets.size(); j++) {
                if (sorted_buckets.at(i)->range_start > sorted_buckets.at(j)->range_start) {
                    //swap
                    bucket_t *tmp;
                    tmp = sorted_buckets.at(i);
                    sorted_buckets.at(i) = sorted_buckets.at(j);
                    sorted_buckets.at(j) = tmp;
                }
            }
        }
        // printf("new order %lu\n", args->tid);
        // print_buckets(&buckets);

        for (int i = 0; i < sorted_buckets.size(); i++) {
            for ( int j = 0; j < sorted_buckets.at(i)->lst.size(); j++) {
                outputs.push_back(sorted_buckets.at(i)->lst.at(j));
            }
        }






		clock_gettime(CLOCK_MONOTONIC, &ts_end);
        if ( ts_start.tv_sec == ts_end.tv_sec && ts_start.tv_nsec > ts_end.tv_nsec) {
            ts_end.tv_sec += 1;
        }
        printf("Thread %d getting time_end %d.%d\n", tid,ts_end.tv_sec, ts_end.tv_nsec);
    }
}

void thread_func_mcs(int tid) {
    //Sync Threads
	bar->wait();
	if (tid == 1) {
		clock_gettime(CLOCK_MONOTONIC, &ts_start);
        printf("Thread %d getting time_start %d.%d\n", tid,ts_start.tv_sec, ts_start.tv_nsec);
        /*
         * sort into buckets
         */

        mybuckets = sortlist_into_buckets2(&inputs, nthreads, 0, 2147483647);
        mybuckets = balance_buckets(mybuckets, nthreads);
        print_buckets(mybuckets);
    }
    bar->wait();
    /*
     * Critiacal Section
     */
    {
        bucket_t *bucket;
        thread_local qnode_t p;
        while (mybuckets->size() > 0 ) {
            mcs->acquire(&p,tid);
            bucket = mybuckets->at(0);
            mybuckets->erase(mybuckets->begin(), mybuckets->begin()+1);
            sorted_buckets.push_back(bucket);
            mcs->release(&p, tid);

            quick_sort(&bucket->lst, 0, bucket->lst.size()-1);

        }
    }

    bar->wait();
    if (tid == 1) {
        //Merge

        for (int i = 0; i < sorted_buckets.size()-1; i++) {
            for ( int j = i+1; j < sorted_buckets.size(); j++) {
                if (sorted_buckets.at(i)->range_start > sorted_buckets.at(j)->range_start) {
                    //swap
                    bucket_t *tmp;
                    tmp = sorted_buckets.at(i);
                    sorted_buckets.at(i) = sorted_buckets.at(j);
                    sorted_buckets.at(j) = tmp;
                }
            }
        }
        // printf("new order %lu\n", args->tid);
        // print_buckets(&buckets);

        for (int i = 0; i < sorted_buckets.size(); i++) {
            for ( int j = 0; j < sorted_buckets.at(i)->lst.size(); j++) {
                outputs.push_back(sorted_buckets.at(i)->lst.at(j));
            }
        }






		clock_gettime(CLOCK_MONOTONIC, &ts_end);
        if ( ts_start.tv_sec == ts_end.tv_sec && ts_start.tv_nsec > ts_end.tv_nsec) {
            ts_end.tv_sec += 1;
        }
        printf("Thread %d getting time_end %d.%d\n", tid,ts_end.tv_sec, ts_end.tv_nsec);
    }
}

void thread_func_peterson(int tid) {
    //Sync Threads
	bar->wait();
	if (tid == 1) {
		clock_gettime(CLOCK_MONOTONIC, &ts_start);
        printf("Thread %d getting time_start %d.%d\n", tid,ts_start.tv_sec, ts_start.tv_nsec);
        /*
         * sort into buckets
         */

        mybuckets = sortlist_into_buckets2(&inputs, nthreads, 0, 2147483647);
        mybuckets = balance_buckets(mybuckets, nthreads);
        print_buckets(mybuckets);
    }
    bar->wait();
    /*
     * Critiacal Section
     */
    {
        bucket_t *bucket;
        while (mybuckets->size() > 0 ) {

        if (locktype == lt_peterson)
            peterson->lock(tid);
        else 
            petersonrel->lock(tid);
            
            bucket = mybuckets->at(0);
            mybuckets->erase(mybuckets->begin(), mybuckets->begin()+1);
            sorted_buckets.push_back(bucket);



        if (locktype == lt_peterson)
            peterson->unlock(tid);
        else 
            petersonrel->unlock(tid);


            quick_sort(&bucket->lst, 0, bucket->lst.size()-1);

        }
    }

    bar->wait();
    if (tid == 1) {
        //Merge

        for (int i = 0; i < sorted_buckets.size()-1; i++) {
            for ( int j = i+1; j < sorted_buckets.size(); j++) {
                if (sorted_buckets.at(i)->range_start > sorted_buckets.at(j)->range_start) {
                    //swap
                    bucket_t *tmp;
                    tmp = sorted_buckets.at(i);
                    sorted_buckets.at(i) = sorted_buckets.at(j);
                    sorted_buckets.at(j) = tmp;
                }
            }
        }
        // printf("new order %lu\n", args->tid);
        // print_buckets(&buckets);

        for (int i = 0; i < sorted_buckets.size(); i++) {
            for ( int j = 0; j < sorted_buckets.at(i)->lst.size(); j++) {
                outputs.push_back(sorted_buckets.at(i)->lst.at(j));
            }
        }






		clock_gettime(CLOCK_MONOTONIC, &ts_end);
        if ( ts_start.tv_sec == ts_end.tv_sec && ts_start.tv_nsec > ts_end.tv_nsec) {
            ts_end.tv_sec += 1;
        }
        printf("Thread %d getting time_end %d.%d\n", tid,ts_end.tv_sec, ts_end.tv_nsec);
    }
}



int main(int argc, char** argv) {
    /*
     * Varibales file handling
     */

    FILE * in_file;  /* input file */
    FILE * out_file; /* output file */
    char buffer[INPUT_BUFFER_SIZE]; /* input text buffer */

    /*
     * time vars
     */
    unsigned long long elapsed_ns;
	double elapsed_s;


    /*
     * get args processed
     */
    handle_args(argc, argv);


    /*
     * Check of valid filename were passed as arguments
     */

	in_file = fopen(opt_in_file, "r");
	if (in_file == NULL) {
		fprintf(stderr, "Unable to open file: %s\n", opt_in_file);
        abort();
    }

	out_file = fopen(opt_out_file, "w");
	if (out_file == NULL) {
		fprintf(stderr, "Unable to open file: %s\n", opt_out_file);
        abort();
    }
    
    /* 
     * reading input from in_file 
     */
    while (fgets(buffer, INPUT_BUFFER_SIZE, in_file)) {
        char* end;
        if ( buffer[0] != '\n') {
            const long sl = strtol(buffer, &end, 10);
            if (end == buffer) {
                (void) fprintf(stderr, "%s: not a decimal number\n", buffer);
            } else if ('\0' != *end && '\n' != *end) {
                (void) fprintf(stderr, "%s: extra characters at end of input: %s\n", buffer, end);
            } else if ((LONG_MIN == sl || LONG_MAX == sl) && ERANGE == errno) {
                (void) fprintf(stderr, "%s out of range of type long\n", buffer);
            } else if (sl > INT_MAX) {
                (void) fprintf(stderr, "%ld greater than INT_MAX\n", sl);
            } else if (sl < INT_MIN) {
                (void) fprintf(stderr, "%ld less than INT_MIN\n", sl);
            } else {
                inputs.push_back((int)sl);
            }
        }
    }
    fclose(in_file);


    /*
     *
     */

    


    switch (locktype) {
    case lt_pthread:
        lck = new PThreadLock;
         lck = new PThreadLock;
        break;
    case lt_tas:
        lck = new TASLock(lo_strong);
        break;
    case lt_ttas:
        lck = new TTASLock(lo_strong);
        break;
    case lt_ticket:
        lck = new TicketLock;
        break;
    case lt_mcs:
        mcs = new MCSLock(lo_strong);
        break;
    case lt_peterson:
        peterson = new PetersonLock();
        nthreads = 2;
        break;
    case lt_tasrel:
        lck = new TASLock(lo_weak);
        break;
    case lt_ttasrel:
        lck = new TTASLock(lo_weak);
        break;
    case lt_mcsrel:
        mcs = new MCSLock(lo_weak);
        break;
    case lt_petersonrel:
        petersonrel = new PetersonrelLock();
        nthreads = 2;
    break;

        default:
            break;
    }


    switch (barriertype) {
    case bt_pthread:
        bar = new PThreadBarrier(nthreads);
        break;
    case bt_sense:
        bar = new SenseBarrier(lo_strong, nthreads);
        break;
    default:
        bar = new PThreadBarrier(nthreads);
        break;
    }

    /*
    * Launch threads
    */

    threads.push_back(0); // reserve for main thread
    for(size_t i = 1; i < nthreads; i++) {
        thread *t;
        switch (locktype) {
        case lt_mcs:
        case lt_mcsrel:
            t =  new thread(thread_func_mcs, i);
            break;
        case lt_peterson:
        case lt_petersonrel:
            t =  new thread(thread_func_peterson, i);
            break;
        default:
            // tas, ttas, tasrel, ttasrel
            t =  new thread(thread_func_lock,i);
            break;
        }
        threads.push_back(t);
    }


    switch (locktype) {
    case lt_mcs:
    case lt_mcsrel:
        thread_func_mcs(0);
        break;
    case lt_peterson:
    case lt_petersonrel:
        thread_func_peterson(0);
        break;
    default:
        //tas, ttas, tasrel, ttasrel
        thread_func_lock(0);     
        break;
    }


    /*
     * Join threads
     */
    
    for(size_t i=1; i < nthreads; i++) {
        threads[i]->join();
        std::cout << "Delete thread " << i << std::endl;
        delete threads[i];
    }
    threads.clear();

    /*
     * Output
     */
    
	elapsed_ns = (ts_end.tv_sec - ts_start.tv_sec)*1000000000 + (ts_end.tv_nsec - ts_start.tv_nsec);

	printf("Time ouput\n");
	printf("Elapsed (ns): %llu\n",elapsed_ns);

	elapsed_s = ((double)elapsed_ns)/1000000000.0;
	printf("Elapsed (s): %lf\n",elapsed_s);


    for ( int i=0; i < outputs.size(); i++) {
        printf("%d\n", outputs.at(i));
    }
    /* 
     * writing output to file 
     */
    for ( vector<int>::iterator it = outputs.begin(); it != outputs.end(); it++ ) {
        fprintf(out_file,"%d\n", *it);
    }
    fclose(out_file);

} 