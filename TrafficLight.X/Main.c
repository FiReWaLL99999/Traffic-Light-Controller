#include <xc.h>
#include <stdint.h>
#include <stdbool.h>

#define _XTAL_FREQ 8000000

#define MODE_INT_PIN  RB0 
#define MANUAL_PIN    RB1  
#define READ_MANUAL() (PORTBbits.RB1)

typedef enum {MODE_AUTO = 0, MODE_MANUAL = 1} mode_t;
volatile mode_t systemMode = MODE_AUTO;

volatile uint8_t manual_active = 0;

volatile int west_timer = 0;
volatile int south_timer = 0;

volatile unsigned char int_debounce = 0;

void init_ports(void);
void update_displays(int south_seconds, int west_seconds);
void set_lights(bool south_red, bool south_yellow, bool south_green,
                bool west_red, bool west_yellow, bool west_green);
void reset_cycle_timers(void);
void handle_manual_toggle(void);
void delay_100ms_and_poll_buttons(uint8_t *button_pressed_flag);

void __interrupt() isr(void) {
    if(INTF) {
        if (!int_debounce) {
            // toggle mode
            if(systemMode == MODE_AUTO) systemMode = MODE_MANUAL;
            else systemMode = MODE_AUTO;
            reset_cycle_timers();
            int_debounce = 1;
        }
        INTF = 0;
    }
}

void main(void) {
    init_ports();
    reset_cycle_timers();

    while(1) {
        if(systemMode == MODE_AUTO) {
            set_lights(true, false, false, 
                       false, false, true); 
            for (int s = 20; s > 0; --s) {
                west_timer = s;            
                south_timer = s + 3;     
                update_displays(south_timer, west_timer);
                for (int k=0;k<10;k++) delay_100ms_and_poll_buttons(NULL);
            }

            set_lights(true, false, false,
                       false, true, false);
            for (int s = 3; s > 0; --s) {
                west_timer = s;
                south_timer = s;   
                update_displays(south_timer, west_timer);
                for (int k=0;k<10;k++) delay_100ms_and_poll_buttons(NULL);
            }

            set_lights(false, false, true, 
                       true, false, false);
            for (int s = 12; s > 0; --s) {
                south_timer = s;       
                west_timer = s + 3;   
                update_displays(south_timer, west_timer);
                for (int k=0;k<10;k++) delay_100ms_and_poll_buttons(NULL);
            }

            set_lights(false, true, false,
                       true, false, false);
            for (int s = 3; s > 0; --s) {
                south_timer = s;        
                west_timer = s;         
                update_displays(south_timer, west_timer);
                for (int k=0;k<10;k++) delay_100ms_and_poll_buttons(NULL);
            }

        }
        else {
            if (manual_active == 0) {
                set_lights(true, false, false, 
                           false, false, true); 
                update_displays(0, 0);

                uint8_t pressed = 0;
                while(!pressed && systemMode == MODE_MANUAL) {
                    delay_100ms_and_poll_buttons(&pressed);
                }
                if (pressed) {
                    set_lights(true, false, false,
                               false, true, false);
                    for (int s=3; s>0; --s) {
                        update_displays(0, s);
                        for (int k=0;k<10;k++) delay_100ms_and_poll_buttons(NULL);
                    }
                    manual_active = 1;
                }
            }
            else {
                set_lights(false, false, true,
                           true, false, false); 
                update_displays(0,0);

                uint8_t pressed = 0;
                while(!pressed && systemMode == MODE_MANUAL) {
                    delay_100ms_and_poll_buttons(&pressed);
                }
                if (pressed) {
                    set_lights(false, true, false,
                               true, false, false);
                    for (int s=3; s>0; --s) {
                        update_displays(s, 0);
                        for (int k=0;k<10;k++) delay_100ms_and_poll_buttons(NULL);
                    }
                    manual_active = 0;
                }
            }
        }
    }
}

void init_ports(void) {
    
    TRISA = 0x00; 
    PORTA = 0x00;

    TRISC = 0x00;
    PORTC = 0x00;
    TRISD = 0x00;
    PORTD = 0x00;

    TRISB = 0xFF; 
    INTF = 0;
    INTEDG = 0; 
    GIE = 1;    
    INTE = 1;   

    OPTION_REGbits.nRBPU = 0;

    int_debounce = 0;
}

void update_displays(int south_seconds, int west_seconds) {
    if (south_seconds < 0) south_seconds = 0;
    if (south_seconds > 99) south_seconds = 99;
    if (west_seconds < 0) west_seconds = 0;
    if (west_seconds > 99) west_seconds = 99;

    uint8_t s_tens = south_seconds / 10;
    uint8_t s_ones = south_seconds % 10;
    uint8_t w_tens = west_seconds / 10;
    uint8_t w_ones = west_seconds % 10;

    PORTC = (uint8_t)((s_tens << 4) | (s_ones & 0x0F));
    PORTD = (uint8_t)((w_tens << 4) | (w_ones & 0x0F));
}

void set_lights(bool s_red, bool s_yel, bool s_grn,
                bool w_red, bool w_yel, bool w_grn) {
    PORTAbits.RA0 = s_red ? 1 : 0;
    PORTAbits.RA1 = s_yel ? 1 : 0;
    PORTAbits.RA2 = s_grn ? 1 : 0;
    PORTAbits.RA3 = w_grn ? 1 : 0;
    PORTAbits.RA4 = w_yel ? 1 : 0;
    PORTAbits.RA5 = w_red ? 1 : 0;
}

void reset_cycle_timers(void) {
    west_timer = 20;  
    south_timer = 23;  
}

void delay_100ms_and_poll_buttons(uint8_t *flag_p) {
    static uint8_t rb1_count = 0;
    const uint8_t DEBOUNCE_THRESHOLD = 3;

    __delay_ms(100);

    if (READ_MANUAL() == 0) {
        if (rb1_count < 255) rb1_count++;
    } else {
        if (rb1_count > 0) rb1_count--;
    }

    if (rb1_count >= DEBOUNCE_THRESHOLD) {
        rb1_count = 0;
        if (flag_p) *flag_p = 1;
    }

    if (int_debounce) {
        static uint16_t int_deb_ctr = 0;
        int_deb_ctr++;
        if (int_deb_ctr > 20) {
            int_deb_ctr = 0;
            int_debounce = 0;
        }
    }
}
