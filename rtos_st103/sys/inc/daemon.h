#ifndef __DAEMON_H__
#define __DAEMON_H__

extern uint32_t regist_to_daemon(char_t *name);
extern uint32_t feed_dog(uint32_t task_fd);
extern void daemon_init(void);

#endif /* __DAEMON_H__ */
