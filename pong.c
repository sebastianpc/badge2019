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

typedef struct point_s {
    char x, y;
} point_t;

typedef struct pong_s {
	point_t paddle;
	point_t ball_pos;
	point_t ball_vel;
} pong_t;

void pong(void) {
	struct ticker tick;
	unsigned int count = 0;

	ticker_start(&tick, 350, EV_TICK);

	pong_t pong;
	//pong_init(&pong);

	dp_fill(0, 0, 240, 240, 0x000);
	dp_puts(25, 50,      0x00F, 0x00, "     Snake   ");
	dp_puts(45, 50 + 28, 0x00F, 0x00, "by Sebastian");
	timer_msleep(1500);
	dp_fill(0, 0, 240, 240, 0x000);
	buttons_config(testbuttons);

	while (1) {
		switch ((enum events)event_wait()) {
		case EV_TICK:
			if (power_pressed()) {
				count += 1;
				if (count == 3) {
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
