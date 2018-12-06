/*
 * Telemetry strings and formatting
 * Copyright (C) 2014  Richard Meadows <richardeoin>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <stdio.h>
#include <string.h>
#include <iostm8s003f3.h>
#include <stdint.h>
#include <stdbool.h>
#include "HC12Board.h"
#include "telemetry.h"
#include "rtty.h"
#include "pips.h"
#include "si_trx.h"
#include "si_trx_defs.h"
#include <intrinsics.h>

#define TIMER1_PRESCALE  (HSCLK_FREQUENCY/1000)
   
char telemetry_string[TELEMETRY_STRING_MAX] = "$$helloworldhello";   

/**
 * CYCLIC REDUNDANCY CHECK (CRC)
 * =============================================================================
 */

/**
 * CRC Function for the XMODEM protocol.
 * http://www.nongnu.org/avr-libc/user-manual/group__util__crc.html#gaca726c22a1900f9bad52594c8846115f
 */
uint16_t crc_xmodem_update(uint16_t crc, uint8_t data)
{
  int i;

  crc = crc ^ ((uint16_t)data << 8);
  for (i = 0; i < 8; i++) {
    if (crc & 0x8000) {
      crc = (crc << 1) ^ 0x1021;
    } else {
      crc <<= 1;
    }
  }

  return crc;
}

/**
 * Calcuates the CRC checksum for a communications string
 * See http://ukhas.org.uk/communication:protocol
 */
uint16_t crc_checksum(char *string)
{
  size_t i;
  uint16_t crc;
  uint8_t c;

  crc = 0xFFFF;

  for (i = 0; i < strlen(string); i++) {
    c = string[i];
    crc = crc_xmodem_update(crc, c);
  }

  return crc;
}

/**
 * TELEMETRY STRING CREATION
 * =============================================================================
 */

uint8_t telemetry_putstr(char *string) // Enter string into telemetry buffer
{
  if (strlen(string) > TELEMETRY_STRING_MAX) return false;
  else strcpy(telemetry_string, string);
  return true;
}

uint8_t telemetry_putstrCRC(char *string) //Enter string into telemetry buffer and append CRC
{
   if (strlen(string) > (TELEMETRY_STRING_MAX - 2)) return false;
   else
   {
      uint16_t checksum = crc_checksum(string);
      strcpy(telemetry_string, string);
      telemetry_string[sizeof(string) + 1] = checksum >> 8;
      telemetry_string[sizeof(string) + 2] = checksum &  0xff;
   }
   return true;
}

/**
 * TELEMETRY OUTPUT
 * =============================================================================
 */

/**
 * The type of telemetry we're currently outputting
 */
enum telemetry_t telemetry_type;
/**
 * Current output
 */
int8_t telemetry_string_length = 0;
/**
 * Where we are in the current output
 */
int8_t telemetry_index;
/**
 * Is the radio currently on?
 */
uint8_t radio_on = 0;


/**
 * Returns 1 if we're currently outputting.
 */
int telemetry_active(void) {
  return (telemetry_string_length > 0);
}

/**
 * Starts telemetry output
 *
 * Returns 0 on success, 1 if already active
 */
int telemetry_start(enum telemetry_t type, int8_t length) {
  if (!telemetry_active()) {

    /* Initialise */
    telemetry_type = type;
    telemetry_index = 0;
    telemetry_string_length = length;

    /* Setup timer tick */
    switch(telemetry_type) {
    case TELEMETRY_RTTY:
      timer1_tick_init(RTTY_BIT_MS);
      break;
    case TELEMETRY_PIPS:
      timer1_tick_init(PIPS_RATE_MS);
      break;
    }
    return 0; /* Success */
  } else {
    return 1; /* Already active */
  }
}




uint8_t is_telemetry_finished(void) {
  if (telemetry_index >= telemetry_string_length) {
    /* All done, deactivate */
    telemetry_string_length = 0;

    /* Turn radio off */
    if (radio_on) {
      si_trx_off(); radio_on = 0;
    }

    /* De-init timer */
    timer1_tick_deinit();

    return 1;
  }
  return 0;
}
/**
 * Called at the telemetry mode's baud rate
 */
void telemetry_tick(void) {
  if (telemetry_active()) {
    switch (telemetry_type) {

    case TELEMETRY_RTTY: /* ---- ---- A character mode */

      if (!radio_on) {
        /* RTTY: We use the modem offset to modulate */
        si_trx_on(SI_MODEM_MOD_TYPE_CW, 1);
        radio_on = 1;
        rtty_preamble();
      }

      if (!rtty_tick()) {
        /* Transmission Finished */
        if (is_telemetry_finished()) return;

        /* Let's start again */
        uint8_t data = telemetry_string[telemetry_index];
        telemetry_index++;

        rtty_start(data);
      }

      break;

    case TELEMETRY_PIPS: /* ---- ---- A pips mode! */

      if (!radio_on) { /* Turn on */
        /* Pips: Cw */
        si_trx_on(SI_MODEM_MOD_TYPE_CW, 1);
        radio_on = 1;
        timer1_tick_time(PIPS_LENGTH_MS);

      } else { /* Turn off */
        si_trx_off();
        radio_on = 0;
        timer1_tick_time(PIPS_RATE_MS);

        telemetry_index++;
        if (is_telemetry_finished())
        
          return;
      }
    }
  }
}


/**
 * CLOCKING
 * =============================================================================
 */

/**
 * Initialises a timer interupt at the time period in ms
 *
 */
void timer1_tick_init(uint16_t millisecs)
{
 
     __disable_interrupt();
  /* Configure Timer 1 */
   
    TIM1_PSCRH = TIMER1_PRESCALE >> 8 ;       //  Prescaler 
    TIM1_PSCRL = TIMER1_PRESCALE & 0xff ;       //  
    
    TIM1_ARRH =millisecs >> 8 ;       //  Count Register
    TIM1_ARRL =millisecs & 0xff ;       //  
    
 
  /* Enable Interrupt */
    TIM1_IER_UIE = 1;       //  Enable the update interrupts.

  /* Enable Timer */
    TIM1_CR1_CEN = 1;       //  Enable the timer.
     __enable_interrupt();
}



/**
 * Changes the timer1 time
 */
void timer1_tick_time(uint16_t millisecs)
{
  
   
    TIM1_ARRH =millisecs >> 8 ;       //  Count Register
    TIM1_ARRL =millisecs & 0xff ;       //  
    
  
}


 /* Disables the timer
 */
void timer1_tick_deinit()
{
     __disable_interrupt();
    TIM1_CR1_CEN = 0;       //  Disable the timer.
    TIM1_IER_UIE = 0;       //  Disable the update interrupts.
     __enable_interrupt();
}
/**
 * Timer 1 Interrupt called at the symbol rate
 */
#pragma vector = TIM1_OVR_UIF_vector
__interrupt void TIM1_UPD_OVF_IRQHandler(void)
{
    telemetry_tick();
    TIM1_SR1_UIF = 0;               //  Reset the interrupt otherwise it will fire again   

}

