/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-12-04     tyx          first implementation
 * 2021-01-17     Meco Man     beautify cmd
 */
#include <stdlib.h>
#include "cpu_usage.h"

static volatile rt_thread_t tid;

static void load_thread(void *param)
{
	while (tid != RT_NULL && (int)tid->user_data)
	{
		while ((int)tid->user_data >= rt_tick_get() % 100);
		rt_thread_delay(1);
	}
	tid = RT_NULL;
}

void cpu_usage_msh(int argc, char *argv[])
{
	if (argc == 3 && rt_strcmp(argv[1], "-l") == 0)
	{
		if (tid)
		{
			tid->user_data = strtoul(argv[2], RT_NULL, 10); // 将参数nptr字符串根据参数base来转换成无符号的长整型数
		}
		else
		{
			tid = rt_thread_create("usage", load_thread, RT_NULL, 256,
					RT_THREAD_PRIORITY_MAX - 2, 20);
			if (tid == RT_NULL)
			{
				rt_kprintf("create thread failed.");
			}
			tid->user_data = strtoul(argv[2], RT_NULL, 10);
			rt_thread_startup(tid);
		}
		rt_kprintf("CPU usage adjusted to %d%c\n", tid->user_data, '%');
	}
	else if (argc == 1)
	{
		rt_kprintf("cpu: %d\n", (int)cpu_load_average());
	}
	else
	{
		rt_kprintf("%s\n", argv[0]);
	}
}
MSH_CMD_EXPORT_ALIAS(cpu_usage_msh, usage, cpu usage);
