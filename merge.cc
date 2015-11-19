#include <cilk/cilk.h>
#include <atomic>
#include <iostream>
#include <stdlib.h>
#include <time.h>

void merge(int *C, int *A, int na, int *B, int nb) {
  while (na>0 && nb>0) {
    if (*A <= *B) {
      *C++ = *A++; na--;
    } else {
      *C++ = *B++; nb--;
    }
  }
  while (na>0) {
    *C++ = *A++; na--;
  }
  while (nb>0) {
    *C++ = *B++; nb--;
  }
}

void merge_sort_tre(int *B, int *A, int n) {
  //std::cout << n << std::endl;
  if (n==1) {
    B[0] = A[0];
  } else {
    int * C = (int *)malloc(n*sizeof(int));
    int * Cp = C;
    int * Ap = A;
    int m = n;
    while(m > 1) {
      merge_sort_tre(Cp, Ap, m/2);
      Cp = Cp + m/2;
      Ap = Ap + m/2;
      m = m - m/2;
      //merge_sort(C+n/2, A+n/2, n-n/2);
    }
    B[n-1] = Ap[0];
    m = 1;
    while(m < n) {
      merge(B+n-2*m, Cp-m, m, B+n-m, m);
      Cp = Cp - m;
      m = 2*m;
      //arr_cpy(Cp, B+n-m, m);
    }
    free(C);
  }
}

void print_arr(int* A, int n) {
  for(int i = 0; i < n; i++) std::cout << A[i];
  std::cout << std::endl;
}

void arr_cpy(int* A, int* B, int n) {
  for(int i = 0; i < n; i++) A[i] = B[i];
}

bool isSorted(int *B, int n) {
  for(int i = 0; i < n - 1; i++) {
    if(B[i+1] < B[i]) return false;
  }
  return true;
}

int binary_search(int k, int * A, int n) {
  int lo = 0;
  int hi = n - 1;
  int m;
  while(lo <= hi) {
    m = (lo + hi) / 2;
    if(A[m] < k) {
      lo = m + 1;
    } else if (A[m] > k) {
      hi = m - 1;
    } else {
      return m;
    }
  }
  return lo;
}

void rec_merge(int *C, int *A, int na, int *B, int nb) {
  if (na < nb) {
    rec_merge(C, B, nb, A, na);
  } else if (na==0) {
    return;
  } else {
    int ma = na/2;
    int mb = binary_search(A[ma], B, nb);
    C[ma+mb] = A[ma];
    rec_merge(C, A, ma, B, mb);
    rec_merge(C+ma+mb+1, A+ma+1, na-ma-1, B+mb, nb-mb);
  }
}

void crec_merge(int *C, int *A, int na, int *B, int nb) {
  if (na < nb) {
    crec_merge(C, B, nb, A, na);
  } else if (na==0) {
    return;
  } else {
    int ma = na/2;
    int mb = binary_search(A[ma], B, nb);
    C[ma+mb] = A[ma];
    cilk_spawn crec_merge(C, A, ma, B, mb);
    crec_merge(C+ma+mb+1, A+ma+1, na-ma-1, B+mb, nb-mb);
    cilk_sync;
  }
}

void rec_merge_tre(int *C, int *A, int na, int *B, int nb) {
  while (na > 0) {
    int ma = na/2;
    int mb = binary_search(A[ma], B, nb);
    C[ma+mb] = A[ma];
    rec_merge_tre(C, A, ma, B, mb);
    C = C+ma+mb+1;
    A = A+ma+1;
    na = na-ma-1;
    B = B+mb;
    nb = nb-mb;
    if (na < nb) {
      int *tmp = B;
      B = A;
      A = tmp;
      int t = nb;
      nb = na;
      na = t;
    }
  }
}

void crec_merge_tre(int *C, int *A, int na, int *B, int nb) {
  while (na > 0) {
    int ma = na/2;
    int mb = binary_search(A[ma], B, nb);
    C[ma+mb] = A[ma];
    cilk_spawn crec_merge_tre(C, A, ma, B, mb);
    C = C+ma+mb+1;
    A = A+ma+1;
    na = na-ma-1;
    B = B+mb;
    nb = nb-mb;
    if (na < nb) {
      int *tmp = B;
      B = A;
      A = tmp;
      int t = nb;
      nb = na;
      na = t;
    }
  }
  cilk_sync;
}

int main() {
  clock_t t;
  int n = 1<<25;
  std::cout << "Merging two random arrays of size " << n << std::endl;
  srand(time(NULL));
  float ans[5];
  for(int i = 0; i < 5; i++) ans[5] = 0.0;
  
  for(int round = 0; round < 10; round++) {
    int * A = (int *)malloc(n*sizeof(int));
    int * B = (int *)malloc(n*sizeof(int));
    int * M = (int *)malloc(2*n*sizeof(int));
    int * tmp = (int *)malloc(n*sizeof(int));
    
    for(int i = 0; i < n; i++) A[i] = rand()%10000000;
    for(int i = 0; i < n; i++) B[i] = rand()%10000000;
    merge_sort_tre(tmp, A, n);
    arr_cpy(A, tmp, n);
    merge_sort_tre(tmp, B, n);
    arr_cpy(B, tmp, n);

    std::cout << "Starting round " << round+1 << "..." << std::endl;

    for(int i = 0; i < 2*n; i++) M[i] = 0;
    t = clock();
    merge(M, A, n, B, n);
    t = clock() - t;
    ans[0] += t * 1.0 / CLOCKS_PER_SEC;
    std::cout <<  "In " << ans[0] / (round + 1) << " merge is " << isSorted(M, n) << std::endl;
    //print_arr(B, n);
  
    for(int i = 0; i < 2*n; i++) M[i] = 0;
    t = clock();
    rec_merge(M, A, n, B, n);
    t = clock() - t;
    ans[1] += t * 1.0 / CLOCKS_PER_SEC;
    std::cout <<  "In " << ans[1] / (round + 1) << " rec_merge is " << isSorted(M, n) << std::endl;
    //print_arr(B, n);
  
    for(int i = 0; i < 2*n; i++) M[i] = 0;
    t = clock();
    crec_merge(M, A, n, B, n);
    t = clock() - t;
    ans[2] += t * 1.0 / CLOCKS_PER_SEC;
    std::cout <<  "In " << ans[2] / (round + 1) << " crec_merge is " << isSorted(M, n) << std::endl;
    //print_arr(B, n);
  
    for(int i = 0; i < 2*n; i++) M[i] = 0;
    t = clock();
    rec_merge_tre(M, A, n, B, n);
    t = clock() - t;
    ans[3] += t * 1.0 / CLOCKS_PER_SEC;
    std::cout <<  "In " << ans[3] / (round + 1) << " rec_merge_tre is " << isSorted(M, n) << std::endl;
    //print_arr(B, n);
  
    for(int i = 0; i < 2*n; i++) M[i] = 0;
    t = clock();
    crec_merge_tre(M, A, n, B, n);
    t = clock() - t;
    ans[4] += t * 1.0 / CLOCKS_PER_SEC;
    std::cout <<  "In " << ans[4] / (round + 1) << " crec_merge_tre is " << isSorted(M, n) << std::endl;
    //print_arr(B, n);
    
    free(A);
    free(B);
    free(M);
    free(tmp);
  }
  return 0;
}
