#include <msp430.h>
#include "libTimer.h"
#include "buzzer.h"
extern int check_pause_state();

#define LED_GREEN BIT0
#define LED_RED BIT6
#define LEDS (BIT0 | BIT6)

#define SW1 BIT3
#define SWITCHES SW1

#define BTN0 BIT0
#define BTN1 BIT1
#define BTN2 BIT2
#define BTN3 BIT3
#define ALL_BTNS (BTN0 | BTN1 | BTN2 | BTN3)

volatile char interrupted = 0;
enum State { PLAYING, INTERRUPTED, DONE };
enum State current_state = PLAYING;

void delay(unsigned int d) {
  while (d--) {
    __delay_cycles(2000);
  }
}

void play_white_noise() {
  buzzer_set_period(1000);   // Constant tone
  P1OUT |= LED_RED;          //Red light will turn on when button is pressed
  P1OUT &= ~LED_GREEN;       //Green light will turn off while button is pressed
}

void stop_white_noise() {
  buzzer_set_period(0);
  P1OUT &= ~LED_RED;         // stop the noise and continue with the melody then turn off red light 
}

int play_main_sequence(int startTone, int delayTime) {
  int tone = startTone;
  int i;
 // while system is not interrupted i < 4 because there will be 4 different tones
  for (i = 0; i < 4 && !interrupted; i++) {
    P1OUT |= LED_GREEN;     // for every tone turn led green light on
    buzzer_set_period(tone);
    delay(delayTime);

    P1OUT &= ~LED_GREEN;    //turn off green led after tone this will cause green light to flicker
    buzzer_set_period(0);
    delay(delayTime / 2);

    tone += 500;            // increase tone make it sound more squeaky
  }

  buzzer_set_period(0);
  P1OUT &= ~LED_GREEN;
  return i;
}

void switch_interrupt_handler() {
  char p1val = P1IN;
  P1IES |= (p1val & SWITCHES);
  P1IES &= (p1val | ~SWITCHES);

  if (!(p1val & SW1)) {
    interrupted = 1;
  } else {
    interrupted = 0;
  }
}

void __interrupt_vec(PORT1_VECTOR) Port_1() {
  if (P1IFG & SWITCHES) {
    P1IFG &= ~SWITCHES;
    switch_interrupt_handler();
  }
}

int main() {
  configureClocks();
  buzzer_init();

  P1DIR |= LEDS;
  P1OUT &= ~LEDS;

  P1REN |= SWITCHES;
  P1IE  |= SWITCHES;
  P1OUT |= SWITCHES;
  P1DIR &= ~SWITCHES;

  P2DIR &= ~ALL_BTNS;
  P2REN |= ALL_BTNS;
  P2OUT |= ALL_BTNS;

  or_sr(0x8); // Enable interrupts

  int totalRounds = 0;
  int toneStart = 2000;
  int delayTime = 1500;

  while (current_state != DONE) {
    char p2val = P2IN;

    if (!(p2val & BTN0)) {
      // P2.0 — hold to play white noise
      play_white_noise();
      while (!(P2IN & BTN0));
      stop_white_noise();

    } else if (!(p2val & BTN1)) {
      // P2.1 — press to pause (INTERRUPTED)
      current_state = INTERRUPTED;

    } else if (!(p2val & BTN2)) {
      // P2.2 — resume from pause
      if (current_state == INTERRUPTED) {
        current_state = PLAYING;
      }

    } else if (!(p2val & BTN3)) {
      // P2.3 — stop everything
      current_state = DONE;
    }

    switch (current_state) {
      case PLAYING:
        // melody will play for a total of 20 rounds 
        if (totalRounds >= 20) {
          current_state = DONE;
        } else {
          int played = play_main_sequence(toneStart, delayTime);
          if (played == 4) {
            totalRounds++;
            delayTime += 100; // Simulate getting softer
          }
        }
        break;

      case INTERRUPTED:
        P1OUT |= LED_RED;         // Red light will stay on during pause
        buzzer_set_period(0);     // no sound
        P1OUT &= ~LED_GREEN;      // turn off green light while paused
        break;

      case DONE:
        break;
    }

    if (interrupted && current_state == PLAYING) {
      current_state = INTERRUPTED;
    }
  }

  buzzer_set_period(0);
  P1OUT &= ~LEDS;
  or_sr(0x18); // CPU off
}
