#include "geckonator/gpio.h"

#include "leds.h"
#include "timer.h"
#include "events.h"
#include "buttons.h"
#include "display.h"
#include "power.h"
#include <stdio.h>
#include <stdlib.h>

enum events {
	EV_SUP_PUSH = 1,
	EV_SUP_RELEASE,
	EV_SMID_PUSH,
	EV_SMID_RELEASE,
	EV_SDOWN_PUSH,
	EV_SDOWN_RELEASE,
	EV_UP_PUSH,
	EV_UP_RELEASE,
	EV_DOWN_PUSH,
	EV_DOWN_RELEASE,
	EV_LEFT_PUSH,
	EV_LEFT_RELEASE,
	EV_RIGHT_PUSH,
	EV_RIGHT_RELEASE,
	EV_CENTER_PUSH,
	EV_CENTER_RELEASE,
	EV_TICK,
	EV_EXIT_TICK,
};

enum directions {
	LEFT,
	RIGHT,
	UP,
	DOWN,
};

static const struct button_config testbuttons[BTN_MAX] = {
	[BTN_SUP]    = { .press = EV_SUP_PUSH,    .release = EV_SUP_RELEASE,    },
	[BTN_SMID]   = { .press = EV_SMID_PUSH,   .release = EV_SMID_RELEASE,   },
	[BTN_SDOWN]  = { .press = EV_SDOWN_PUSH,  .release = EV_SDOWN_RELEASE,  },
	[BTN_UP]     = { .press = EV_UP_PUSH,     .release = EV_UP_RELEASE,     },
	[BTN_DOWN]   = { .press = EV_DOWN_PUSH,   .release = EV_DOWN_RELEASE,   },
	[BTN_LEFT]   = { .press = EV_LEFT_PUSH,   .release = EV_LEFT_RELEASE,   },
	[BTN_RIGHT]  = { .press = EV_RIGHT_PUSH,  .release = EV_RIGHT_RELEASE,  },
	[BTN_CENTER] = { .press = EV_CENTER_PUSH, .release = EV_CENTER_RELEASE, },
};


typedef struct snake_s {
	int x;
	int y;
	int length;
	int tail[576][2];
	int apple[2];
	int last_tail[2];
	enum directions direction;
} snake_t;

void snake_set_apple(snake_t* snake) {
	unsigned int x = (rand() % 24)*10;
	unsigned int y = (rand() % 24)*10;
	snake->apple[0] = x;
	snake->apple[1] = y;
}


void snake_init(snake_t* snake) {
	srand(timer_now());
	snake->x = 120;
	snake->y = 120;
	snake->length = 0;
	snake->tail[0][0] = -1;
	snake->direction = DOWN;
	snake_set_apple(snake);
}

void snake_move_head(snake_t* snake) {
	switch (snake->direction) {
		case DOWN:
			snake->y = snake->y+10;
			break;
		case UP:
			snake->y = snake->y-10;
			break;
		case LEFT:
			snake->x = snake->x-10;
			break;
		case RIGHT:
			snake->x = snake->x+10;
			break;
	}
}

void snake_move_tail(snake_t* snake) {
	snake->last_tail[0] = snake->tail[0][0];
	snake->last_tail[1] = snake->tail[0][1];
	for (int i = 0; i < snake->length-1; i++) {
		snake->tail[i][0] = snake->tail[i+1][0];
		snake->tail[i][1] = snake->tail[i+1][1];
	}
	snake->tail[snake->length-1][0] = snake->x;
	snake->tail[snake->length-1][1] = snake->y;
}

void snake_add_tail(snake_t* snake) {
	snake->tail[snake->length][0] = snake->x;
	snake->tail[snake->length][1] = snake->y;
	snake->length++;
	snake_move_head(snake);
}

void snake_game_over(snake_t* snake, struct ticker* ticker) {
	ticker_stop(ticker);
	dp_fill(0, 0, 240, 240, 0x000);

	char buf[12] = { 0 };
	sprintf(buf, "  Length: %d", snake->length+1);

	dp_puts(25, 50,      0xF00, 0x00, "    Game Over");
	dp_puts(45, 50 + 28, 0xCB0, 0x00, buf);
	timer_msleep(3000);
}

void snake_check_bounds(snake_t* snake) {
	if (snake->x<=-10) {
		snake->x = 240;
	}
	if (snake->y<=-10) {
		snake->y = 240;
	}
	if (snake->x>=250) {
		snake->x = 0;
	}
	if (snake->y>=250) {
		snake->y = 0;
	}
}

void snake(void) {
	struct ticker tick;
	unsigned int count = 0;

	snake_t snake;
	snake_init(&snake);
	snake_add_tail(&snake);

	dp_fill(0, 0, 240, 240, 0x000);
	dp_puts(25, 50,      0x00F, 0x00, "     Snake   ");
	dp_puts(45, 50 + 28, 0x00F, 0x00, "by Sebastian");
	timer_msleep(1500);
	dp_fill(0, 0, 240, 240, 0x000);
	buttons_config(testbuttons);

	ticker_start(&tick, 350, EV_TICK);

	while (1) {
		switch ((enum events)event_wait()) {
		case EV_UP_PUSH:
			snake.direction = UP;
			break;
		case EV_DOWN_PUSH:
			snake.direction = DOWN;
			break;
		case EV_LEFT_PUSH:
			snake.direction = LEFT;
			break;
		case EV_RIGHT_PUSH:
			snake.direction = RIGHT;
			break;
		case EV_SUP_PUSH:
			snake_add_tail(&snake);
			break;
		case EV_SMID_PUSH:
			snake_set_apple(&snake);
			break;
		case EV_SDOWN_PUSH:
			snake_game_over(&snake, &tick);
			return;
		case EV_TICK:
			snake_move_tail(&snake);
			snake_move_head(&snake);
			snake_check_bounds(&snake);
			for (int i = 0; i < snake.length; i++) {
				if (snake.tail[i][0] == snake.x && snake.tail[i][1] == snake.y){
					snake_game_over(&snake, &tick);
					return;
				}
			}
			if (snake.x == snake.apple[0] && snake.y == snake.apple[1]) {
				snake_add_tail(&snake);
				snake_set_apple(&snake);
			}
			//dp_fill(0, 0, 240, 240, 0x000);
			for (int i = 0; i < snake.length; i++) {
				dp_fill(snake.tail[i][0], snake.tail[i][1], 10, 10, 0xCB0);
			}
			dp_fill(snake.last_tail[0], snake.last_tail[1], 10, 10, 0x000);
			dp_fill(snake.x, snake.y, 10, 10, 0x00F);
			dp_fill(snake.apple[0], snake.apple[1], 10, 10, 0xF00);
			//Exit
			if (power_pressed()) {
				count += 1;
				if (count == 3) {
					snake_game_over(&snake, &tick);
					return;
				}
			} else {
				count = 0;
			}
			//End exit
			break;
		default:
			break;
		}
	}
}
