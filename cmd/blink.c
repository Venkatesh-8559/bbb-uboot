/*
 * U-Boot standalone blink command
 */

#include <common.h>
#include <command.h>
#include <errno.h>
#include <dm.h>
#include <asm/gpio.h>
#include <linux/delay.h>
#include <console.h>

static int do_blink(struct cmd_tbl *cmdtp, int flag, int argc,
		    char *const argv[])
{
	unsigned int gpio;
	const char *str_gpio;
	int ret;

	if (argc != 2)
		return CMD_RET_USAGE;

	str_gpio = argv[1];

#ifdef CONFIG_DM_GPIO
	ret = gpio_lookup_name(str_gpio, NULL, NULL, &gpio);
	if (ret) {
		printf("GPIO: '%s' not found\n", str_gpio);
		return CMD_RET_FAILURE;
	}
#else
	gpio = dectoul(str_gpio, NULL);
#endif

	ret = gpio_request(gpio, "blink");
	if (ret && ret != -EBUSY) {
		printf("gpio: requesting pin %u failed\n", gpio);
		return CMD_RET_FAILURE;
	}

	ret = gpio_direction_output(gpio, 0);
	if (ret) {
		printf("gpio: failed to configure pin %u as output\n", gpio);
		goto err;
	}

	printf("Blinking GPIO %u every 100 ms ...\n", gpio);
	printf("Press Ctrl+C to stop...\n");

	while (!ctrlc()) {
		gpio_set_value(gpio, 1);
		udelay(100000);

		gpio_set_value(gpio, 0);
		udelay(100000);
	}

	gpio_set_value(gpio, 0);

	printf("\nGPIO Blink stopped.\n");

	if (ret != -EBUSY)
		gpio_free(gpio);

	return CMD_RET_SUCCESS;

err:
	if (ret != -EBUSY)
		gpio_free(gpio);

	return CMD_RET_FAILURE;
}

U_BOOT_CMD(
	blink, 2, 0, do_blink,
	"blink a GPIO pin",
	"<pin>"
);
