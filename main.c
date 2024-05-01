#include <stdbool.h>

#include <stdint.h>

#include <stdio.h>

#include <inttypes.h>

#include <string.h>

#include "inc/hw_ints.h"

#include "inc/hw_types.h"

#include "inc/hw_memmap.h"

#include "inc/tm4c123gh6pm.h"

#include "driverlib/interrupt.h"

#include "driverlib/sysctl.h"

#include "driverlib/gpio.h"

#include "driverlib/pin_map.h"

#include "driverlib/uart.h"

#include "utils/uartstdio.h"

#include "utils/uartstdio.c"

#include "inc/hw_uart.h"

#include "driverlib/pwm.h"

#include "driverlib/rom.h"

#include "inc/tm4c123gh6pm.h"

#include "driverlib/debug.h"

#include <stdlib.h>

#include "driverlib/timer.h"


#define FRECUENCIA 50
volatile double CARGA;
volatile uint32_t PWMCLOCK;
volatile uint8_t AJUSTE;
bool E_1 = false;
bool E_2 = false;
bool E_3 = false;
bool E_4 = false;
bool E_5 = false;
bool E_6 = false;
int i = 0;
int SERV1 = 19;
int SERV2 = 18;
bool DIR = false;
char TX[2];
char RX[2];

uint32_t ui32Status;

void TerminalText(char * array) {
  while ( * array) {
    UARTCharPut(UART3_BASE, * array);
    array++;
  }
}

void DelayMilisegundos(uint32_t milisegundos) {
  uint32_t periodo = (80000) * milisegundos;
  TimerLoadSet(TIMER0_BASE, TIMER_A, periodo);
  TimerEnable(TIMER0_BASE, TIMER_A);
  while (TimerValueGet(TIMER0_BASE, TIMER_A) < periodo - 1) {}
  TimerDisable(TIMER0_BASE, TIMER_A);
}
void Llevar(int PWMPARQUEO, int PWMNIVEL) {
  SERV2 = 18;
  for (SERV1 = 19; SERV1 <= PWMPARQUEO; SERV1++) {
    PWMOutputState(PWM0_BASE, PWM_OUT_6_BIT | PWM_OUT_7_BIT, false);
    SERV2++;
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_6, SERV1 * CARGA / 1000);
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_7, SERV2 * CARGA / 1000);
    PWMOutputState(PWM0_BASE, PWM_OUT_6_BIT | PWM_OUT_7_BIT, true);
    DelayMilisegundos(40);
  }
  PWMOutputState(PWM0_BASE, PWM_OUT_7_BIT, false);
  SERV2 = PWMNIVEL;
  PWMPulseWidthSet(PWM0_BASE, PWM_OUT_7, SERV2 * CARGA / 1000);
  PWMOutputState(PWM0_BASE, PWM_OUT_7_BIT, true);
  DelayMilisegundos(50);
}

void Traer(int PWMPARQUEO, int PWMNIVEL) {
  SERV2 = PWMNIVEL;
  for (SERV1 = PWMPARQUEO; SERV1 >= 19; SERV1--) {
    PWMOutputState(PWM0_BASE, PWM_OUT_6_BIT | PWM_OUT_7_BIT, false);
    SERV2--;
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_6, SERV1 * CARGA / 1000);
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_7, SERV2 * CARGA / 1000);
    PWMOutputState(PWM0_BASE, PWM_OUT_6_BIT | PWM_OUT_7_BIT, true);
    DelayMilisegundos(40);
  }
  PWMOutputState(PWM0_BASE, PWM_OUT_7_BIT, false);
  SERV2 = 18;
  PWMPulseWidthSet(PWM0_BASE, PWM_OUT_7, SERV2 * CARGA / 1000);
  PWMOutputState(PWM0_BASE, PWM_OUT_7_BIT, true);
  DelayMilisegundos(50);

}

void Out_In(char * array, uint32_t delay_value, uint32_t retardo_fin) {
  uint8_t pattern[4];
  if ( * array == 'O') {
    pattern[0] = 8;
    pattern[1] = 4;
    pattern[2] = 2;
    pattern[3] = 1;
  } else if ( * array == 'I') {
    pattern[0] = 1;
    pattern[1] = 2;
    pattern[2] = 4;
    pattern[3] = 8;
  }
  for (i = 0; i < 45; i++) {
    int j;
    for (j = 0; j < 4; j++) {
      GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, pattern[j]);
      DelayMilisegundos(delay_value);
    }
  }
  DelayMilisegundos(retardo_fin);
}

void Down_Up(char * array, uint32_t delay_value, uint32_t retardo_fin) {
  uint32_t updown;
  if ( * array == 'U') {
    updown = 32;
  } else if ( * array == 'D') {
    updown = 16;
  }
  GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5, updown);
  DelayMilisegundos(delay_value);
  GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5, 0);
  DelayMilisegundos(retardo_fin);
}

