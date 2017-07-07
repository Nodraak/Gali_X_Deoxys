
# Pinout

Here is some very crude diagram showing the pinout.


Some explanation so you are not (too) lost:

* Xbee: wireless transmitter used for remote debug
* Can: bus connecting all microcontrollers
* BRK, Dir, Pwm: H Bridge (LMD18200) pins
* Pwm: (cheap) servo controlled with PWM
* Ax12/Mx28: (high end) servo controlled with a proprietary UART-like bus
* SRF04/SRF05: ultra sound distance (proximity) sensor


## Boards

### CQB

Heavily used board. Encoders should used hardware counter or maybe another board.

```text
                     +---------+
        +------------+   USB   +------------+
        |            +---------+            |
        |                                   |
(Xbee)  | D1 Serial Tx                  Vin |
(Xbee)  | D0 Serial Rx                  GND |
        | x                               x |
        | GND                             x |
Can Td  | D2                             A7 |
L Enc 1 | D3 PB0                         A6 | R BRK
        | D4                             A5 | R Pwm?
L BRK   | D5 Pwm16                       A4 |
        | D6 Pwm1                        A3 |
        | D7                         P13 A2 | R Enc 2
        | D8                         PA1 A1 | R Enc 1
L Pwm   | D9 Pwm1                        A0 |
Can Rd  | D10                             x |
L Dir   | D11                          3.3V |
L Enc 2 | D12 PB4                       D13 | R Dir
        |                                   |
        |            +---------+            |
        +------------+ Button  +------------+
                     +---------+
```


### CQR

Used much less than planned. It was basically doing nothing.

```text
                     +---------+
        +------------+   USB   +------------+
        |            +---------+            |
        |                                   |
Xbee    | D1 Serial Tx                  Vin |
Xbee    | D0 Serial Rx                  GND |
        | x                               x |
        | GND                             x |
Can Td  | D2                             A7 | micro sw 4
        | D3 PB0                         A6 | micro sw 3
        | D4                             A5 | micro sw 2
        | D5 Pwm16                       A4 | micro sw 1
        | D6 Pwm1                        A3 |
        | D7                         P13 A2 | B (buzzer/button/tirette)
        | D8                         PA1 A1 |
        | D9 Pwm1                        A0 |
Can Rd  | D10                             x |
        | D11                          3.3V |
        | D12 PB4                       D13 |
        |                                   |
        |            +---------+            |
        +------------+ Button  +------------+
                     +---------+
```


### CQES

Input/output are a mess. Use buses, it saves *a lot* of wires (I2C, ...).

```text
                                            USB
                                             //
 Ax12Tx | PC10 S3Tx      S3Rx PC11 | Ax12Rx  // R Clamp (pwm1) | PC9 pwm8         digi PC8 | R Pump (digi)
 SRF4 T | PC12                 PD2 |         //  R Flap (pwm2) | PB8 pwm4         digi PC6 | L Pump (digi)
        | x                      x |         //  R Prog (pwm3) | PB9 pwm4              PC5 | switch1
        | x                    GND |         //                | x                       x |
        | x                      x |         //                | GND                     x |
        | x                      x |         //            C1b | PA5            CanTD PA12 | Can Td
 SRF1 T | PA13                   x |         //            C2a | PA6            CanRD PA11 | Can Rd
        | PA14                 3.3 |         //            C2b | PA7                  PB12 | switch2
 SRF2 T | PA15                   5 |         // L Clamp (pwm4) | PB6 pwm4             PB11 | switch3
        | GND                  GND |         //  L Flap (pwm5) | PC7 pwm8                x |
    IR1 | PB7                  GND |         //  L Prog (pwm6) | PA9 pwm1              PB2 | switch4
 SRF3 T | PC13                 Vin | 5V      //           pwm7 | PA8                   PB1 | pwm
    IR2 | PC14                   x |         //           pwm8 | PB10                 PB15 | pwm
~SRF4 T~| PC15          AnagIn PA0 | S1      //         pwm C2 | PB4                  PB14 | laser1
 SRF1 E | PH0           AnagIn PA1 | S2      //         pwm C1 | PB5                  PB13 | laser2
 SRF2 E | PH1           AnagIn PA4 | S3      //                | PB3                     x |
        | x             AnagIn PB0 | S4      //        Ax12 sw | PA10                  PC4 | pwm
 SRF3 E | PC2           AnagIn PC1 | S5      //         (Xbee) | PA2 S2Tx                x |
 SRF4 E | PC3           AnagIn PC0 | C1a     //         (Xbee) | PA3 S2Rx                x |
                                             //
```


## Wiring

### MCP 2551 (CAN Transciever)

```text
                  +----\   /----+
                  |     ---     |
to uc ---- CAN TD | Tx       Rs | GND
              GND | VSS   CAN H | ----------- to CAN bus
               5V | VDD   CAN L | ----------- to CAN bus
to uc ---- CAN RD | Rx     VRef | floating
                  |             |
                  +-------------+
```


### Mosfet (Pump)

```text
          +-------+
         /    O    \
        |           |
        +-----------+
        |           |
        |           |
        | G   D   S |
        +-----------+
          ||  ||  ||
          ||  ||  ||
          ++  ++  ++
          |   |   \
          |   |     gnd
          |   \
          |     out
          |     charge -
          |     (charge + sur 10v)
          |     diode de out (- de charge) vers + de charge
          \
            data
            pulldown

```

### Ax12 Stuff

         stm
  5V Sw Rx Tx
 +----------------------+
 )                      |
 +----------------------+
  Sw Ax12 Ax12         gnd

-> pullup on Sw + Rx
