if (TARGET ttg-parsec)
  add_executable(chain-ttg "chain-ttg.cc")
  target_link_libraries(chain-ttg ttg-parsec)
endif (TARGET ttg-parsec)

if (TARGET Taskflow::Taskflow)
  add_executable(chain-tf "chain-tf.cc")
  target_link_libraries(chain-tf Taskflow::Taskflow)
endif (TARGET Taskflow::Taskflow)
