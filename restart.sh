#/bin/bash
PASSWORD='1'
cd /home/wang/NBUT_HERO_AIM/
echo $PASSWORD | sudo rm -r build
mkdir build
cd build
cmake ..
make
