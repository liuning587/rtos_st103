/*
 * devlibtest.c
 *
 *  Created on: 2012-11-17
 *      Author: daixuyi
 */
#include <stdio.h>
#include <devlib.h>
#include <shell.h>
#if 1

static status_t test_init(struct fileopt* dev)
{
	printf("test init\n");
	return OK;
}

static int32_t test_open(struct fileopt* dev, uint16_t flag)
{
	printf("test open, flag:%d\n", flag);
	return OK;
}

static status_t test_relesae(struct fileopt* dev)
{
	printf("test relesae\n");
	return OK;
}
const static fileopt_t the_testopt =
{
		.init = test_init,
		.release = test_relesae,
		.open = test_open,
};

void
test_devlib(void)
{
	int32_t fd = -1;
	devlib_init();
	if (OK != dev_create("test", &the_testopt))
	{
		printf("dev_create err\n");
		return;
	}

	fd = dev_open("test", 0);
	if (fd <= 0)
	{
		printf("open test err\n");
		return;
	}
	//dev_read
	//dev_write
	dev_close(fd);
	dev_release("test");
}

uint32_t do_devtest(cmd_tbl_t * cmdtp, uint32_t argc, const uint8_t *argv[])
{
	test_devlib();
    return 0;
}
SHELL_CMD(
	devlib,	CFG_MAXARGS,		do_devtest,
 	"devlib \r\t\t\t\t do_devtest \r\n"
);
#endif
