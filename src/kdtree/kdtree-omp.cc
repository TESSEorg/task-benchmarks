
#include "idx.h"

#include <atomic>
#include <iostream>
#include <chrono>

using time_point = std::chrono::high_resolution_clock::time_point;

inline time_point now() { return std::chrono::high_resolution_clock::now(); }

inline std::chrono::system_clock::time_point system_now() {
  return std::chrono::system_clock::now();
}

inline int64_t duration_in_mus(time_point const &t0, time_point const &t1) {
  return std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
}

std::atomic<int> task_counter = 0;

static void taskfn(const Idx& idx)
{
  {
    ++task_counter;
    if (idx.l+1 < LMAX) {
      #pragma omp task shared(task_counter) firstprivate(idx)
        taskfn(Idx(idx.l + 1, {{idx.x[0] * 2}}));
      #pragma omp task shared(task_counter) firstprivate(idx)
        taskfn(Idx(idx.l + 1, {{idx.x[0] * 2+1}}));
    }
  }
}

int main(){
  
  auto t0 = now();

#pragma omp parallel master
  {
    #pragma omp task
    {
      taskfn(Idx{});
    }
    #pragma omp taskwait
  }

  auto t1 = now();
  std::cout << "# of tasks = " << task_counter.load() << std::endl;
  std::cout << "time elapsed (microseconds) = " << duration_in_mus(t0, t1) << std::endl;

  return 0;
}

