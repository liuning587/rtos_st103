/*------------------------------------------------------------------------------ 
Section: Includes 
------------------------------------------------------------------------------*/ 
#include "types.h"
#include "stdio.h"
#include "shell.h"
#include "sched.h"
#include "string.h"

#if 0
/*------------------------------------------------------------------------------ 
Section: Macro Definitions 
------------------------------------------------------------------------------*/ 
/* NONE */ 
 
/*------------------------------------------------------------------------------ 
Section: Private Type Definitions 
------------------------------------------------------------------------------*/ 
/* NONE */ 
 
/*------------------------------------------------------------------------------ 
Section: Global Variables 
------------------------------------------------------------------------------*/ 
/* NONE */ 
 
/*------------------------------------------------------------------------------ 
Section: Private Variables 
------------------------------------------------------------------------------*/ 
static SEM_ID semExcTst;
static uint8_t excTestStack[512];
static int testid;
static unsigned int * pTst;
FUNCPTR     _func_excTest;
/*------------------------------------------------------------------------------ 
Section: Private Function Prototypes 
------------------------------------------------------------------------------*/ 
void testStackOver()
{
    uint8_t a[1024];
    memset(a,0xff,1024);
    printf("test stack flow over:%d",a[1023]);
}

void taskloop()
{
    while(1){
        semTake(semExcTst, WAIT_FOREVER);
        switch (testid){
            case 0 : /* usage fault */
                _func_excTest(0);
                break;
            case 1 : /* bus fault */
                pTst = (unsigned int *)0xD00ED000;
                printf("pTst value: %08x \r\n",*pTst);
                break;
            case 2 :

                testStackOver();
#if 0
              __asm(
                   "MRS R0  MSP"
                   "LDR R1  =0x01"
                   "MSR R1  FAULTMASK  "
                    );
#endif
              break;
           default:
                printf(".\r\n");
            }
        
    }

}

void createTask()
{
    semExcTst = semBCreate(0);

    taskSpawn((uint8_t*)"tExcTest",9,excTestStack,512,(OSFUNCPTR)taskloop,0);
    
}


void exctest(int id)
{
    testid = id;
    semGive(semExcTst);
}
/*------------------------------------------------------------------------------ 
Section: Public Function  
------------------------------------------------------------------------------*/ 

uint32_t do_excTest(cmd_tbl_t * cmdtp, uint32_t argc, const uint8_t *argv[])
{   
    int id;

    if (argc != 2) {
        printf ("Usage:\r\n");
        printf ("%s\r\n", cmdtp->usage);
        return 1;
    }

    sscanf((int8_t *)argv[1], "%d", &id);


    if (id == -1)
        createTask();
    else
        exctest(id);

    return 0;
}
SHELL_CMD(
	et,	CFG_MAXARGS,		do_excTest,
 	"et \r\t id(-1,0,1,2) \r\t\t\t\t Test exception handler \r\n"
);

 
#endif
 
/*------------------------------End of excTest.c---------------------------------*/ 

