#!/bin/bash
cd ffmpeg

make clean

export NDK=/home/andy/workspace/tools/android-ndk-r14b
export PREBUILT=$NDK/toolchains/arm-linux-androideabi-4.9/prebuilt
export PLATFORM=$NDK/platforms/android-9/arch-arm
export PREFIX=../fflib
build_lib(){
  ./configure --disable-x86asm \
--prefix=$PREFIX \
--target-os=linux \
--arch=arm \
--sysroot=$PLATFORM \
--cc=$PREBUILT/linux-x86_64/bin/arm-linux-androideabi-gcc \
--nm=$PREBUILT/linux-x86_64/bin/arm-linux-androideabi-nm \
--cross-prefix=$PREBUILT/linux-x86_64/bin/arm-linux-androideabi- \
--enable-shared \
--enable-runtime-cpudetect \
--enable-cross-compile \
--enable-runtime-cpudetect \
--enable-gpl \
--enable-small \
--disable-static \
--disable-asm \
--disable-stripping \
--disable-ffprobe \
--disable-ffplay \
--disable-ffmpeg \
--disable-ffserver \
--disable-debug \
--disable-linux-perf \
--extra-cflags="-fPIC -DANDROID -D__thumb__ -mthumb -Wfatal-errors -Wno-deprecated -mfloat-abi=softfp -marm -march=armv7-a"
}

build_lib

make
make install

cd ..
