# Bytes at work byteENGINE AM625 on byteDEVKIT

This file describes how to build u-boot for the byteENGINE AM625 module mounted
on the byteDEVKIT.

## Install Dependencies

Cross toolchain:

```bash
sudo apt install gcc-aarch64-linux-gnu gcc-arm-linux-gnueabihf
```

## ti-linux-firmware

### Download TI Firmware

```bash
git clone https://git.ti.com/git/processor-firmware/ti-linux-firmware.git
cd ti-linux-firmware
git checkout 08.04.01.005
```

## arm-trusted-firmware

### Download TF-A

```bash
git clone https://git.ti.com/git/atf/arm-trusted-firmware.git
cd arm-trusted-firmware
git checkout 08.04.00.002
```

### Build TF-A

```bash
make CROSS_COMPILE=aarch64-linux-gnu- PLAT=k3 TARGET_BOARD=lite SPD=opteed all
```

## optee-os

### Download OP-TEE Trusted OS

```bash
git clone https://git.ti.com/git/optee/ti-optee-os.git
cd ti-optee-os
git checkout 08.04.00.002
```

### Build OP-TEE OS

```bash
make ARCH=arm CROSS_COMPILE64=aarch64-linux-gnu- PLATFORM=k3-j721e CFG_ARM64_core=y all
```

**NOTE:** On fail check for missing python dependencies, e.g. `python3-pycryptodome`.

## u-boot

### Download bytes at work u-boot

```bash
git clone https://github.com/bytesatwork/u-boot-ti.git ti-u-boot
cd ti-u-boot
git checkout baw-ti-u-boot-2021.01
```

### Build u-boot for R5

```bash
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- am62x_bytedevkit_r5_defconfig O=build-r5
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- O=build-r5 -j`nproc`
```

**NOTE:** Clean command: `make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- O=build-r5 distclean`

### Build u-boot for A53

You should have downloaded [TI Firmware](#download-ti-firmware) and built
[TF-A](#build-tf-a), [optee-os](#build-op-tee-os) already.
Commands imply [folder structure](#folder-structure).

```bash
make ARCH=arm CROSS_COMPILE=aarch64-linux-gnu- am62x_bytedevkit_a53_defconfig O=build-a53
make ARCH=arm CROSS_COMPILE=aarch64-linux-gnu- \
ATF=`pwd`/../arm-trusted-firmware/build/k3/lite/release/bl31.bin \
TEE=`pwd`/../ti-optee-os/out/arm-plat-k3/core/tee-pager_v2.bin \
DM=`pwd`/../ti-linux-firmware/ti-dm/am62xx/ipc_echo_testb_mcu1_0_release_strip.xer5f \
O=build-a53 -j`nproc`
```

**NOTE:** Clean command: `make ARCH=arm CROSS_COMPILE=aarch64-linux-gnu- O=build-a53 distclean`

## k3-image-gen

### Download System Firmware Image Generator

```bash
git clone https://git.ti.com/git/k3-image-gen/k3-image-gen.git
cd k3-image-gen
git checkout 08.04.01.005
```

### Build tiboot3.bin

You should have downloaded [TI Firmware](#download-ti-firmware) and built
[u-boot for R5](#build-u-boot-for-r5) already.
Command imply [folder structure](#folder-structure).

```bash
make ARCH=arm CROSS_COMPILE=aarch64-linux-gnu- \
SYSFW_PATH=`pwd`/../ti-linux-firmware/ti-sysfw/ti-fs-firmware-am62x-gp.bin \
SOC=am62x SBL=`pwd`/../ti-u-boot/build-r5/spl/u-boot-spl.bin
```

**NOTE:** Clean command: `make ARCH=arm CROSS_COMPILE=aarch64-linux-gnu- clean`

## Result

### Copy Files to SD

```bash
cp -vp k3-image-gen/tiboot3.bin <path-to-SD-boot-partition>/
cp -vp ti-u-boot/build-a53/tispl.bin <path-to-SD-boot-partition>/
cp -vp ti-u-boot/build-a53/u-boot.img <path-to-SD-boot-partition>/
```

### Folder Structure

```txt
.
├── arm-trusted-firmware
├── k3-image-gen
├── ti-linux-firmware
├── ti-optee-os
└── ti-u-boot
```

## Prepare SPI boot

Program the `tiboot3.bin`, `tispl.bin` and `u-boot.img` from the SD card to the
SPI flash.

```log
=> sf probe
=> fatload mmc 1 ${loadaddr} tiboot3.bin
=> sf update $loadaddr 0x0 $filesize
=> fatload mmc 1 ${loadaddr} tispl.bin
=> sf update $loadaddr 0x80000 $filesize
=> fatload mmc 1 ${loadaddr} u-boot.img
=> sf update $loadaddr 0x280000 $filesize
```

**NOTE:** Force SPI boot with the BOOT MODE DIP switches on the byteDEVKIT.
