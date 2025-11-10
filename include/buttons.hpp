#ifndef BUTTONS_HPP
#define BUTTONS_HPP

#define BTN_NEXT  16   // button to go forward
#define BTN_PREV  17   // button to go backward

extern int currentScreen;

void buttonSetup();
void handleButtons();

#endif // BUTTONS_HPP