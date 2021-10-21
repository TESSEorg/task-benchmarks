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

int main(int argc, char* argv[]) {

  ttg_initialize(argc, argv, -1);

  Edge<int, int> I2N, N2N;
  Edge<void, int> N2S;

  auto init = make_tt<void>([](std::tuple<Out<int, int>> &outs) { send<0>(0, 0, outs); }, edges(), edges(I2N));

  auto next = make_tt<int>([](const int &key, const int&, std::tuple<Out<int, int>, Out<void, int>> &outs) {
    if (key < 10000000) {
      send<0>(key+1, 0, outs);
    }
    else {
      sendv<1>(0, outs);
    }
  } , edges(fuse(I2N, N2N)), edges(N2N, N2S));

  auto stop = make_tt<void>([](const int &, std::tuple<> &outs) {}, edges(N2S), edges());

  auto connected = make_graph_executable(init.get());
  assert(connected);
  std::cout << "Graph is connected.\n";

  auto t0 = now();
  if (init->get_world().rank() == 0) init->invoke();

  ttg_execute(ttg_default_execution_context());
  ttg_fence(ttg_default_execution_context());
  auto t1 = now();

  std::cout << "time elapsed (microseconds) = " << duration_in_mus(t0, t1) << std::endl;

  return 0;
}

