#ifndef MENU_H
#define MENU_H

#include "SDLFramework.h"

// Show a simple resolution selection menu using the existing SDL renderer.
bool showResolutionMenu(SDLFramework &app);

// Return value for main menu selections.
enum MainMenuChoice {
    MENU_PLAY,
    MENU_PVP,
    MENU_TUTORIAL,
    MENU_SETTINGS,
    MENU_QUIT
};

// Draw a simple main menu with items: Play (vs AI), PvP, Tutorial, Settings, Quit.
MainMenuChoice showMainMenu(SDLFramework &app);

// Show a basic tutorial screen with instructions.
void showTutorial(SDLFramework &app);

// Settings menu with resolution and game duration options.
// Returns the selected match duration in seconds (60, 120, 180, or 300).
struct GameSettings {
    int matchDuration; // seconds
};

// Global settings
extern GameSettings gSettings;

// Show settings menu (resolution + match duration)
void showSettingsMenu(SDLFramework &app);

#endif // MENU_H
