if (TARGET ttg)
  include(AddTTGExecutable)
  add_ttg_executable(chain-ttg "chain-ttg.cc")
endif (TARGET ttg)

if (TARGET Taskflow::Taskflow)
  add_executable(chain-tf "chain-tf.cc")
  target_link_libraries(chain-tf Taskflow::Taskflow)
endif (TARGET Taskflow::Taskflow)
