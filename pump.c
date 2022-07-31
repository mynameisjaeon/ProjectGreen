#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/binary_info.h"


int main() {
	const uint IN1PIN = 12;
	const uint IN2PIN = 13;
	gpio_init(IN1PIN);
	gpio_init(IN2PIN);
	gpio_set_dir(IN1PIN, GPIO_OUT);
	gpio_set_dir(IN2PIN, GPIO_OUT);
	while (true) {
		gpio_put(IN1PIN, 0);
		gpio_put(IN2PIN, 0);
		sleep_ms(5000);
		gpio_put(IN1PIN, 1);
		sleep_ms(5000);
	}
}
