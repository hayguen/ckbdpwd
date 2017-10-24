cmake_minimum_required(VERSION 2.8)

set (CMAKE_INSTALL_PREFIX /usr/local)
message("CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX} (should be /usr/local)")

project(ckbdpwd)

add_executable(ckbdpwd "ckbdpwd.c")

install(TARGETS ckbdpwd DESTINATION bin )
