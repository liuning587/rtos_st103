#if 0
#include <types.h>
#include <sched.h>
#include <shell.h>
#include "spi_sd.h"

uint32_t do_sd_test(cmd_tbl_t * cmdtp, uint32_t argc, const uint8_t *argv[])
{
    SD_CardInfo cardinfo;
    SD_Error ret = SD_Init();
    if (ret != SD_RESPONSE_NO_ERROR)
    {
        printf("SD_Init err! ret:%d\n", ret);
        return -1;
    }
    printf("SD_Init OK!\n");

    ret = SD_GetCardInfo(&cardinfo);
    if (ret != SD_RESPONSE_NO_ERROR)
    {
        printf("SD_GetCardInfo err! ret:%d\n", ret);
        return -1;
    }
    printf("SD_GetCardInfo OK!\n");
    printf("CardCapacity:%d\n", cardinfo.CardCapacity);
    printf("CardBlockSize:%d\n", cardinfo.CardBlockSize);

    return 0;
}
SHELL_CMD(
    sdtest, CFG_MAXARGS,        do_sd_test,
    "sdtest \r\t\t\t\t do_sd_test \r\n"
);
#endif
