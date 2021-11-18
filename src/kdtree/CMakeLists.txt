if (TARGET ttg)
  include(AddTTGExecutable)
  add_ttg_executable(kdtree-ttg "kdtree-ttg.cc")
endif (TARGET ttg)

if (TARGET Taskflow::Taskflow)
  add_executable(kdtree-tf "kdtree-tf.cc")
  target_link_libraries(kdtree-tf Taskflow::Taskflow)
endif (TARGET Taskflow::Taskflow)

if (TARGET OpenMP::OpenMP_CXX)
  add_executable(kdtree-omp "kdtree-omp.cc")
  target_link_libraries(kdtree-omp PUBLIC OpenMP::OpenMP_CXX)
endif (TARGET OpenMP::OpenMP_CXX)
