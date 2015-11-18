#include <cilk/cilk.h>
#include <atomic>
#include <iostream>
#include <time.h>

int fib(int n) {
  if (n < 2) return n;
  int x,y;
  x = fib(n - 1);
  y = fib(n - 2);
  return x + y;
}

int cfib(int n) {
  if (n < 2) return n;
  int x,y;
  x = cilk_spawn cfib(n - 1);
  y = cfib(n - 2);
  cilk_sync;
  return x + y;
}

int fibt(int n) {
  if (n < 2) return n;
  int x = 0;
  while (n >= 2) {
    int y = fibt(n - 1);
    x += y;
    n -= 2;
  }
  return x + n;
}

int fibt_at(int n) {
  if (n < 2) return n;
  std::atomic<int> x(0);
  while (n >= 2) {
    int y = fibt_at(n - 1);
    std::atomic_fetch_add(&x, y);
    n -= 2;
  }
  std::atomic_fetch_add(&x, n);
  int ret = x;
  return ret;
}

int cfibt_at(int n) {
  if (n < 2) return n;
  std::atomic<int> x(0);
  while (n >= 2) {
    int y = cilk_spawn cfibt_at(n - 1);
    std::atomic_fetch_add(&x, y);
    n -= 2;
  }
  cilk_sync;
  std::atomic_fetch_add(&x, n);
  int ret = x;
  return ret;
}

int fibt_arr(int n) {
  if (n < 2) return n;
  int x = 0;
  int MAX = n/2;
  int ind = 0;
  int y[MAX];
  while (n >= 2) {
    y[ind] = fibt_arr(n - 1);
    ind ++;
    n -= 2;
  }
  for(ind = 0; ind < MAX; ind++) x += y[ind];
  return x + n;
}

int cfibt_arr(int n) {
  if (n < 2) return n;
  int x = 0;
  int MAX = n/2;
  int ind = 0;
  int y[MAX];
  while (n >= 2) {
    y[ind] = cilk_spawn cfibt_arr(n - 1);
    ind ++;
    n -= 2;
  }
  cilk_sync;
  for(ind = 0; ind < MAX; ind++) x += y[ind];
  return x + n;
}

int main() {
  clock_t t;
  int in = 40;
  int n;
  t = clock();
  n = fib(in);
  t = clock() - t;
  std::cout << n << " fib       " << t * 1.0 / CLOCKS_PER_SEC << std::endl;
  t = clock();
  n = cfib(in);
  t = clock() - t;
  std::cout << n << " cfib      " << t * 1.0 / CLOCKS_PER_SEC << std::endl;
  t = clock();
  n = fibt(in);
  t = clock() - t;
  std::cout << n << " fibt      " << t * 1.0 / CLOCKS_PER_SEC << std::endl;
  t = clock();
  n = fibt_at(in);
  t = clock() - t;
  std::cout << n << " fibt_at   " << t * 1.0 / CLOCKS_PER_SEC << std::endl;
  t = clock();
  n = cfibt_at(in);
  t = clock() - t;
  std::cout << n << " cfibt_at  " << t * 1.0 / CLOCKS_PER_SEC << std::endl;
  t = clock();
  n = fibt_arr(in);
  t = clock() - t;
  std::cout << n << " fibt_arr  " << t * 1.0 / CLOCKS_PER_SEC << std::endl;
  t = clock();
  n = cfibt_arr(in);
  t = clock() - t;
  std::cout << n << " cfibt_arr " << t * 1.0 / CLOCKS_PER_SEC << std::endl;
  return 0;
}
