#!/bin/bash

sudo apt install libopenmpi-dev -y

sudo apt install simgrid -y

sudo apt install lam-runtime -y

sudo apt install mpich -y

sudo apt install openmpi-bin -y	

sudo apt install papi-tools -y

cd $HOME/Downloads/

wget http://pkgs.fedoraproject.org/repo/pkgs/libunwind/libunwind-1.1.tar.gz/fb4ea2f6fbbe45bf032cd36e586883ce/libunwind-1.1.tar.gz 

tar vxf libunwind-1.1.tar.gz

mv libunwind-1.1 ../Programas/

cd $HOME/Programas/libunwind-1.1/

./configure

make 

sudo make install

cd $HOME/Downloads/

wget http://www.vi-hps.org/upload/packages/scorep/scorep-3.0.tar.gz

tar vxf scorep-3.0.tar.gz

mv scorep-3.0 $HOME/Programas/

cd $HOME/Programas/scorep-3.0/

programsPath="$(pwd)"

./configure --prefix=$(pwd)/build --without-gui --with-opari2=/opt/opari2/bin/ \
 --with-mpi=openmpi --without-shmen --enable-papi --with-papi-lib=/usr/local/lib/ \
 --with-papi-header=/usr/local/include/ --with-libunwind=$programsPath/libunwind-1.1/ \
 --with-libunwind-include=$programsPath/libunwind-1.1/include/ \
 --with-libunwind-lib=$programsPath/libunwind-1.1/ader=/usr/local/include/ \
 --with-libunwind=$programsPath/libunwind-1.1/ \
 --with-libunwind-include=$programsPath/libunwind-1.1/

make -j3

sudo make install

sed -i '1iexport PATH='"$programsPath"'/build/bin:$PATH' ~/.bashrc

sed -i '1iexport LD_LIBRARY_PATH='"$programsPath"'/build/lib:$LD_LIBRARY_PATH'  ~/.bashrc

source ~/.bashrc