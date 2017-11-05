#!/bin/bash

sudo apt install cmake -y

cd $HOME/Programas/

git clone --recursive git://github.com/schnorr/akypuera.git

cd akypuera

mkdir build

cd build

cmake -DOTF2=ON -DOTF_PATH="$HOME/Programas/scorep-3.0/build/bin/" ..

make

sudo make install