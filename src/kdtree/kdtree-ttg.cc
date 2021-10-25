#include "ttg.h"

#include <chrono>

#include "idx.h"

std::atomic<int> task_counter = 0;

using namespace ttg;

using time_point = std::chrono::high_resolution_clock::time_point;

inline time_point now() { return std::chrono::high_resolution_clock::now(); }

inline std::chrono::system_clock::time_point system_now() {
  return std::chrono::system_clock::now();
}

inline int64_t duration_in_mus(time_point const &t0, time_point const &t1) {
  return std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
}

auto make_ttg() {
  Edge<Idx, void> I2D, D2D;

  auto rank = ttg_default_execution_context().rank();

  auto init = make_tt<void>([](std::tuple<Out<Idx, void>> &outs) { ++task_counter; sendk<0>(Idx{}, outs); }, edges(), edges(I2D));

  auto down = make_tt([](const Idx& idx, std::tuple<Out<Idx, void>> &outs) {
    ++task_counter;
    if (idx.l+1<LMAX) {
      ::sendk<0>(Idx(idx.l+1, {{idx.x[0]*2}}), outs);
      ::sendk<0>(Idx(idx.l+1, {{idx.x[0]*2+1}}), outs);
    }
  }, edges(fuse(I2D, D2D)), edges(D2D));

  init->set_keymap([rank](){ return rank; });
  down->set_keymap([rank](const Idx& idx){ return rank; });

  return std::make_tuple(std::move(init), std::move(down));
}

int main(int argc, char* argv[]) {

  ttg_initialize(argc, argv, -1);

  // change to true to flow task ids as values, this stresses the cost of managing data copies
  // default (false) uses task ids as usual (as "keys"), this avoids the need for data copy management
  constexpr bool flow_taskid_as_values = false;
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

