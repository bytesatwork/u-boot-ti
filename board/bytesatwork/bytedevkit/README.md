# Bytes at work byteENGINE AM625 on byteDEVKIT

This file describes how to build u-boot for the byteENGINE AM625 module mounted
on the byteDEVKIT.


## Install and get Dependencies

[Cross toolchain](https://software-dl.ti.com/processor-sdk-linux/esd/AM62X/09_01_00_08/exports/docs/linux/Overview/GCC_ToolChain.html#location-in-sdk)

[TI-linux-firmware](https://software-dl.ti.com/processor-sdk-linux/esd/AM62X/09_01_00_08/exports/docs/devices/AM62X/linux/Release_Specific_Release_Notes.html#ti-linux-firmware)

[TF-A](https://software-dl.ti.com/processor-sdk-linux/esd/AM62X/09_01_00_08/exports/docs/devices/AM62X/linux/Release_Specific_Release_Notes.html#tf-a)

[OP-TEE](https://software-dl.ti.com/processor-sdk-linux/esd/AM62X/09_01_00_08/exports/docs/devices/AM62X/linux/Release_Specific_Release_Notes.html#op-tee)

## Build TF-A

[TI TF-A build instructions](https://software-dl.ti.com/processor-sdk-linux/esd/AM62X/09_01_00_08/exports/docs/linux/Foundational_Components_ATF.html#arm-trusted-firmware-a)

## Build OP-TEE

[TI OP-TEE build instructions](https://software-dl.ti.com/processor-sdk-linux/esd/AM62X/09_01_00_08/exports/docs/linux/Foundational_Components_OPTEE.html#op-tee)

## Build u-boot

You should have downloaded `TI-linux-firmware` and built [TF-A](#build-tf-a), [OP-TEE OS](#build-op-tee) already.

[TI u-boot build instructions](https://software-dl.ti.com/processor-sdk-linux/esd/AM62X/09_01_00_08/exports/docs/linux/Foundational_Components/U-Boot/UG-General-Info.html#build-u-boot)

**IMPORTANT:** Use `am62x_bytedevkit_r5_defconfig` or `am62x_bytedevkit_a53_defconfig` instead of the TI defconfigs.

**NOTE:** Clean command: `make ARCH=arm CROSS_COMPILE=aarch64-linux-gnu- O=build-a53 distclean`

Copy the related files to SD card, see end of section
[TI u-boot build instructions](https://software-dl.ti.com/processor-sdk-linux/esd/AM62X/09_01_00_08/exports/docs/linux/Foundational_Components/U-Boot/UG-General-Info.html#build-u-boot)

## Prepare eMMC boot

Program the `tiboot3.bin`, `tispl.bin` and `u-boot.img` from the SD card to the
eMMC.

```log
=> run update_emmc
```

The bootloader needs to be stored in the boot0 hardware partition of the eMMC.
The layout of boot0 is defined so that it fits within 4 MiB, defined in blocks
of 512 Bytes:

```txt
                start   end     size    size
tiboot3.bin     0x0000  0x0400  0x0400   512 KiB
tispl.bin       0x0400  0x1000  0x0C00  1536 KiB
u-boot.img      0x1000  0x2000  0x1000  2048 KiB
```
