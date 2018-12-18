/*
* main.c  Main Code Module
* 
* Pico Balloon Tracker using HC12 radio module and GPS
* HC12 Module with STM8S003F3 processor and silabs Si4463 Radio
*  
* Derived Work Copyright (c) 2018 Imperial College Space Society
* From original work Copyright (C) 2014  Richard Meadows <richardeoin>
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

/*
Things to do 17 December 2018

make the program parse the gps coordinates
form the telemetry string
transmit the telemetry string
Try to reduce memory usage when sending the pubx strings to disable nmea
*/




#include <stdint.h>
#include <iostm8s003f3.h>
#include "HC12Board.h"
#include "si_trx.h"
#include "si_trx_defs.h"
#include "telemetry.h"
#include "energy.h"
#include "gps.h"
#include <intrinsics.h>
#include "main.h"


  /*
  Strategy:

  put into flight mode
  
  loop start
  get gps position and disable gps interrupts
  create telemetry string
  transmit telemetry string
  loop.
  */

void delay_ms(unsigned long ms) {
	//The best naive delay @16MHz
	//the 960 comes from the number of instructions to perform the do/while loop
	//to figure it out, have a look at the generated ASM file after compilation
	unsigned long cycles = 960 * ms;
	do
	{
		cycles--;
	}
	while(cycles > 0);
}


/*
 * the TX data buffer
 * contains ASCII data, which is either transmitted as CW oder RTTY
 */
uint16_t tx_buf_rdy = 0;			/* the read-flag (main -> main) */
uint16_t tx_buf_length = 0;			/* how many chars to send */
char tx_buf[TX_BUF_MAX_LENGTH] = {SYNC_PREFIX "$$" PAYLOAD_NAME ","};	/* the actual buffer */

/* current (latest) GPS fix and measurements */
struct gps_fix current_fix;

void get_fix_and_measurements(void) {
	gps_get_fix(&current_fix);
	//current_fix.temperature_int = get_die_temperature();
	//current_fix.voltage_bat = get_battery_voltage();
	//current_fix.voltage_sol = get_solar_voltage();
}



int main( void )
{
    // get the clock working
    __disable_interrupt();
    InitialiseSystemClock();    
    __enable_interrupt();
    
    // Start the UART
    InitialiseUART(); // set up the uart
    
    
    // start up the radio    
    //Initialise Si4060 interface 
    si_trx_init();
    


    //Initialise GPS    
    delay_ms(1000); // gps startup delay
    while(!(gps_disable_nmea_output()));
    //while(!(gps_set_gps_only()));
    while(!(gps_set_airborne_model()));
    while(!(gps_set_power_save()));
    while(!(gps_power_save(0)));
    while(!(gps_save_settings()));
        


    
    // start telemetry
    
    while (1)
    {
  
    telemetry_start(TELEMETRY_PIPS, 5);

    // Sleep Wait 
    while (telemetry_active());
    
    telemetry_start(TELEMETRY_RTTY, 20);

    // Sleep Wait 
    while (telemetry_active());
    
    
    }
    
    
}
          


