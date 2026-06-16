#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <string.h>
#include <stdio.h>

#define DOT_DURATION 300     // ms threshold for dot/dash
#define MAX_MORSE_LEN 5

static void gpio_setup(void);
static void usart_setup(void);
static void delay_ms(uint32_t ms);

char morse[20];
uint8_t idx = 0;

int _write(int file, char *ptr, int len);

//---------------------------------------------------------------------------

typedef struct {
    const char *code;
    char letter;
} MorseCode;

static const MorseCode morse_table[] = {
    {".-", 'A'},   {"-...", 'B'}, {"-.-.", 'C'}, {"-..", 'D'},  {".", 'E'},
    {"..-.", 'F'}, {"--.", 'G'},  {"....", 'H'}, {"..", 'I'},   {".---", 'J'},
    {"-.-", 'K'},  {".-..", 'L'}, {"--", 'M'},   {"-.", 'N'},   {"---", 'O'},
    {".--.", 'P'}, {"--.-", 'Q'}, {".-.", 'R'},  {"...", 'S'},  {"-", 'T'},
    {"..-", 'U'},  {"...-", 'V'}, {".--", 'W'},  {"-..-", 'X'}, {"-.--", 'Y'},
    {"--..", 'Z'},
    {"-----", ' '}
};
#define MORSE_TABLE_LEN (sizeof(morse_table)/sizeof(morse_table[0]))

static char morse_to_char(const char *code) {
    for (uint8_t i = 0; i < MORSE_TABLE_LEN; i++) {
        if (strcmp(code, morse_table[i].code) == 0)
            return morse_table[i].letter;
    }
    return '?'; // unknown
}


//---------------------------------------------------------------------------
int main(void) {
    rcc_clock_setup_pll(&rcc_hse_configs[RCC_CLOCK_HSE8_72MHZ]);
    gpio_setup();
    usart_setup();

    printf("Morse Code Ready...\r\n");

    while (1) {
        // --- START when LED button (PA0) is pressed ---
        if (gpio_get(GPIOA, GPIO0)) {
            gpio_clear(GPIOC, GPIO13);  // LED ON
            //printf("Session Started...\r\n");
            delay_ms(200);
            idx = 0;

            // --- INPUT while button is held ---
            while (gpio_get(GPIOA, GPIO0)) {
                if (gpio_get(GPIOA, GPIO1)) {
                    uint32_t t = 0;
                    while (gpio_get(GPIOA, GPIO1)) {
                        delay_ms(10);
                        t += 10;
                    }

                    if (t < DOT_DURATION)
                        morse[idx++] = '.';
                    else
                        morse[idx++] = '-';

                    morse[idx] = '\0';
                    printf("%c", morse[idx - 1]);
                    delay_ms(300);
                }
            }

            // --- STOP when LED button released ---
            gpio_set(GPIOC, GPIO13); // LED OFF
            //printf("\r\nSession Stopped.\r\nMorse Input: %s\r\n", morse);
            delay_ms(500);
            char decoded = morse_to_char(morse);
            //printf("Decoded Character: %c\r\n", decoded);
            printf("\t%c\r\n", decoded);
            delay_ms(500);
        }
    }
}

/* ---------------- Helper Functions ---------------- */

static void gpio_setup(void) {
    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_GPIOC);

    // PA0 → Start/Stop button, PA1 → Morse input
    gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, GPIO0 | GPIO1);
    gpio_clear(GPIOA, GPIO0 | GPIO1); // Pull-up

    // PC13 → LED
    gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);
    gpio_set(GPIOC, GPIO13); // LED OFF
}   

static void usart_setup(void) {
    rcc_periph_clock_enable(RCC_USART1);

    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,
                  GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART1_TX);

    usart_set_baudrate(USART1, 9600);
    usart_set_databits(USART1, 8);
    usart_set_stopbits(USART1, USART_STOPBITS_1);
    usart_set_mode(USART1, USART_MODE_TX);
    usart_set_parity(USART1, USART_PARITY_NONE);
    usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);
    usart_enable(USART1);
}

static void delay_ms(uint32_t ms) {
    for (uint32_t i = 0; i < (ms * 8000); i++) {
        __asm__("nop");
    }
}

// printf support for USART1
int _write(int file, char *ptr, int len) {
    (void)file;
    int i;
    for (i = 0; i < len; i++) {
        if (ptr[i] == '\n')
            usart_send_blocking(USART1, '\r');
        usart_send_blocking(USART1, ptr[i]);
    }
    return i;
}
