
#ifndef _EXC_H_
#define _EXC_H_
#include <types.h>

/*------------------------------------------------------------------------------
Section: Includes
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
Section: Macro Definitions
------------------------------------------------------------------------------*/
#define MAX_RECORD      10
#define EXCREC_OFFSET   0x2001

/*------------------------------------------------------------------------------
Section: Type Definitions
------------------------------------------------------------------------------*/
typedef struct {
    uint32_t index;
} REC_INFO;


typedef struct {
    uint32_t	valid;		/* indicators that following fields are valid */
    uint32_t	vecAddr;	/* exception vector address */
    uint32_t    fsr;		/* fault status register*/
    uint32_t    faddr;      /* fault address register */
	uint32_t	sp;
	uint32_t	taskid;
    uint32_t	regs[16];
    time_t      time;       /* occur time */
} EXC_INFO;

typedef struct {
    uint32_t  vecAddr;    /* exception vector */
    char_t *  excMsg;     /* exception message */
} EXC_MSG_TBL;


/*------------------------------------------------------------------------------
Section: Globals
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
Section: Function Prototypes
------------------------------------------------------------------------------*/
void excInfoShow(EXC_INFO info) ;
extern status_t excInit(void);


#ifdef __cplusplus      /* Maintain C++ compatibility */
}
#endif /* __cplusplus */

#endif /*_EXC_H_ */

/*-------------------------------End of exc.h-------------------------------*/


