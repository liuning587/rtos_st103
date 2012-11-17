/**
 ******************************************************************************
 * @file       rtos_config.h
 * @brief   此文件存放与RTOS相关的配置
 *
 ******************************************************************************
 */
#ifndef __RTOS_CONFIG_H__
#define __RTOS_CONFIG_H__

#define SYS_EXC_PRI             (1u)    /**< 异常处理任务优先级 */
#define SYS_DAEMO_PRI           (2u)    /**< 守护任务优先级 */
#define SYS_SHELL_PRI           (6u)    /**< SHELL任务优先级 */
#define SYS_LOG_PRI             (  )    /**< 日志任务优先级 */

#define SYS_EXC_STACK_SIZE      (480u)  /**< 异常处理任务堆栈大小 */
#define SYS_DAEMO_STACK_SIZE    (  )    /**< 守护任务堆栈大小 */
#define SYS_TASK_STACK_SIZE     (1024u *2) /**< SHELL线程的堆栈大小 */
#define SYS_LOG_STACK_SIZE      (  )    /**< 日志任务优先级 */

#endif /* __RTOS_CONFIG_H__ */
