#ifndef K64F_H_
#define K64F_H_

typedef enum color {off, red, green, blue} color_t;

Serial pc(USBTX, USBRX);
DigitalOut redLED(LED_RED);
DigitalOut greenLED(LED_GREEN);
DigitalOut blueLED(LED_BLUE);
InterruptIn switch2(SW2);
InterruptIn switch3(SW3);

#endif // K64F.H 