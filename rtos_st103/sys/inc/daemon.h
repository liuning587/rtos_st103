#ifndef __DAEMON_H__
#define __DAEMON_H__

extern int32_t daemon_reg(char_t name);
extern int32_t daemon_feed(uint32_t id);
extern void daemon_reboot(void);

#endif /* __DAEMON_H__ */
