
#include <iostream>
#include <getopt.h> /* struct option  */
#include <thread>
#include <barrier>

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

// boiler plate for testing 
vector<thread*> threads;
struct timespec ts_start, ts_end;
barrier<> *bar_timer;

// Test subject
vector<int> SharedData;  

// Program stuff
const char *program_name;  
const char *name = "Zach Moolman";
size_t nthreads = 5;     // default threads including the main thread?? 
int locktype = lt_none;
int barriertype = bt_none;
int niterations = 1000;

// Lock & barriers to test
ZBarrier *bar;
ZLock *lck; 
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
    while (1) {
        static struct option long_options[] = {
            {"name", no_argument, 0, 'a'},
            {"number", required_argument, 0, 'n'},
            {"bar", required_argument, 0, 'b'},
			{"thread", required_argument, 0, 't'},
            {"lock", required_argument, 0, 'l'},
            {0, 0, 0, 0}
        };

        int option_index = 0;
        c = getopt_long ( argc, argv, "an:b:t:l:", long_options, &option_index);

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

    if (( locktype == lt_none &&  barriertype == bt_none) || (locktype != lt_none &&  barriertype != bt_none)) {
        printf("Please specify either a lock or barrier to test\n");
        abort();
    }

}

void thread_func_barrier(int tid) {
    //Sync Threads
	bar_timer->arrive_and_wait();
	if (tid == 1) {
		clock_gettime(CLOCK_MONOTONIC, &ts_start);
        printf("Thread %d getting time_start %d.%d\n", tid,ts_start.tv_sec, ts_start.tv_nsec);
    }

    thread_local int cnt = 0;

    for(int i = 0; i<niterations*nthreads; i++){
        if(i%nthreads == tid) {
                SharedData[0]++;
        }
        bar->wait();
    }

    bar_timer->arrive_and_wait();
    if (tid == 1) {
		clock_gettime(CLOCK_MONOTONIC, &ts_end);
        if ( ts_start.tv_sec == ts_end.tv_sec && ts_start.tv_nsec > ts_end.tv_nsec) {
            ts_end.tv_sec += 1;
        }
        printf("Thread %d getting time_end %d.%d\n", tid,ts_end.tv_sec, ts_end.tv_nsec);
    }
}


void thread_func_lock(int tid) {
    //Sync Threads
	bar_timer->arrive_and_wait();
	if (tid == 1) {
		clock_gettime(CLOCK_MONOTONIC, &ts_start);
        printf("Thread %d getting time_start %d.%d\n", tid,ts_start.tv_sec, ts_start.tv_nsec);
    }

    // subject code
    thread_local int cnt = 0;

    while ( cnt < niterations){
        lck->lock();
        {
            SharedData[0]++;
        }
        lck->unlock();
        cnt++;
    }

    bar_timer->arrive_and_wait();
    if (tid == 1) {
		clock_gettime(CLOCK_MONOTONIC, &ts_end);
        if ( ts_start.tv_sec == ts_end.tv_sec && ts_start.tv_nsec > ts_end.tv_nsec) {
            ts_end.tv_sec += 1;
        }
        printf("Thread %d getting time_end %d.%d\n", tid,ts_end.tv_sec, ts_end.tv_nsec);
    }
}



void thread_func_mcs(int tid) {

    //Sync Threads
	bar_timer->arrive_and_wait();
	if (tid == 1) {
		clock_gettime(CLOCK_MONOTONIC, &ts_start);
        printf("Thread %d getting time_start %d.%d\n", tid,ts_start.tv_sec, ts_start.tv_nsec);
    }

    // subject code
    thread_local int cnt = 0;
    thread_local qnode_t p;
    for ( int cnt = 0; cnt < niterations; cnt++){
        mcs->acquire(&p, tid);
        {
            SharedData[0]++;
        }
        mcs->release(&p, tid);
    }

    //Sync Thread for

    bar_timer->arrive_and_wait();
    if (tid == 1) {
		clock_gettime(CLOCK_MONOTONIC, &ts_end);
        if ( ts_start.tv_sec == ts_end.tv_sec && ts_start.tv_nsec > ts_end.tv_nsec) {
            ts_end.tv_sec += 1;
        }
        printf("Thread %d getting time_end %d.%d\n", tid,ts_end.tv_sec, ts_end.tv_nsec);
    }
}

void thread_func_peterson(int tid) {

    //Sync Threads
	bar_timer->arrive_and_wait();
	if (tid == 1) {
		clock_gettime(CLOCK_MONOTONIC, &ts_start);
        printf("Thread %d getting time_start %d.%d\n", tid,ts_start.tv_sec, ts_start.tv_nsec);
    }


    for ( int cnt = 0; cnt < niterations; cnt++){
        if (locktype == lt_peterson)
            peterson->lock(tid);
        else 
            petersonrel->lock(tid);
        
        {
            SharedData[0]++;
        }
        
        if (locktype == lt_peterson)
            peterson->unlock(tid);
        else 
            petersonrel->unlock(tid);
    }
    
    bar_timer->arrive_and_wait();
    if (tid == 1) {
		clock_gettime(CLOCK_MONOTONIC, &ts_end);
        if ( ts_start.tv_sec == ts_end.tv_sec && ts_start.tv_nsec > ts_end.tv_nsec) {
            ts_end.tv_sec += 1;
        }
        printf("Thread %d getting time_end %d.%d\n", tid,ts_end.tv_sec, ts_end.tv_nsec);
    }

}


int main(int argc, char** argv) {
    // cout << "Atomic test program" << endl;
    handle_args(argc, argv);

    // Target var
    SharedData = {0,0};
    
    // Output var
    unsigned long long elapsed_ns;
	double elapsed_s;

    if ((barriertype == bt_sense || barriertype == bt_senserel || barriertype == bt_pthread) && locktype == lt_none) {
        //do barrier test
        bar_timer = new barrier(nthreads);
        switch ( barriertype ) {
        case bt_sense:
            bar = new SenseBarrier(lo_weak, nthreads);
            break;
        case bt_senserel:
            bar = new SenseBarrier(lo_strong, nthreads);
            break;
        default:
        bar = new PThreadBarrier(nthreads);
            break;
        }
        
        threads.push_back(0); // reserve for main thread
        for(size_t i = 1; i < nthreads; i++) {
            thread *t;
            t =  new thread(thread_func_barrier, i);
            threads.push_back(t);
        }
        thread_func_barrier(0);

    } else {
        //do lock test
        switch (locktype) {
            case lt_pthread:
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

        /*
        * Launch threads
        */

        bar_timer = new barrier(nthreads);
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
	printf("shared[0] ): %d\n",SharedData[0]);

} 