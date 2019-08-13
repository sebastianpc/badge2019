#include "geckonator/gpio.h"

#include "leds.h"
#include "timer.h"
#include "events.h"
#include "buttons.h"
#include "display.h"
#include "power.h"
#include <stdio.h>
#include <stdlib.h>

#define PADDLE_HEIGHT	5*10
#define PADDLE_WIDTH	5*1
#define BALL_SIZE	10

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
};

static const struct button_config testbuttons[BTN_MAX] = {
	[BTN_SUP]    = { .press = EV_SUP_PUSH,    .release = EV_SUP_RELEASE,    },
	[BTN_SMID]   = { .press = EV_SMID_PUSH,   .release = EV_SMID_RELEASE,   },
	[BTN_SDOWN]  = { .press = EV_SDOWN_PUSH,  .release = EV_SDOWN_RELEASE,  },
	[BTN_UP]     = { .press = EV_UP_PUSH,     .release = EV_UP_RELEASE,     },
	[BTN_DOWN]   = { .press = EV_DOWN_PUSH,   .release = EV_DOWN_RELEASE,   },
	[BTN_LEFT]   = { .press = EV_LEFT_PUSH,   .rate = 20,	.repeat = EV_LEFT_PUSH,	.release = EV_LEFT_RELEASE,   },
	[BTN_RIGHT]  = { .press = EV_RIGHT_PUSH,  .rate = 20,	.repeat = EV_RIGHT_PUSH, .release = EV_RIGHT_RELEASE,  },
	[BTN_CENTER] = { .press = EV_CENTER_PUSH, .release = EV_CENTER_RELEASE, },
};

typedef struct point_s {
    char x, y;
} point_t;

typedef struct pong_s {
	point_t paddle;
	point_t ball_pos;
	point_t ball_vel;
	//point_t ball_old_pos;
} pong_t;
/*
void pong_init(pong_t* pong) {
	pong->paddle = { .x = 20, .y = 220 };
	pong->ball_pos = { .x = 120, .y = 120 };
	pong->ball_vel = { .x = 1, .y = 1};
}
*/

void pong_move_ball(pong_t* pong) {
	dp_fill(pong->ball_pos.x, pong->ball_pos.y, BALL_SIZE, BALL_SIZE, 0x000);
	pong->ball_pos.x += pong->ball_vel.x;
	pong->ball_pos.y += pong->ball_vel.y;
	dp_fill(pong->ball_pos.x, pong->ball_pos.y, BALL_SIZE, BALL_SIZE, 0xFFF);

	// Ball hits paddle
	if (pong->ball_pos.x >= pong->paddle.x && pong->ball_pos <= pong->paddle.x + PADDLE_HEIGHT && pong->ball_pos.y + BALL_SIZE >= pong->paddle.y) {

		pong->ball_vel.y = -pong->ball_vel.y;
	}
	if (pong->ball_pos.x >= 240 || pong->ball_pos.x <= 0) {
		pong->ball_vel.x = -pong->ball_vel.x;
	}
	if (pong->ball_pos.y  >= (pong->paddle.y + PADDLE_WIDTH) && !(pong->ball_pos.x >= pong->paddle.x && pong->ball_pos.x <= pong->paddle.x + PADDLE_HEIGHT)){
		dp_puts(25, 50, 0x00F, 0X00, "GAME OVER");
		timer_msleep(1500);
	}
}

void pong(void) {
	struct ticker tick;
	unsigned int count = 0;

	ticker_start(&tick, 350, EV_TICK);

	pong_t pong = {
		.paddle = { .x = 20, .y = 220},
		.ball_pos = { .x = 50, .y = 120 },
		.ball_vel = { .x = 1, .y = 5 }
	};
	
	dp_fill(0, 0, 240, 240, 0x000);
	dp_puts(25, 50,      0x00F, 0x00, "       Pong   ");
	dp_puts(45, 50 + 28, 0x00F, 0x00, "  by Pwnies");
	timer_msleep(1500);
	dp_fill(0, 0, 240, 240, 0x000);
	buttons_config(testbuttons);

	while (1) {
		switch ((enum events)event_wait()) {
		case EV_LEFT_PUSH:
			pong.paddle.x -= 1;
			break;
		case EV_RIGHT_PUSH:
			pong.paddle.x += 1;
			break;
		case EV_TICK:
			dp_fill(0, 0, 240, 240, 0x000);
			pong_move_ball(&pong);
			dp_fill(pong.paddle.x, pong.paddle.y, PADDLE_HEIGHT, PADDLE_WIDTH, 0xFFF);
			if (power_pressed()) {
				count += 1;
				if (count == 3) {
					ticker_stop(&tick);
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
