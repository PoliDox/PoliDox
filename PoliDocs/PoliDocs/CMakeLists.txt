cmake_minimum_required(VERSION 3.14)
project(PoliDox)

set(CMAKE_CXX_STANDARD 14)

add_executable(Symbol.cpp Account.cpp CLIENTcontroller.cpp CRDT.cpp CRDTclient.cpp CRDTserver.cpp EDITOR.cpp PoliDocs.cpp SERVERcontroller.cpp Account.h CLIENTcontroller.h CRDT.h CRDTclient.h CRDTserver.h EDITOR.h SERVERcontroller.h Symbol.h)