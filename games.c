#include "geckonator/gpio.h"

#include "menu.h"

void snake(void);

static const struct menuitem game_menu[] = {
        { .label = "Snake",  .cb = snake, },
};

void games(void) {
	menu(game_menu, ARRAY_SIZE(game_menu), 0xCB0, 0x000);
}
