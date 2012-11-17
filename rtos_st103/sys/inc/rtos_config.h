/**
 ******************************************************************************
 * @file       rtos_config.h
 * @brief   ���ļ������RTOS��ص�����
 *
 ******************************************************************************
 */
#ifndef __RTOS_CONFIG_H__
#define __RTOS_CONFIG_H__

#define SYS_EXC_PRI             (1u)    /**< �쳣�����������ȼ� */
#define SYS_DAEMO_PRI           (2u)    /**< �ػ��������ȼ� */
#define SYS_SHELL_PRI           (6u)    /**< SHELL�������ȼ� */
#define SYS_LOG_PRI             (  )    /**< ��־�������ȼ� */

#define SYS_EXC_STACK_SIZE      (480u)  /**< �쳣���������ջ��С */
#define SYS_DAEMO_STACK_SIZE    (  )    /**< �ػ������ջ��С */
#define SYS_TASK_STACK_SIZE     (1024u *2) /**< SHELL�̵߳Ķ�ջ��С */
#define SYS_LOG_STACK_SIZE      (  )    /**< ��־�������ȼ� */

#endif /* __RTOS_CONFIG_H__ */
