#include "ttg.h"

#include "idx.h"

#include "../util/chrono.h"

std::atomic<int> task_counter = 0;

using namespace ttg;

auto make_ttg() {
  Edge<Idx, void> I2D, D2D;

  auto init = make_tt<void>([](std::tuple<Out<Idx, void>> &outs) { ++task_counter; sendk<0>(Idx{}, outs); }, edges(), edges(I2D));

  auto down = make_tt([](const Idx& idx, std::tuple<Out<Idx, void>> &outs) {
    ++task_counter;
    if (idx.l+1<LMAX) {
      ::sendk<0>(Idx(idx.l+1, {{idx.x[0]*2}}), outs);
      ::sendk<0>(Idx(idx.l+1, {{idx.x[0]*2+1}}), outs);
    }
  }, edges(fuse(I2D, D2D)), edges(D2D));

  return std::make_tuple(std::move(init), std::move(down));
}

int main(int argc, char* argv[]) {

  int nt = -1;
  if (argc > 1) {
    nt = std::atoi(argv[1]);
  }

  ttg_initialize(argc, argv, nt);

  auto [init, down] = make_ttg();

  auto connected = make_graph_executable(init.get());
  assert(connected);
  std::cout << "Graph is connected.\n";

  auto t0 = now();
  if (init->get_world().rank() == 0) init->invoke();

  ttg_execute(ttg_default_execution_context());
  ttg_fence(ttg_default_execution_context());
  auto t1 = now();

  std::cout << "# of tasks = " << task_counter.load() << std::endl;
  std::cout << "time elapsed (microseconds) = " << duration_in_mus(t0, t1) << std::endl;

  return 0;
}

