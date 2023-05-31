#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/dma.h"
#include "blink.pio.h"

int main() {
    const uint LED_PIN_TO_USE=15;
    stdio_init_all();

    // Initialize the PIO instance
    PIO pio = pio0;
    uint offset = pio_add_program(pio, &blink_program);

    // Create a PIO state machine instance
    uint sm = pio_claim_unused_sm(pio, true);

    // Load the program into the PIO state machine
    pio_sm_config c = blink_program_get_default_config(offset);
    sm_config_set_sideset_pins(&c, LED_PIN_TO_USE); // Set the LED pin
    pio_sm_init(pio, sm, offset, &c);

    // Start the PIO state machine
    pio_sm_set_enabled(pio, sm, true);
    printf("LED blinking started...\n");

    while (true) {
        tight_loop_contents();
    }

    return 0;
}
