#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/drivers/led_strip.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(threads);

static const struct device *led = DEVICE_DT_GET(DT_ALIAS(led_strip));
static const struct adc_dt_spec pot = ADC_DT_SPEC_GET(DT_PATH(zephyr_user));

K_MSGQ_DEFINE(fila, sizeof(uint16_t), 4, 4);

static void tarefa_adc(void)
{
    while (1) {
        uint16_t bruto;
        struct adc_sequence seq = { .buffer = &bruto, .buffer_size = sizeof(bruto) };

        adc_sequence_init_dt(&pot, &seq);
        adc_read_dt(&pot, &seq);

        int32_t mv = bruto;
        adc_raw_to_millivolts_dt(&pot, &mv);   // mv ja vem corrigido pela referencia e pelo ganho

        uint16_t valor = mv;
        k_msgq_put(&fila, &valor, K_NO_WAIT);

        k_sleep(K_MSEC(100));
    }
}

static void tarefa_led(void)
{
    while (1) {
        uint16_t mv;

        k_msgq_get(&fila, &mv, K_FOREVER);   // Blocked aqui: parada, sem gastar processador

        uint8_t nivel = CLAMP(mv * 64 / 3300, 0, 64);
        struct led_rgb cor = { .r = nivel, .g = 64 - nivel, .b = 0 };

        led_strip_update_rgb(led, &cor, 1);
        LOG_INF("%d mV", mv);
    }
}

K_THREAD_DEFINE(thread_adc, 1024, tarefa_adc, NULL, NULL, NULL, 5, 0, 0);   // prioridade maior
K_THREAD_DEFINE(thread_led, 1024, tarefa_led, NULL, NULL, NULL, 6, 0, 0);   // prioridade menor

int main(void)
{
    if (!device_is_ready(led) || !adc_is_ready_dt(&pot)) {
        LOG_ERR("led_strip ou adc nao esta pronto");
        return 0;
    }

    adc_channel_setup_dt(&pot);

    return 0;   // as duas threads seguem rodando com a main ja terminada
}
