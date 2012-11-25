#include <types.h>
#include <sched.h>
#include <shell.h>
#include <ff.h>

#define die(rc) {printf("Failed with rc=%u.\n", rc);return -1;}

uint32_t do_fat_test(cmd_tbl_t * cmdtp, uint32_t argc, const uint8_t *argv[])
{
    FATFS Fatfs;        /* File system object */
    FIL Fil;            /* File object */
    BYTE Buff[128];     /* File read buffer */

    FRESULT rc;             /* Result code */
    DIR dir;                /* Directory object */
    FILINFO fno;            /* File information object */
    UINT bw, br, i;


    f_mount(0, &Fatfs);     /* Register volume work area (never fails) */

    printf("\nCreate a new file (hello.txt).\n");
    rc = f_open(&Fil, "HELLO.TXT", FA_WRITE | FA_CREATE_ALWAYS);
    if (rc) die(rc);

    printf("\nWrite a text data. (Hello world!)\n");
    rc = f_write(&Fil, "Hello world!\r\n", 14, &bw);
    if (rc) die(rc);
    printf("%u bytes written.\n", bw);

    printf("\nClose the file.\n");
    rc = f_close(&Fil);
    if (rc) die(rc);

    printf("\nOpen an existing file (hello.txt).\n");
    rc = f_open(&Fil, "HELLO.TXT", FA_READ);
    if (rc) die(rc);

    printf("\nType the file content.\n");
    for (;;) {
        rc = f_read(&Fil, Buff, sizeof Buff, &br);  /* Read a chunk of file */
        if (rc || !br) break;           /* Error or end of file */
        for (i = 0; i < br; i++)        /* Type the data */
            printf("%c", Buff[i]);
    }
    if (rc) die(rc);

    printf("\nClose the file.\n");
    rc = f_close(&Fil);
    if (rc) die(rc);

    printf("\nOpen root directory.\n");
    rc = f_opendir(&dir, "");
    if (rc) die(rc);

    printf("\nDirectory listing...\n");
    for (;;) {
        rc = f_readdir(&dir, &fno);     /* Read a directory item */
        if (rc || !fno.fname[0]) break; /* Error or end of dir */
        if (fno.fattrib & AM_DIR)
            printf("   <dir>  %s\n", fno.fname);
        else
            printf("%8lu  %s\n", fno.fsize, fno.fname);
    }
    if (rc) die(rc);

    f_mount(0, &Fatfs);     /* unmount */
    printf("\nTest completed.\n");

    return 0;
}
SHELL_CMD(
    fattest, CFG_MAXARGS,        do_fat_test,
    "fattest \r\t\t\t\t do_fat_test \r\n"
);


