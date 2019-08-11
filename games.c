#include "geckonator/gpio.h"

#include "menu.h"

void snake(void);
void pong(void);

static const struct menuitem game_menu[] = {
    { .label = "Snake",  .cb = snake, },
    { .label = "Pong",  .cb = pong, },
};

void games(void) {
	menu(game_menu, ARRAY_SIZE(game_menu), 0xCB0, 0x000);
}
