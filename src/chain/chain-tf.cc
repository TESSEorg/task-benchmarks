#include <taskflow/taskflow.hpp>  // Taskflow is header-only

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

int main(){
  
  tf::Executor executor;
  tf::Taskflow taskflow;

  auto t0 = now();

  tf::Task init = taskflow.emplace([](){++task_counter;}).name("init");
  tf::Task stop = taskflow.emplace([](){++task_counter;}).name("stop");

  int N = 10000000;

#ifdef USE_CONDITIONAL_TASK
  // creates a task that increments a counter until target value
  tf::Task next = taskflow.emplace(
    [&](){ if (task_counter < N) {++task_counter; return 0; } else return 1; }
  ).name("next");

  init.precede(next);
  // creates a feedback loop {0: increment, 1: stop}
  next.precede(next, stop);
#else
  tf::Task next = taskflow.emplace(
                              [&](){ ++task_counter; }
                              );
  init.precede(next);
  tf::Task prev = std::move(next);
  for(int t=1; t!=N; ++t) {
    tf::Task next = taskflow.emplace(
        [&](){ ++task_counter; }
    );
    prev.precede(next);
    prev = std::move(next);
  }
  prev.precede(stop);
#endif

  executor.run(taskflow).wait();
  auto t1 = now();
  std::cout << "# of tasks = " << task_counter.load() << std::endl;
  std::cout << "time elapsed (microseconds) = " << duration_in_mus(t0, t1) << std::endl;

  return 0;
}

