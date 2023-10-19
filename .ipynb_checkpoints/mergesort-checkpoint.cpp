#include <vector>

#include <limits.h>
#include "mergesort.h"


void _merge (std::vector <int> *l, int p, int q, int r) {
    int n1, n2, *L, *R, i, j;
    n1 = q - p + 1;
    n2 = r - q;

    L = new int[n1 + 1];
    R = new int[n2 + 1];


    for (i = 0; i < n1; i++) 
      L[i] = l->at(p+i);
    
    for (i = 0; i < n2; i++) 
        R[i] = l->at(q+1+i);
    
    L[n1] = INT_MAX;
    R[n2] = INT_MAX;
    i = 0;
    j = 0;
    for (int k = p; k <=r; k++) {
        if (( L[i] != INT_MAX) && (L[i] <= R[j])) {
              l->at(k) = L[i];
              i++;
        } else {
            if ( R[j] != INT_MAX) 
              l->at(k) = R[j];
              j++;
        }
    }
}

void merge_sort(std::vector<int> *l, int p, int r) {
  int q;
  if (p < r) {
    q = (p +r ) /2;
    merge_sort(l, p, q);
    merge_sort(l, q + 1, r);
    _merge(l, p, q, r);
  }
}
