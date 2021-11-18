
#include "idx.h"

#include <atomic>
#include <iostream>

#include "../util/chrono.h"

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

