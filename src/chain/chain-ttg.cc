#include "ttg.h"

#include <chrono>

using namespace ttg;

using time_point = std::chrono::high_resolution_clock::time_point;

inline time_point now() { return std::chrono::high_resolution_clock::now(); }

inline std::chrono::system_clock::time_point system_now() {
  return std::chrono::system_clock::now();
}

inline int64_t duration_in_mus(time_point const &t0, time_point const &t1) {
  return std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
}

std::atomic<int> task_counter = 0;

template <bool flow_taskid_as_values>
auto make_ttg();

// flows task ids via values
template <>
auto make_ttg<true>() {
  Edge<int, int> I2N, N2N;
  Edge<void, int> N2S;

  auto init = make_tt<void>([](std::tuple<Out<int, int>> &outs) { ++task_counter; send<0>(0, 0, outs); }, edges(), edges(I2N));

  auto next = make_tt<int>([](const int &key, const int&, std::tuple<Out<int, int>, Out<void, int>> &outs) {
    ++task_counter;
    if (key < 10000000) {
      send<0>(key+1, 0, outs);
    }
    else {
      sendv<1>(0, outs);
    }
  } , edges(fuse(I2N, N2N)), edges(N2N, N2S));

  auto stop = make_tt<void>([](const int &, std::tuple<> &outs) { ++task_counter; }, edges(N2S), edges());

  auto rank = ttg_default_execution_context().rank();
  init->set_keymap([rank](void){ return rank; });
  next->set_keymap([rank](const int& key){ return rank; });
  stop->set_keymap([rank](void){ return rank; });

  return std::make_tuple(std::move(init), std::move(next), std::move(stop));
}

// flows task ids via keys
template <>
auto make_ttg<false>() {
  Edge<int, void> I2N, N2N;
  Edge<void, int> N2S;

  auto rank = ttg_default_execution_context().rank();

  auto init = make_tt<void>([](std::tuple<Out<int, void>> &outs) { sendk<0>(0, outs); }, edges(), edges(I2N));

  auto next = make_tt([](const int& key, std::tuple<Out<int, void>, Out<void, int>> &outs) {
    if (key < 10000000) {
      ::sendk<0>(key+1, outs);
    }
    else {
      ::sendv<1>(key, outs);
    }
  }, edges(fuse(I2N, N2N)), edges(N2N, N2S));

  auto stop = make_tt<void>([](const int &v, std::tuple<> &outs) { std::cout << "last task received v=" << v << std::endl; }, edges(N2S), edges());

  init->set_keymap([rank](void){ return rank; });
  next->set_keymap([rank](const int& key){ return rank; });
  stop->set_keymap([rank](void){ return rank; });

  return std::make_tuple(std::move(init), std::move(next), std::move(stop));
}

int main(int argc, char* argv[]) {

  ttg_initialize(argc, argv, -1);

  // change to true to flow task ids as values, this stresses the cost of managing data copies
  // default (false) uses task ids as usual (as "keys"), this avoids the need for data copy management
  constexpr bool flow_taskid_as_values = false;
  auto [init, next, stop] = make_ttg<flow_taskid_as_values>();

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

