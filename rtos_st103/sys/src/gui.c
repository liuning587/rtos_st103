#include <types.h>
#include <gui.h>

status_t
gui_init(void)
{
    lcd_init();
    lcd_logo();
    return OK;
}
