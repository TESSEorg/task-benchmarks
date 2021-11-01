#include <omp.h>
#include <iostream>
#include <atomic>
#include <chrono>

#define USE_CONDITIONAL_TASK

using time_point = std::chrono::high_resolution_clock::time_point;

inline time_point now() { return std::chrono::high_resolution_clock::now(); }

inline std::chrono::system_clock::time_point system_now() {
  return std::chrono::system_clock::now();
}

inline int64_t duration_in_mus(time_point const &t0, time_point const &t1) {
  return std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
}

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

