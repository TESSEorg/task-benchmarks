#include <taskflow/taskflow.hpp>  // Taskflow is header-only

#include "idx.h"

#include "../util/chrono.h"

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

int main(int argc, char *argv[]){

  int nt = std::thread::hardware_concurrency();
  if (argc > 1) {
    nt = std::atoi(argv[1]);
  }

  tf::Executor executor(nt);
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

