#include <omp.h>
#include <iostream>
#include <atomic>

#include "../util/chrono.h"

std::atomic<int> task_counter = 0;
static int N = 10000000;

static void recursive_taskfn()
{
  #pragma omp task shared(task_counter)
  {
    if (task_counter < N) {
      ++task_counter;
      // resubmit recursively
      recursive_taskfn();
    }
  }
  #pragma omp taskwait
}

static void dependency_taskfn()
{
  for (int i = 0; i < N; ++i) {
    #pragma omp task depend(out:task_counter)
    {
      ++task_counter;
    }
  }
  #pragma omp taskwait
}

int main(){
  
  auto t0 = now();

  /* NOTE: recursively submitting tasks leads
   *       to stack exhaustion with both GCC-9
   *       and Clang-14 */
  constexpr const bool use_deps = true;

#pragma omp parallel
  {
    if constexpr(use_deps) {
      dependency_taskfn();
    } else {
      recursive_taskfn();
    }
  }

  auto t1 = now();
  std::cout << "# of tasks = " << task_counter.load() << std::endl;
  std::cout << "time elapsed (microseconds) = " << duration_in_mus(t0, t1) << std::endl;

  return 0;
}

