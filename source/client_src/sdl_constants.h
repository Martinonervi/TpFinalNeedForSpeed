#ifndef SDL_CONSTANTS_H
#define SDL_CONSTANTS_H

#define NOT_ACCESSIBLE (-1)
#define LOOP (-1)
#define DEFAULT 0
#define MAX_UPGRADE_FRAMES 3
#define MAX_SPEED 80.0f
#define MIN_SPEED 0.0f

#define MAX_NEEDLE_ANGLE 45.0f
#define MIN_NEEDLE_ANGLE (-225.0f)

// Sizes:

#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 800

#define UPS_WIDTH 480
#define UPS_HEIGHT 480
#define MINIMAP_SIZE 250
#define START_BTN_WIDTH 300
#define START_BTN_HEIGHT 80
#define SIZE_OF_DIAL 150
#define DIAL_MARGIN 40
#define HUD_MARGIN 10

#define VOLUME_AMOUNT 10

// Time:

#define HOURS_TO_SECS 3600
#define MINS_TO_SECS 60

// Texts:

#define BTN_TEXT "Start Game"
#define BACK_SLASH "/"
#define RACE_TXT "Race "
#define CHECK_TXT "Checkpoint "
#define NO_TEXT ""
#define SRV_DISC_TXT "Server disconnection"
#define TIME_PENALTY "Time penalty: + %.2f"
#define NEXT_ROUTE_TXT "Next Race Route"
#define UP_SHOP_TXT "Upgrade Shop"
#define RANKING_TXT "Position "

// Colors:

inline SDL_Color BTN_GREEN = {0, 208, 22, 255};

inline SDL_Color BTN_GREEN_HOVER = {0, 255, 51, 255};

inline SDL_Color BLACK = {0, 0, 0, 255};

inline SDL_Color RED = {255, 0, 0, 255};

inline SDL_Color WHITE = {255, 255, 255, 255};

inline SDL_Color YELLOW = {255, 255, 0, 255};

inline SDL_Color RED_HEALTH_FILL = {255, 0, 77, 125};




#endif //SDL_CONSTANTS_H
