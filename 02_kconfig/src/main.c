#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/led_strip.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(kconfig);

static const struct device *led = DEVICE_DT_GET(DT_ALIAS(led_strip));

#if defined(CONFIG_COR_VERMELHO)
static struct led_rgb cor = { .r = 64 };
#elif defined(CONFIG_COR_VERDE)
static struct led_rgb cor = { .g = 64 };
#else
static struct led_rgb cor = { .b = 64 };
#endif

static struct led_rgb apagado;

int main(void)
{
    if (!device_is_ready(led)) {
        LOG_ERR("led_strip nao esta pronto");
        return 0;
    }

    LOG_INF("piscando a cada %d ms", CONFIG_PISCA_MS);

    while (1) {
        led_strip_update_rgb(led, &cor, 1);
        k_sleep(K_MSEC(CONFIG_PISCA_MS));

        led_strip_update_rgb(led, &apagado, 1);
        k_sleep(K_MSEC(CONFIG_PISCA_MS));
    }
}
