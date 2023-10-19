#include <iostream>
#include <string.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include <vector>

#include "mergesort.h"

using namespace std;

// #define DEBUG_ENABLED
#define INPUT_BUFFER_SIZE 128
char *program_name;
const char *name = "Zach Moolman";
char* opt_in_file = NULL;
char* opt_out_file = NULL;

#define ALG_QUICK 0
#define ALG_MERGE 1
int opt_alg = -1;


int partition_sort(vector<int> * l,int p, int r ) {
  int pvalue = l->at(r);
  int i = p-1;
  for (int j = p; j < r; j++) {
    if ( l->at(j) <= pvalue) {
      i++;
      int tmp = l->at(i);
      l->at(i) = l->at(j);
      l->at(j) = tmp;
    }
  }
  int tmp = l->at(i+1);
  l->at(i+1) = l->at(r);
  l->at(r) = tmp;
  return  i+1;
}

void quick_sort(vector<int> *l, int p, int r) {
    if (p < r) {
        int q = partition_sort(l, p, r);
#ifdef DEBUG_ENABLED
        printf("p = %d r = %d q = %d\n", p, r, q);
#endif
        pthread_t lt;
        quick_sort(l, p, q-1);
        quick_sort(l, q+1, r);
    }
}

void print_list(vector<int> * l) {
    for ( vector<int>::iterator it = l->begin(); it != l->end(); it++ ) {
        cout << *it << endl;
    }
}


int main(int argc, char** argv) {
    char *cp; /* pointer to end of  prgram last '/' */
    FILE * in_file;  /* input file */
    FILE * out_file; /* output file */
    char buffer[INPUT_BUFFER_SIZE]; /* input text buffer */
    vector<int> in_list; /* input list */
    vector<int> out_list; /* output list */

	if ((cp = strrchr(argv[0], '/')) != NULL)
		program_name = cp + 1;
	else
		program_name = argv[0];

    int c;
    while (1) {
        static struct option long_options[] = {
            {"name", no_argument, 0, 'n'},
            {"insert", required_argument, 0, 'i'},
            {"out", required_argument, 0, 'o'},
            {"alg", required_argument, 0, 'a'},
            {0, 0, 0, 0}
        };

        int option_index = 0;
        c = getopt_long ( argc, argv, "ni:o:a:", long_options, &option_index);

        if ( c == -1) 
            break;

        switch (c) {
            case 'n':
                printf("option -n with value `%s'\n", name);
                exit(0);
                break;
            case 'i':
                opt_in_file = optarg;
                break;
            case 'o':
                opt_out_file = optarg;
                break;
            case 'a':
                if (strcmp(optarg, "merge") == 0)
                   opt_alg = ALG_MERGE;
                else if (strcmp(optarg, "quick") == 0)
                   opt_alg = ALG_QUICK;
                else  {
                    printf("Algorithm not recognised `%s'\n", optarg);
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
        printf("non-option ARGV-elemets: ");
        while ( optind < argc) 
            printf("%s ", argv[optind++]);
        puts("\n");
    }
    
    if ( opt_in_file == NULL) {
#ifdef DEBUG_ENABLED
        static char* in_file_buffer = "321.txt";
        opt_in_file = in_file_buffer;
#endif
#ifndef DEBUG_ENABLED
        printf("Input file not specified\n");
        abort();
#endif
    }

    if ( opt_out_file == NULL) {
#ifdef DEBUG_ENABLED
        static char* out_file_buffer = "123.txt";
        opt_out_file = out_file_buffer;
#endif
#ifndef DEBUG_ENABLED
        printf("Output file not specified\n");
        abort();
#endif
    }

    if ( opt_alg == -1) {
        printf("Algorithm not specified\n");
        abort();
    }

#ifdef DEBUG_ENABLED
    printf("Input file: %s\n", opt_in_file);
    printf("Output file: %s\n", opt_out_file);
    printf("Algorithm: %s\n", opt_alg==ALG_MERGE?"Merge":"Quick");
#endif

    /* Getting ready to exe program..  */
	in_file = fopen(opt_in_file, "r");
	if (in_file == NULL) {
		fprintf(stderr, "Unable to open file: %s\n", opt_in_file);
        abort();
    }

	out_file = fopen(opt_out_file, "w+");
	if (out_file == NULL) {
		fprintf(stderr, "Unable to open file: %s\n", opt_out_file);
        abort();
    }
    
    /* reading input from in_file */
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
                in_list.push_back((int)sl);
            }
        }
    }
    fclose(in_file);
#ifdef DEBUG_ENABLED
    print_list(&in_list);
#endif
    if ( opt_alg == ALG_QUICK)
        quick_sort(&in_list, 0, in_list.size()-1);
    else 
        // quick_sort(&in_list, 0, in_list.size()-1);
        merge_sort(&in_list, 0, in_list.size()-1);

#ifdef DEBUG_ENABLED
    print_list(&in_list);
#endif


    /* writing output  */
    for ( vector<int>::iterator it = in_list.begin(); it != in_list.end(); it++ ) {
        fprintf(out_file,"%d\n", *it);
    }
    fclose(out_file);

}