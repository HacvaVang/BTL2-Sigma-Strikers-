#ifndef MENU_H
#define MENU_H

#include "SDLFramework.h"

// Show a simple resolution selection menu using the existing SDL renderer.
// This will call `setResolution` on the provided app when a resolution is chosen.
// The function returns true if a resolution was applied, false if the menu was cancelled.
bool showResolutionMenu(SDLFramework &app);

// Return value for main menu selections.  The caller can loop using this
// result to display the menu repeatedly until "Play" or "Quit" is chosen.
enum MainMenuChoice {
    MENU_PLAY,
    MENU_TUTORIAL,
    MENU_SETTINGS,
    MENU_QUIT
};

// Draw a simple main menu with three items (play/tutorial/settings) plus quit.
// Uses arrow keys/enter for navigation.  Returns the selected choice.
MainMenuChoice showMainMenu(SDLFramework &app);

// Show a basic tutorial screen with instructions.
// Displays an informational text and waits for any key press or mouse click
// before returning to the caller.
void showTutorial(SDLFramework &app);

#endif // MENU_H
