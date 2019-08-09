// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2025 bytesatwork AG - https://www.bytesatwork.io
 */

#include <command.h>
#include <cpu.h>
#include <dm.h>
#include <init.h>

static int do_cpuinfo(struct cmd_tbl *cmdtp, int flag, int argc, char * const argv[])
{
#if IS_ENABLED(CONFIG_DM) && IS_ENABLED(CONFIG_CPU)
	struct udevice *dev;
	char buf[256];
	int ret;

	ret = uclass_get_device(UCLASS_CPU, 0, &dev);
	if (ret) {
		printf("Failed to get CPU device: %d\n", ret);
		return ret;
	}

	ret = cpu_get_desc(dev, buf, sizeof(buf));
	if (ret) {
		printf("Failed to get CPU description: %d\n", ret);
		return ret;
	}

	puts(buf);
	return 0;
#else
	return print_cpuinfo();
#endif
}

U_BOOT_CMD(cpuinfo, 1, 1, do_cpuinfo, "show cpu info", "");
