/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <sys/printk.h>
#include <drivers/gpio.h>


#define GPIO_PORT	"GPIO_0"
// #define GPIO_PORT DT_LABEL(DT_NODELABEL(gpio0))

#define	BUTTON1_PIN	13

#define LED1_PIN	17

static struct gpio_callback button1_cb_data;

static struct k_work button1_work;

const struct device *my_gpio_port;

struct k_timer my_timer;

void my_timer_handler(struct k_timer *dummy);

void button1_pressed(const struct device *port,
					struct gpio_callback *cb,
					gpio_port_pins_t pins)
{
	int ret;
	printk("Button1 work is pressed\n");
	ret = k_work_submit(&button1_work);
	if(ret == 0)
	{
		printk("Button1 work is already in queue\n");
	}
	
	k_msleep(12000);
}

void button1_work_handler(struct k_work *work)
{
	k_timer_init(&my_timer, my_timer_handler, NULL);
	k_timer_start(&my_timer, K_SECONDS(10), K_NO_WAIT);
	k_work_cancel(&button1_work);
}

void my_timer_handler(struct k_timer *dummy)
{
	printk("Timer expiry function worked\n");
	int val = gpio_pin_get(my_gpio_port,BUTTON1_PIN);
	if( val == 0){
		gpio_pin_toggle(my_gpio_port,LED1_PIN);	
	}   
	k_work_init(&button1_work, button1_work_handler);
}

void configure_gpios()
{
	int ret;

	my_gpio_port = device_get_binding(GPIO_PORT);

	gpio_pin_configure(my_gpio_port,BUTTON1_PIN , GPIO_INPUT | GPIO_PULL_UP);

	gpio_pin_configure(my_gpio_port, LED1_PIN , GPIO_OUTPUT | GPIO_OUTPUT_ACTIVE);

	gpio_init_callback(&button1_cb_data, button1_pressed, BIT(BUTTON1_PIN));
	gpio_add_callback(my_gpio_port, &button1_cb_data);

	gpio_pin_interrupt_configure(my_gpio_port, BUTTON1_PIN, GPIO_INT_EDGE_TO_INACTIVE);

	k_work_init(&button1_work, button1_work_handler);
}

void main(void)
{
	printk("Hello World! %s\n", CONFIG_BOARD);
	configure_gpios();	
}
