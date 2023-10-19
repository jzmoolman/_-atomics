
#include <iostream>
#include <getopt.h> /* struct option  */
#include <thread>
#include <barrier>

#include "./include/zlock.h"
#include "./include/taslock.h"
#include "./include/ttaslock.h"
#include "./include/ticketlock.h"
#include "./include/ticketlock.h"
#include "./include/mcslock.h"

using namespace std;

// Lock type 
enum {
    lt_none,
    lt_tas,
    lt_ttas,
    lt_ticket,
    lt_mcs
};

// Program stuff
const char *program_name;  
const char *name = "Zach Moolman";
size_t nthreads = 5;     // default threads
int locktype = lt_none;

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
            {"name", no_argument, 0, 'n'},
			{"thread", required_argument, 0, 't'},
            {"lock", required_argument, 0, 'l'},
            {0, 0, 0, 0}
        };

        int option_index = 0;
        c = getopt_long ( argc, argv, "nt:l:", long_options, &option_index);

        if ( c == -1) 
            break;

        switch (c) {
            case 'n':
                printf("option -n with value `%s'\n", name);
                exit(0);
                break;
			case 't':
	            nthreads = atoi(optarg);
				if((nthreads > 150) || (nthreads == 0)) {
					printf("ERROR: invalid number of threads\n");
				    abort();
				}
				break;	

            case 'l':
                if (strcmp(optarg, "tas") == 0)
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
        printf("Lock not specified\n");
        abort();
    }
}


// boiler plate for testing 
vector<thread*> threads;
barrier<> bar(nthreads);
barrier<> mcs_bar(nthreads-1);
struct timespec time_start, time_end;

// Test subject
vector<int> SharedData;  

// Lock to test
ZLock *lck; 
MCSLock *mcs;

void thread_func(int tid) {
    //Sync Threads
	bar.arrive_and_wait();
	if (tid == 0) {
		clock_gettime(CLOCK_MONOTONIC, &time_start);
    }
    bar.arrive_and_wait();

    // subject code
    thread_local int cnt = 0;

    while ( cnt < 1000){
        lck->lock();
        {
            SharedData[0]++;
            cout << "id " << tid << " value " << SharedData[0] << endl;
        }
        lck->unlock();
        cnt++;
        // this_thread::sleep_for(chrono::microseconds(5));
    }

    //Sync Thread for
    bar.arrive_and_wait();
	if (tid == 0) {
		clock_gettime(CLOCK_MONOTONIC, &time_end);
    }
}

void thread_func_mcs(int tid) {

    //Sync Threads
	mcs_bar.arrive_and_wait();
	if (tid == 1) {
		clock_gettime(CLOCK_MONOTONIC, &time_start);
    }
    mcs_bar.arrive_and_wait();

    // subject code
    thread_local int cnt = 0;
    thread_local qnode_t p;
    while ( cnt < 1000){
        mcs->acquire(&p, tid);
        {
            SharedData[0]++;
            cout << "id " << tid << " value " << SharedData[0] << endl;
        }
        mcs->release(&p, tid);
        cnt++;
        // this_thread::sleep_for(chrono::microseconds(5));
    }

    //Sync Thread for
    mcs_bar.arrive_and_wait();
	if (tid == 1) {
		clock_gettime(CLOCK_MONOTONIC, &time_end);
    }
}

int main(int argc, char** argv) {
    cout << "Atomic test program" << endl;
    handle_args(argc, argv);

    // Target var
    SharedData = {0,0};
    
    // Output var
    unsigned long long elapsed_ns;
	double elapsed_s;
    switch (locktype)
    {
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
    //thread_func(0);     

    /*
     * Join threads
     */
    
    for(size_t i=1; i < nthreads; i++) {
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