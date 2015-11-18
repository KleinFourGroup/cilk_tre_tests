#include <cilk/cilk.h>
#include <atomic>
#include <iostream>
#include <stdlib.h>
#include <time.h>

void print_arr(int* A, int n) {
  for(int i = 0; i < n; i++) std::cout << A[i];
  std::cout << std::endl;
}

void arr_cpy(int* A, int* B, int n) {
  for(int i = 0; i < n; i++) A[i] = B[i];
}

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

void merge_sort(int *B, int *A, int n) {
  if (n==1) {
    B[0] = A[0];
  } else {
    int C[n];
    merge_sort(C, A, n/2);
    merge_sort(C+n/2, A+n/2, n-n/2);
    merge(B, C, n/2, C+n/2, n-n/2);
  }
}

void cmerge_sort(int *B, int *A, int n) {
  if (n==1) {
    B[0] = A[0];
  } else {
    int C[n];
    cilk_spawn cmerge_sort(C, A, n/2);
    cmerge_sort(C+n/2, A+n/2, n-n/2);
    cilk_sync;
    merge(B, C, n/2, C+n/2, n-n/2);
  }
}

void merge_sort_tre(int *B, int *A, int n) {
  //std::cout << n << std::endl;
  if (n==1) {
    B[0] = A[0];
  } else {
    int C[n];
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
  }
}

void cmerge_sort_tre(int *B, int *A, int n) {
  //std::cout << n << std::endl;
  if (n==1) {
    B[0] = A[0];
  } else {
    int C[n];
    int * Cp = C;
    int * Ap = A;
    int m = n;
    while(m > 1) {
      cilk_spawn cmerge_sort_tre(Cp, Ap, m/2);
      Cp = Cp + m/2;
      Ap = Ap + m/2;
      m = m - m/2;
      //merge_sort(C+n/2, A+n/2, n-n/2);
    }
    cilk_sync;
    B[n-1] = Ap[0];
    m = 1;
    while(m < n) {
      merge(B+n-2*m, Cp-m, m, B+n-m, m);
      Cp = Cp - m;
      m = 2*m;
      //arr_cpy(Cp, B+n-m, m);
    }
  }
}

bool isSorted(int *B, int n) {
  for(int i = 0; i < n - 1; i++) {
    if(B[i+1] < B[i]) return false;
  }
  return true;
}

int main() {
  clock_t t;
  int n = 1<<18;
  std::cout << n << std::endl;
  srand(time(NULL));
  int A[n];
  for(int i = 0; i < n; i++) A[i] = rand()%10;
  //print_arr(A, n);
  int B[n];
  t = clock();
  merge_sort(B, A, n);
  t = clock() - t;
  std::cout <<  "In " << t * 1.0 / CLOCKS_PER_SEC << " merge_sort is " << isSorted(B, n) << std::endl;
  //print_arr(B, n);
  t = clock();
  cmerge_sort(B, A, n);
  t = clock() - t;
  std::cout <<  "In " << t * 1.0 / CLOCKS_PER_SEC << " cmerge_sort is " << isSorted(B, n) << std::endl;
  //print_arr(B, n);
  t = clock();
  merge_sort_tre(B, A, n);
  t = clock() - t;
  std::cout <<  "In " << t * 1.0 / CLOCKS_PER_SEC << " merge_sort_tre is " << isSorted(B, n) << std::endl;
  //print_arr(B, n);
  t = clock();
  cmerge_sort_tre(B, A, n);
  t = clock() - t;
  std::cout <<  "In " << t * 1.0 / CLOCKS_PER_SEC << " cmerge_sort_tre is " << isSorted(B, n) << std::endl;
  //print_arr(B, n);
  return 0;
}
