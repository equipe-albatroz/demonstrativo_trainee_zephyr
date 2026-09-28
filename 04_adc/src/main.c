#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/drivers/led_strip.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(adc);

static const struct device *led = DEVICE_DT_GET(DT_ALIAS(led_strip));
static const struct adc_dt_spec pot = ADC_DT_SPEC_GET(DT_PATH(zephyr_user));

static int32_t le_millivolts(void)
{
    uint16_t bruto;
    struct adc_sequence seq = { .buffer = &bruto, .buffer_size = sizeof(bruto) };

    adc_sequence_init_dt(&pot, &seq);
    adc_read_dt(&pot, &seq);

    int32_t mv = bruto;
    adc_raw_to_millivolts_dt(&pot, &mv);   // mv ja vem corrigido pela referencia e pelo ganho

    return mv;
}

static void pinta(int32_t mv)
{
    uint8_t nivel = CLAMP(mv * 64 / 3300, 0, 64);
    struct led_rgb cor = { .r = nivel, .g = 64 - nivel, .b = 0 };

    led_strip_update_rgb(led, &cor, 1);
}

int main(void)
{
    if (!device_is_ready(led) || !adc_is_ready_dt(&pot)) {
        LOG_ERR("led_strip ou adc nao esta pronto");
        return 0;
    }

    adc_channel_setup_dt(&pot);

    while (1) {
        int32_t mv = le_millivolts();

        LOG_INF("%d mV", mv);
        pinta(mv);

        k_sleep(K_MSEC(100));
    }
}
