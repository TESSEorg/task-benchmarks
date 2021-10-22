#include <taskflow/taskflow.hpp>  // Taskflow is header-only

#include "idx.h"

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

struct NodeOp {
  Idx idx;

  NodeOp(const Idx& idx) : idx(idx){}

  void operator()(tf::Subflow& subflow) {
    ++task_counter;
    if (idx.l+1<LMAX) {
      subflow.emplace(NodeOp(Idx(idx.l + 1, {{idx.x[0] * 2}})));
      subflow.emplace(NodeOp(Idx(idx.l + 1, {{idx.x[0] * 2 + 1}})));
    }
  }
};

int main(){
  
  tf::Executor executor;
  tf::Taskflow taskflow;

  auto t0 = now();

  tf::Task init = taskflow.emplace([](){ ++task_counter; });

  tf::Task down = taskflow.emplace(NodeOp(Idx{}));
  init.precede(down);

  executor.run(taskflow).wait();
  auto t1 = now();
  std::cout << "# of tasks = " << task_counter.load() << std::endl;
  std::cout << "time elapsed (microseconds) = " << duration_in_mus(t0, t1) << std::endl;

  return 0;
}

