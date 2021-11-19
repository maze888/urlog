add_executable (simple simple.cpp)
#add_executable (simple2 simple2.cpp)

target_link_libraries (simple urlog fmt liburing.a)
#target_link_libraries (simple2 urlog)
