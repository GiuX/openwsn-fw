/*
 * Driver for the IEEE802.15.4e timers.
 *
 * Authors:
 * Thomas Watteyne <watteyne@eecs.berkeley.edu>, August 2011
 *
 * These timers are dedicated to IEEE802.15.4e. IEEE802.15.4e uses TimerA,
 * clocked from the 32kHz crystal, in up mode, with interrupts enabled.
 * The counter resetting corresponds to the slot edge.
 *
 * IEEE802.15.4e only uses the following registers:
 * - CCR0 holds the slot duration; it will cause the counter to reset
 * - CCR1 is used in compare mode to time the MAC FSM
 * - CCR2 is used in capture mode to timestamp the arrival of a packet for
 *   synchronization.
 */

#include "msp430x26x.h"
#include "ieee154etimer.h"
#include "IEEE802154e.h"

//=========================== variables =======================================

//=========================== prototypes ======================================

//=========================== public ==========================================

/**
\brief Initialize the IEEE802.15.4e timer.
*/
void ieee154etimer_init() {
   
   // source ACLK from 32kHz crystal
   BCSCTL3 |= LFXT1S_0;
   
   // CCR0 contains max value of counter (slot length)
   TACCTL0  =  CCIE;
   TACCR0   =  TsSlotDuration;
   
   // CCR1 in compare mode
   TACCTL1  =  0;
   TACCR1   =  0;
   
   // CCR2 in capture mode
   TACCTL2  =  CAP+SCS+CCIS1+CM_1;
   TACCR2   =  0;
   
   //reset couter
   TAR      =  0;
   
   // start counting
   TACTL    =  MC_1+TBSSEL_1;                    // up mode, clocked from ACLK
}

//--- CCR1 compare timer

/**
\brief Schedule the timer to fire in the future.

Calling this function cancels all running timers

\param [in] offset The time at which to fire, relative to the current slot start
                   time, in 32-kHz ticks.
*/
void ieee154etimer_schedule(uint16_t offset) {
   // offset when to fire
   TACCR1   =  offset;
   // enable CCR1 interrupt (this also cancels any pending interrupts)
   TACCTL1  =  CCIE;
}

/**
\brief Cancel a timer.

This function has no effect if no timer is running.
*/
void ieee154etimer_cancel() {
   TACCR1   =  0;                                // reset CCR1 value (also resets interrupt flag)
   TACCTL1 &= ~CCIE;                             // disable CCR1 interrupt
}

//--- CCR2 capture timer

inline uint16_t ieee154etimer_getCapturedTime() {
   return TAR;
}

/*
void ieee154etimer_clearCaptureOverflow() {
   volatile uint16_t dummy;
   dummy    =  TACCR2;
   TACCTL2 &= ~COV;
   TACCTL2 &= ~CCIFG;
}

void ieee154etimer_enableCaptureInterrupt() {
   TACCTL2 |=  CCIE;
}

void ieee154etimer_getCapturedTime(timestamp_t* timestampToWrite) {
   uint8_t overflown;
   overflown = (TACCTL2 & 0x02) >> 1;
 
   // determine whether this timestampe is valid
   if (overflown==1) {
      timestampToWrite->valid   = 0;
   } else {
      timestampToWrite->valid   = 1;
   }
   
   // the actual timestamp
   timestampToWrite->timestamp = TACCR2;
}

void ieee154etimer_disableCaptureInterrupt() {
   TACCTL2 &= ~CCIE;
}

//=========================== private =========================================
