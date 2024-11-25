#include <stdbool.h>
#include <stdlib.h>

#include <avr/io.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/atomic.h>

#include "speech_synth.h"

#define LED_PIN PIN4_bm

ISR(RTC_PIT_vect)
{
    RTC.PITINTFLAGS = RTC_PI_bm;
}

static void configure_pins(void)
{
    PORTA.DIR = 0;
    PORTA.PIN0CTRL |= PORT_PULLUPEN_bm;
    PORTA.PIN1CTRL |= PORT_PULLUPEN_bm;
    PORTA.PIN2CTRL |= PORT_PULLUPEN_bm;
    PORTA.PIN3CTRL |= PORT_PULLUPEN_bm;
    PORTA.PIN4CTRL |= PORT_PULLUPEN_bm;
    PORTA.PIN5CTRL |= PORT_PULLUPEN_bm;
    PORTA.PIN6CTRL |= PORT_PULLUPEN_bm;
    PORTA.PIN7CTRL |= PORT_PULLUPEN_bm;

    PORTB.DIR = 0;
    PORTB.PIN0CTRL |= PORT_PULLUPEN_bm;
    PORTB.PIN1CTRL |= PORT_PULLUPEN_bm;
    PORTB.PIN2CTRL |= PORT_PULLUPEN_bm;
    PORTB.PIN3CTRL |= PORT_PULLUPEN_bm;
    PORTB.PIN4CTRL |= PORT_PULLUPEN_bm;
    PORTB.PIN5CTRL |= PORT_PULLUPEN_bm;

    PORTC.DIR = 0;
    PORTC.PIN0CTRL |= PORT_PULLUPEN_bm;
    PORTC.PIN1CTRL |= PORT_PULLUPEN_bm;
    PORTC.PIN2CTRL |= PORT_PULLUPEN_bm;
    PORTC.PIN3CTRL |= PORT_PULLUPEN_bm;
}

static void rtc_init(void)
{
    while (RTC.STATUS > 0)
    {
        ; /* Wait for all register to be synchronized */
    }

    RTC.CTRLA |= RTC_RUNSTDBY_bm;
    RTC.CLKSEL = RTC_CLKSEL_INT1K_gc;

    RTC.DBGCTRL = RTC_DBGRUN_bm;
    RTC.PITINTCTRL = RTC_PI_bm;           /* Periodic Interrupt: enabled */
    RTC.PITCTRLA = RTC_PERIOD_CYC32768_gc /* RTC Clock Cycles 32768 */
                   | RTC_PITEN_bm;        /* Enable: enabled */
}

static int16_t internal_temp_read(void)
{
    ADC0.CTRLA |= ADC_ENABLE_bm;
    ADC0.CTRLC = ADC_REFSEL_1024MV_gc;
    ADC0.MUXPOS = ADC_MUXPOS_TEMPSENSE_gc;
    ADC0.CTRLE = 200;
    ADC0.COMMAND = ADC_MODE_SINGLE_12BIT_gc | ADC_START_IMMEDIATE_gc;

    while (!(ADC0.INTFLAGS & ADC_RESRDY_bm))
    {
        ;
    }
    ADC0.INTFLAGS = ADC_RESRDY_bm;

    const int8_t sigrow_offset = SIGROW.TEMPSENSE1;
    const uint8_t sigrow_gain = SIGROW.TEMPSENSE0;
    const uint16_t adc_reading = ADC0.RESULT >> 2;
    uint32_t temp = adc_reading - sigrow_offset;
    temp *= sigrow_gain;
    temp += 0x80;
    temp >>= 8;
    uint16_t temperature_in_K = temp;
    ADC0.CTRLA = 0;

    return ((int16_t)temperature_in_K * 10) - 2732;
}

static void say_temp(speech_synth_t *synth, int16_t t)
{
    uint16_t d;

    if ((abs(t) / 10) >= 60)
    {
        speech_synth_say(synth, lpc_name_out_of_range);
    }
    else
    {
        if (t < 0)
        {
            speech_synth_say(synth, lpc_name_minus);
            t = abs(t);
        }

        d = t / 10;

        if (d < 60)
        {
            if (d < 21)
            {
                speech_synth_say(synth, lpc_name_zero + d);
            }
            else
            {
                d = t / 100;
                speech_synth_say(synth, lpc_name_twenty + d - 2);
                d = (t / 10) % 10;
                speech_synth_say(synth, lpc_name_zero + d);
            }

            speech_synth_say(synth, lpc_name_point);
            d = t % 10;
            speech_synth_say(synth, lpc_name_zero + d);
        }
    }
}

int main(void)
{
    // 10MHz clock
    _PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, CLKCTRL_PDIV_2X_gc | CLKCTRL_PEN_bm);
    _PROTECTED_WRITE(CLKCTRL.OSC20MCTRLA, CLKCTRL_RUNSTDBY_bp);
    power_all_disable();
    configure_pins();
    rtc_init();

    PORTB.DIRSET = LED_PIN;
    PORTB.OUTSET = LED_PIN;

    set_sleep_mode(SLEEP_MODE_STANDBY);
    sei();

    speech_synth_t *synth = speech_synth_init();
    speech_synth_tone(synth);
    speech_synth_say(synth, lpc_name__talking__digital__thermometer_starting);

    for (;;)
    {
        set_sleep_mode(SLEEP_MODE_STANDBY);
        PORTB.OUTSET = LED_PIN;
        const int16_t t = internal_temp_read();
        speech_synth_tone(synth);
        speech_synth_say(synth, lpc_name_temperature);
        say_temp(synth, t);

        // Prepare for sleep
        ATOMIC_BLOCK(ATOMIC_FORCEON)
        {
            RTC.CNT = 0;
            RTC.PITINTFLAGS = RTC_PI_bm;
            set_sleep_mode(SLEEP_MODE_PWR_DOWN);
            PORTB.OUTCLR = LED_PIN;
        }
        // Sleep for a minute
        for (uint8_t i = 0; i < 2; i++)
        {
            sleep_mode();
        }
    }
}
