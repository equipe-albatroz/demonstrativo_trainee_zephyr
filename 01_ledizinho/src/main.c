#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/led_strip.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(ledizinho);

static const struct device *led = DEVICE_DT_GET(DT_ALIAS(led_strip));

static struct led_rgb cores[] = {
    { .r = 64, .g = 0,  .b = 0  },
    { .r = 0,  .g = 64, .b = 0  },   // 64 de 255: no escuro, 255 cega a plateia
    { .r = 0,  .g = 0,  .b = 64 },
};

int main(void)
{
    if (!device_is_ready(led)) {
        LOG_ERR("led_strip nao esta pronto");
        return 0;
    }

    int i = 0;

    while (1) {
        led_strip_update_rgb(led, &cores[i], 1);
        LOG_INF("cor %d", i);

        i = (i + 1) % ARRAY_SIZE(cores);
        k_sleep(K_MSEC(500));
    }
}
