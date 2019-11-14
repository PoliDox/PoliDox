# Welcome to PoliDox
PoliDox is a cooperative text editor written in C++ with the help of Qt. It was developed at the Politecnico di Torino (Italy) as a project of the 2019 System Programming course.

It consists in a client-server application, which uses by default the port 5678 of the localhost. If you wish, you can change these values modifying accordingly the macros SERVER\_IP and PORT\_NUMBER in the files client/Client.h as well as server/Server.h
Internally it is makes use of the Crdt algorithm, which is described in the website of [Conclave](https://conclave-team.github.io/conclave-site/#what-is-a-real-time-collaborative-text-editor), a similar cooperative text editor written in JavaScript.

## Requirements
- Qt 
- Mongo db: Fede vi racconterà presto come installarlo. Non perdetevelo mi raccomando

## Big paragraph

### Smaller paragraph

[here is a link](https://www.dinamobasket.com)

and here is a bullet list:
- a
- b
- c
