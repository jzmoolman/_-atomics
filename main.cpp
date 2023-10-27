
#include <iostream>
#include <getopt.h> /* struct option  */
#include <thread>
#include <barrier>

#include "./include/zbarrier.h"
#include "./include/pthreadbarrier.h" // wrapper for c++ barrier class
#include "./include/senserelbarrier.h"

#include "./include/zlock.h"
#include "./include/pthreadlock.h"   // Wrapper for c++ mutex class
#include "./include/taslock.h"
#include "./include/ttaslock.h"
#include "./include/ticketlock.h"
#include "./include/ticketlock.h"
#include "./include/mcslock.h"

using namespace std;

// Lock type 
enum {
    lt_none,
    lt_pthread,
    lt_tas,
    lt_ttas,
    lt_ticket,
    lt_mcs
};

enum {
    bt_none,
    bt_pthread,
    bt_senserel
};

// boiler plate for testing 
vector<thread*> threads;
struct timespec time_start, time_end;

// Test subject
vector<int> SharedData;  

// Program stuff
const char *program_name;  
const char *name = "Zach Moolman";
size_t nthreads = 5;     // default threads including the main thread?? 
int locktype = lt_none;
int barriertype = bt_none;
int niterations = 100;

// Lock & barriers to test
ZBarrier *bar;
ZLock *lck; 
MCSLock *mcs;

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

    if ( locktype == lt_none) {
        locktype = lt_pthread;
        printf("Lock not specified, using default -> pthread\n");
    }

    if ( barriertype == bt_none) {
        barriertype = bt_pthread;
        printf("Barrier not specified, using default -> pthread\n");
    }

}

void thread_func(int tid) {
    //Sync Threads
	bar->wait();
	if (tid == 0) {
        printf("Thread 4 is getting time");
		clock_gettime(CLOCK_MONOTONIC, &time_start);
    }
    bar->wait();

    // subject code
    thread_local int cnt = 0;

    while ( cnt < niterations){
        lck->lock();
        {
            SharedData[0]++;
            // cout << "id " << tid << " value " << SharedData[0] << endl;
        }
        lck->unlock();
        cnt++;
        // this_thread::sleep_for(chrono::microseconds(5));
    }

    printf("Thread %d is done", tid);
    //Sync Thread for
    bar->wait();
	if (tid == 0) {
		clock_gettime(CLOCK_MONOTONIC, &time_end);
    }
}

void thread_func_mcs(int tid) {

    //Sync Threads
	bar->wait();
	if (tid == 1) {
		clock_gettime(CLOCK_MONOTONIC, &time_start);
        printf("Thread 1 is getting time");
    }
    bar->wait();

    // subject code
    thread_local int cnt = 0;
    thread_local qnode_t p;
    while ( cnt < 1000){
        mcs->acquire(&p, tid);
        {
            SharedData[0]++;
            // cout << "id " << tid << " value " << SharedData[0] << endl;
        }
        mcs->release(&p, tid);
        cnt++;
        // this_thread::sleep_for(chrono::microseconds(5));
    }

    //Sync Thread for

    printf("Thread %d is done", tid);
    bar->wait();
	if (tid == 1) {
		clock_gettime(CLOCK_MONOTONIC, &time_end);
    }
}


void thread_func_senserel(int tid) {

    //Sync Threads
	bar->wait();
	if (tid == 1) {
		clock_gettime(CLOCK_MONOTONIC, &time_start);
        printf("Thread %d getting time_start\n", tid);
    }
    
    bar->wait();
	if (tid == 4) {
		clock_gettime(CLOCK_MONOTONIC, &time_end);
        printf("Thread %d getting time_end\n", tid);
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

    switch ( barriertype ) {
    case bt_senserel:
        bar = new SenserelBarrier(nthreads);
        break;
    default:
        bar = new PThreadBarrier(nthreads);
        break;
    }

    switch (locktype) {
        case lt_pthread:
            lck = new PThreadLock;
            break;
        case lt_tas:
            lck = new TASLock;
            break;
        case lt_ttas:
            lck = new TTASLock;
            break;
        case lt_ticket:
            lck = new TicketLock;
            break;
        case lt_mcs:
            mcs = new MCSLock;
        default:
            break;
    }




    /*
     * Launch threads
     */

    threads.push_back(0); // reserve for main thread
    for(size_t i = 1; i < nthreads; i++){
        thread *t;
        if (locktype == lt_mcs)
            t =  new thread(thread_func_mcs,i);
        else 
            t =  new thread(thread_func,i);

        threads.push_back(t);

    }
   
    if (locktype != lt_mcs) {
        thread_func(0);     
    }


    /*
     * Join threads
     */
    
    for(size_t i=1; i < nthreads-1; i++) {
        threads[i]->join();
        delete threads[i];
    }
    threads.clear();


    /*
     * Output
     */

    
	elapsed_ns = (time_end.tv_sec-time_start.tv_sec)*1000000000 + (time_end.tv_nsec-time_start.tv_nsec);

	printf("Time ouput\n");
	printf("Elapsed (ns): %llu\n",elapsed_ns);

	elapsed_s = ((double)elapsed_ns)/1000000000.0;
	printf("Elapsed (s): %lf\n",elapsed_s);

} 