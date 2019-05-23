cmake_minimum_required(VERSION 3.14)
project(PoliDox)

set(CMAKE_CXX_STANDARD 17)

add_executable(PoliDox CRDTclient.cpp CLIENTcontroller.cpp Account.cpp  CRDT.cpp  CRDTserver.cpp EDITOR.cpp PoliDocs.cpp SERVERcontroller.cpp Char.cpp CRDTclient.h CLIENTcontroller.h Account.h CRDT.h CRDTserver.h EDITOR.h SERVERcontroller.h Char.h)