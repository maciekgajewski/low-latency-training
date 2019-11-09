#include <atomic>
#include <iostream>
#include <thread>

#include <x86intrin.h>

volatile int X = 0;
volatile int Y = 0;
volatile int r1 = 0;
volatile int r2 = 0;
//         std::atomic_signal_fence(std::memory_order_relaxed);

std::atomic<int> threadsReady;

int main() {
  int errors = 0;
  int iterations = 0;
  while (true) {
    X = 0;
    Y = 0;
    r1 = 0;
    r2 = 0;

    threadsReady = 0;

    std::thread t1([] {
      threadsReady++;
      while (threadsReady < 2)
        ;
      X = 1;
      r1 = Y;
    });
    std::thread t2([] {
      threadsReady++;
      while (threadsReady < 2)
        ;
      Y = 1;
      r2 = X;
    });

    t1.join();
    t2.join();

    if (r1 == 0 && r2 == 0)
      errors++;
    iterations++;
    if (iterations % 1000 == 0)
      std::cout << errors << "/" << iterations << std::endl;
  }
}
