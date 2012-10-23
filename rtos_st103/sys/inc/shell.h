/**
 ******************************************************************************
 * @file       shell.h
 * @version    V0.0.1
 * @brief      shellģ��.
 * @details    This file including all API functions's implement of shell.
 * @copy       Copyrigth(C)
 *
 ******************************************************************************
 */
#ifndef _SHELL_H_
#define _SHELL_H_


/*-----------------------------------------------------------------------------
 Section: Includes
 ----------------------------------------------------------------------------*/
#include <types.h>

/*-----------------------------------------------------------------------------
 Section: Macro Definitions
 ----------------------------------------------------------------------------*/
#define INCLUDE_SHELL
#define CFG_MAXARGS 8

/*-----------------------------------------------------------------------------
 Section: Type Definitions
 ----------------------------------------------------------------------------*/
/**
 * Monitor Command Table
 */
struct cmd_tbl_s
{
	char_t      *name;		/**< Command Name			        */
	uint32_t    maxargs;	/**< maximum number of arguments	*/
	uint32_t    (*cmd)(struct cmd_tbl_s *, uint32_t, const uint8_t *[]);
	char_t      *usage;		/**< Usage message	(short)	    */
};
typedef struct cmd_tbl_s	cmd_tbl_t;

#define Struct_Section  __attribute__ ((unused,section (".shell_cmd")))

#ifdef INCLUDE_SHELL
#define SHELL_CMD(name, maxargs, cmd, usage) \
const cmd_tbl_t __shell_cmd_##name Struct_Section = {#name, maxargs, cmd, usage}
#else
#define SHELL_CMD(name,maxargs,cmd,usage) 
#endif
/*-----------------------------------------------------------------------------
 Section: Function Prototypes
 ----------------------------------------------------------------------------*/
extern void shell_init(void);

#endif /*_SHELL_H_ */

/*------------------------------End of shell.h-------------------------------*/

