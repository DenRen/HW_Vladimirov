CLBROD
===

## Install SFML

```sh
sudo apt update
sudo apt upgrade -y
sudo apt install -y \
    make cmake g++ build-essential git \
    libglu1-mesa-dev freeglut3-dev libxcursor-dev \
    libfreetype6-dev \
    libudev-dev libopenal-dev libvorbis-dev libflac-dev libxrandr-dev
mkdir build && cd build
cmake ..
make -j $(nproc) install
```