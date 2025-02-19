#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "hardware/i2c.h"
#include "lib/ssd1306.h"

#define LED_R_PIN 11
#define LED_G_PIN 12
#define LED_B_PIN 13
#define JOYSTICK_X_PIN 26
#define JOYSTICK_Y_PIN 27
#define JOYSTICK_BUTTON_PIN 22
#define BUTTON_A_PIN 5
#define I2C_SDA_PIN 14
#define I2C_SCL_PIN 15

volatile bool led_green_state = false;
volatile bool pwm_enabled = true;
volatile int border_style = 0;

void joystick_button_isr(uint gpio, uint32_t events) {
    led_green_state = !led_green_state;
    gpio_put(LED_G_PIN, led_green_state);

    border_style = (border_style + 1) % 3;
}

void button_a_isr(uint gpio, uint32_t events) {
    pwm_enabled = !pwm_enabled;
}

void setup_pwm(uint pin) {
    gpio_set_function(pin, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(pin);
    pwm_set_wrap(slice_num, 4095);
    pwm_set_enabled(slice_num, true);
}

void setup_adc(uint pin) {
    adc_gpio_init(pin);
}

void setup_i2c() {
    i2c_init(i2c0, 400 * 1000);
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);
}

int main() {
    stdio_init_all();

    gpio_init(LED_R_PIN);
    gpio_init(LED_G_PIN);
    gpio_init(LED_B_PIN);
    gpio_set_dir(LED_R_PIN, GPIO_OUT);
    gpio_set_dir(LED_G_PIN, GPIO_OUT);
    gpio_set_dir(LED_B_PIN, GPIO_OUT);

    gpio_init(JOYSTICK_BUTTON_PIN);
    gpio_set_dir(JOYSTICK_BUTTON_PIN, GPIO_IN);
    gpio_pull_up(JOYSTICK_BUTTON_PIN);
    gpio_set_irq_enabled_with_callback(JOYSTICK_BUTTON_PIN, GPIO_IRQ_EDGE_FALL, true, &joystick_button_isr);

    gpio_init(BUTTON_A_PIN);
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_A_PIN);
    gpio_set_irq_enabled_with_callback(BUTTON_A_PIN, GPIO_IRQ_EDGE_FALL, true, &button_a_isr);

    setup_pwm(LED_R_PIN);
    setup_pwm(LED_B_PIN);
    setup_adc(JOYSTICK_X_PIN);
    setup_adc(JOYSTICK_Y_PIN);
    setup_i2c();

    ssd1306_t display;
    ssd1306_init(&display, 128, 64, false, 0x3C, i2c0);

    while (true) {
        adc_select_input(0);
        uint16_t x_value = adc_read();
        adc_select_input(1);
        uint16_t y_value = adc_read();

        if (pwm_enabled) {
            pwm_set_gpio_level(LED_R_PIN, x_value);
            pwm_set_gpio_level(LED_B_PIN, y_value);
        } else {
            pwm_set_gpio_level(LED_R_PIN, 0);
            pwm_set_gpio_level(LED_B_PIN, 0);
        }

        ssd1306_clear(&display);
        ssd1306_draw_square(&display, x_value / 32, y_value / 32, 8, 8);
        ssd1306_show(&display);

        sleep_ms(100);
    }

    return 0;
}