#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/led_strip.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(overlay);

#define N_PIXELS DT_PROP(DT_ALIAS(led_strip), chain_length)

static const struct device *led = DEVICE_DT_GET(DT_ALIAS(led_strip));

static struct led_rgb pixels[N_PIXELS];

static const struct led_rgb cores[] = {
    { .r = 64, .g = 0,  .b = 0  },
    { .r = 0,  .g = 64, .b = 0  },
    { .r = 0,  .g = 0,  .b = 64 },
};

int main(void)
{
    if (!device_is_ready(led)) {
        LOG_ERR("led_strip nao esta pronto");
        return 0;
    }

    LOG_INF("pintando %d pixels", N_PIXELS);

    int i = 0;

    while (1) {
        for (int p = 0; p < N_PIXELS; p++) {
            pixels[p] = cores[i];
        }
        led_strip_update_rgb(led, pixels, N_PIXELS);

        i = (i + 1) % ARRAY_SIZE(cores);
        k_sleep(K_MSEC(500));
    }
}
