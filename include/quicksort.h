#include <vector>
#include <stdio.h>

#include <limits.h>


int _partition_sort(std::vector<int> *l,int p, int r ) {
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

void quick_sort(std::vector<int> *l, int p, int r) {
    if ( l == NULL ) return;
    if (p < r) {
        int q = _partition_sort(l, p, r);
#ifdef DEBUG_ENABLED
        printf("p = %d r = %d q = %d\n", p, r, q);
#endif
        quick_sort(l, p, q-1);
        quick_sort(l, q+1, r);
    }
}