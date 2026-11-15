#ifndef BUTTON_HANDLER_H
#define BUTTON_HANDLER_H

enum ButtonPress { BTN_NONE, BTN_UP_P, BTN_DOWN_P, BTN_SEL_P, BTN_BACK_P };

void        button_init();
ButtonPress button_read();

#endif