void UARTIntHandler(void) {
  ui32Status = UARTIntStatus(UART0_BASE, true);
  UARTIntClear(UART0_BASE, ui32Status);
  while (UARTCharsAvail(UART0_BASE)) {
    RX[0] = UARTCharGet(UART0_BASE);
    RX[1] = UARTCharGetNonBlocking(UART0_BASE);
    UARTCharPut(UART0_BASE, RX[0]);
    if (RX[0] == 'R') {
      E_1 = false;
      E_2 = false;
      E_3 = false;
      E_4 = false;
      E_5 = false;
      E_6 = false;
    }
    if (RX[0] == 'A') {
      Out_In("OUT", 8, 500);
      GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5, 0);
    }
    if (RX[0] == 'W') {
      Down_Up("UP", 1250, 500);
      GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5, 0);
    }
    if (RX[0] == 'S') {
      Down_Up("DOWN", 1250, 250);
      GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5, 0);
    }
    if (RX[0] == 'D') {
      Out_In("IN", 8, 500);
      GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5, 0);
    }

    if (RX[0] == '1') {
      if (E_1 == false) {
        Down_Up("DOWN", 1200, 250);
        Out_In("OUT", 8, 500);
        Down_Up("UP", 1200, 500);
        Out_In("IN", 16, 500);
        GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5, 0);
        Llevar(34, 33);
        DelayMilisegundos(800);
        Out_In("OUT", 16, 500);
        Down_Up("DOWN", 1200, 250);
        Out_In("IN", 8, 500);
        Down_Up("UP", 1200, 250);
        GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5, 0);
        Traer(34, 33);
        E_1 = true;
      } else {
        Llevar(34, 33);
        DelayMilisegundos(800);
        Down_Up("DOWN", 1200, 250);
        Out_In("OUT", 8, 500);
        Down_Up("UP", 1200, 500);
        Out_In("IN", 16, 500);
        GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5, 0);
        Traer(34, 33);
        DelayMilisegundos(800);
        Out_In("OUT", 8, 500);
        Down_Up("DOWN", 1200, 250);
        Out_In("IN", 16, 500);
        Down_Up("UP", 1200, 250);
        GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5, 0);
        E_1 = false;
      }
    }

    if (RX[0] == '2') {
      if (E_2 == false) {
        Down_Up("DOWN", 1200, 250);
        Out_In("OUT", 8, 500);
        Down_Up("UP", 1200, 500);
        Out_In("IN", 16, 500);
        GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5, 0);
        Llevar(49, 48);
        DelayMilisegundos(800);
        Out_In("OUT", 16, 500);
        Down_Up("DOWN", 1200, 250);
        Out_In("IN", 8, 500);
        Down_Up("UP", 1200, 250);
        GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5, 0);
        Traer(49, 48);
        E_2 = true;
      } else {
        Llevar(49, 48);
        DelayMilisegundos(800);
        Down_Up("DOWN", 1200, 250);
        Out_In("OUT", 8, 500);
        Down_Up("UP", 1200, 500);
        Out_In("IN", 16, 500);
        GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5, 0);
        Traer(49, 48);
        DelayMilisegundos(800);
        Out_In("OUT", 8, 500);
        Down_Up("DOWN", 1200, 250);
        Out_In("IN", 16, 500);
        Down_Up("UP", 1200, 250);
        GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5, 0);
        E_2 = false;
      }
    }

    if (RX[0] == '3') {
      if (E_3 == false) {
        Down_Up("DOWN", 1200, 250);
        Out_In("OUT", 8, 500);
        Down_Up("UP", 1200, 500);
        Out_In("IN", 16, 500);
        GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5, 0);
        Llevar(64, 63);
        DelayMilisegundos(800);
        Out_In("OUT", 16, 500);
        Down_Up("DOWN", 1200, 250);
        Out_In("IN", 8, 500);
        Down_Up("UP", 1200, 250);
        GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5, 0);
        Traer(64, 63);
        E_3 = true;
      } else {
        Llevar(64, 63);
        DelayMilisegundos(800);
        Down_Up("DOWN", 1200, 250);
        Out_In("OUT", 8, 500);
        Down_Up("UP", 1200, 500);
        Out_In("IN", 16, 500);
        GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5, 0);
        Traer(64, 63);
        DelayMilisegundos(800);
        Out_In("OUT", 8, 500);
        Down_Up("DOWN", 1200, 250);
        Out_In("IN", 16, 500);
        Down_Up("UP", 1200, 250);
        GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5, 0);
        E_3 = false;
      }
    }

    if (RX[0] == '4') {
      if (E_4 == false) {
        Down_Up("DOWN", 1200, 250);
        Out_In("OUT", 8, 500);
        Down_Up("UP", 1200, 500);
        Out_In("IN", 16, 500);
        GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5, 0);
        Llevar(79, 77);
        DelayMilisegundos(800);
        Out_In("OUT", 16, 500);
        Down_Up("DOWN", 1200, 250);
        Out_In("IN", 8, 500);
        Down_Up("UP", 1200, 250);
        GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5, 0);
        Traer(79, 77);
        E_4 = true;
      } else {
        Llevar(79, 77);
        DelayMilisegundos(800);
        Down_Up("DOWN", 1200, 250);
        Out_In("OUT", 8, 500);
        Down_Up("UP", 1200, 500);
        Out_In("IN", 16, 500);
        GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5, 0);
        Traer(79, 77);
        DelayMilisegundos(800);
        Out_In("OUT", 8, 500);
        Down_Up("DOWN", 1200, 250);
        Out_In("IN", 16, 500);
        Down_Up("UP", 1200, 250);
        GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5, 0);
        E_4 = false;
      }
    }
    if (RX[0] == '5') {
      if (E_5 == false) {
        Down_Up("DOWN", 1200, 250);
        Out_In("OUT", 8, 500);
        Down_Up("UP", 1200, 500);
        Out_In("IN", 16, 500);
        GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5, 0);
        Llevar(96, 93);
        DelayMilisegundos(800);
        Out_In("OUT", 16, 500);
        Down_Up("DOWN", 1200, 250);
        Out_In("IN", 8, 500);
        Down_Up("UP", 1200, 250);
        GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5, 0);
        Traer(96, 93);
        E_5 = true;
      } else {
        Llevar(96, 93);
        DelayMilisegundos(800);
        Down_Up("DOWN", 1200, 250);
        Out_In("OUT", 8, 500);
        Down_Up("UP", 1200, 500);
        Out_In("IN", 16, 500);
        GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5, 0);
        Traer(96, 93);
        DelayMilisegundos(800);
        Out_In("OUT", 8, 500);
        Down_Up("DOWN", 1200, 250);
        Out_In("IN", 16, 500);
        Down_Up("UP", 1200, 250);
        GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5, 0);
        E_5 = false;
      }
    }
    if (RX[0] == '6') {

      if (E_6 == false) {
        Down_Up("DOWN", 1200, 250);
        Out_In("OUT", 8, 500);
        Down_Up("UP", 1200, 500);
        Out_In("IN", 16, 500);
        GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5, 0);
        Llevar(109, 106);
        DelayMilisegundos(800);
        Out_In("OUT", 16, 500);
        Down_Up("DOWN", 1200, 250);
        Out_In("IN", 8, 500);
        Down_Up("UP", 1200, 250);
        GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5, 0);
        Traer(109, 106);
        E_6 = true;
      } else {
        Llevar(109, 106);
        DelayMilisegundos(800);
        Down_Up("DOWN", 1200, 250);
        Out_In("OUT", 8, 500);
        Down_Up("UP", 1200, 500);
        Out_In("IN", 16, 500);
        GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5, 0);
        Traer(109, 106);
        DelayMilisegundos(800);
        Out_In("OUT", 8, 500);
        Down_Up("DOWN", 1200, 250);
        Out_In("IN", 16, 500);
        Down_Up("UP", 1200, 250);
        GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5, 0);
        E_6 = false;
      }

    }
  }
}
void PWMConfig(void) {
  SysCtlPWMClockSet(SYSCTL_PWMDIV_64);
  PWMCLOCK = SysCtlClockGet() / 64;
  CARGA = (PWMCLOCK / FRECUENCIA) - 1;
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);
  GPIOPinTypePWM(GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5);
  GPIOPinConfigure(GPIO_PC4_M0PWM6);
  GPIOPinConfigure(GPIO_PC5_M0PWM7);

  PWMGenConfigure(PWM0_BASE, PWM_GEN_3, PWM_GEN_MODE_NO_SYNC);
  PWMGenPeriodSet(PWM0_BASE, PWM_GEN_3, CARGA);
  PWMGenEnable(PWM0_BASE, PWM_GEN_3);
  PWMOutputState(PWM0_BASE, PWM_OUT_6_BIT | PWM_OUT_7_BIT, true);

  PWMPulseWidthSet(PWM0_BASE, PWM_OUT_6, 19 * CARGA / 1000);
  PWMPulseWidthSet(PWM0_BASE, PWM_OUT_7, 18 * CARGA / 1000);

}
int main(void) {
  SysCtlClockSet(SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_SYSDIV_2_5);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
  TimerConfigure(TIMER0_BASE, TIMER_CFG_ONE_SHOT);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
  GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
  GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
  GPIOPinConfigure(GPIO_PA0_U0RX);
  GPIOPinConfigure(GPIO_PA1_U0TX);
  UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 9600, (UART_CONFIG_WLEN_8 | UART_CONFIG_PAR_NONE | UART_CONFIG_STOP_ONE));

  IntMasterEnable();
  IntEnable(INT_UART0);
  UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);
  UARTEnable(UART0_BASE);
  //PARA VERIFICAR LA CONEXION BLUETOOTH
  UARTCharPut(UART0_BASE, 'A');
  UARTCharPut(UART0_BASE, '\r');
  UARTCharPut(UART0_BASE, '\n');
  GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5, 0);
  PWMConfig();
  while (1) {

  }
}
