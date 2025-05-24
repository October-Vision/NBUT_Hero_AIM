#/bin/bash
PASSWORD='nuc'
cd /home/nuc/NBUT_HERO_AIM/
echo $PASSWORD | sudo rm -r build
mkdir build
cd build
cmake ..
make
