#!/usr/bin/env bash

git clone https://github.com/Kure2323/Todo.git
cd Todo
cmake -S . -B build
cmake --build build
sudo cmake --install build
