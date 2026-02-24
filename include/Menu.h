#ifndef MENU_H
#define MENU_H

#include "SDLFramework.h"

// Show a simple resolution selection menu using the existing SDL renderer.
// This will call `setResolution` on the provided app when a resolution is chosen.
// The function returns true if a resolution was applied, false if the menu was cancelled.
bool showResolutionMenu(SDLFramework &app);

#endif // MENU_H
