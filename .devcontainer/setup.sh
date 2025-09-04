#!/bin/bash
set -e

LLVM_TOOLCHAIN_VERSION=19

sudo mkdir -p /etc/apt/keyrings
sudo curl -fsSL https://apt.llvm.org/llvm-snapshot.gpg.key | sudo gpg --dearmor -o /etc/apt/keyrings/llvm-snapshot.gpg
sudo chmod a+r /etc/apt/keyrings/llvm-snapshot.gpg

CODENAME=$(. /etc/os-release && echo $VERSION_CODENAME)
ARCH=$(dpkg --print-architecture)

echo "deb [arch=$ARCH signed-by=/etc/apt/keyrings/llvm-snapshot.gpg] http://apt.llvm.org/$CODENAME/ llvm-toolchain-$CODENAME-$LLVM_TOOLCHAIN_VERSION main" | sudo tee /etc/apt/sources.list.d/llvm-snapshot.list
echo "deb-src [arch=$ARCH signed-by=/etc/apt/keyrings/llvm-snapshot.gpg] http://apt.llvm.org/$CODENAME/ llvm-toolchain-$CODENAME-$LLVM_TOOLCHAIN_VERSION main" | sudo tee -a /etc/apt/sources.list.d/llvm-snapshot.list

sudo apt update -y
sudo apt install -y clang-$LLVM_TOOLCHAIN_VERSION libc++-$LLVM_TOOLCHAIN_VERSION-dev libc++abi-$LLVM_TOOLCHAIN_VERSION-dev libclang-rt-$LLVM_TOOLCHAIN_VERSION-dev

sudo ln -sf /usr/bin/clang-${LLVM_TOOLCHAIN_VERSION} /usr/bin/cc
sudo ln -sf /usr/bin/clang++-${LLVM_TOOLCHAIN_VERSION} /usr/bin/c++

sudo update-alternatives --install /usr/bin/cc cc /usr/bin/clang-${LLVM_TOOLCHAIN_VERSION} 30
sudo update-alternatives --install /usr/bin/c++ c++ /usr/bin/clang++-${LLVM_TOOLCHAIN_VERSION} 30

sudo update-alternatives --auto cc
sudo update-alternatives --auto c++

sudo update-alternatives --display cc
sudo update-alternatives --display c++

echo "/usr/lib/llvm-${LLVM_TOOLCHAIN_VERSION}/lib" | sudo tee /etc/ld.so.conf.d/libcxx.conf > /dev/null
sudo ldconfig

export CXX=clang++-$LLVM_TOOLCHAIN_VERSION
export CC=clang-$LLVM_TOOLCHAIN_VERSION

ls -l /usr/bin/cc /usr/bin/c++
cc --version
c++ --version
