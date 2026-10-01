# Wireless Communication Between Two Arduino Boards Using HC-05 Bluetooth Modules
![Wireless Communication Between Two Arduino Boards Using HC-05 Bluetooth Modules](https://playwithcircuit.com/wp-content/uploads/2024/09/Master-and-Slave-communication-between-two-Arduino-Boards-using-HC-05-Bluetooth-Modules.webp)

This project demonstrates how to establish wireless serial communication between two Arduino UNO boards using two HC-05 Bluetooth modules.

One HC-05 is configured as the **Master**, while the other is configured as the **Slave**. After pairing the modules, the Master Arduino sends commands to the Slave Arduino over Bluetooth. In this example, the Slave controls a 12V DC motor according to commands received from the Master.

The Master Arduino uses three push buttons to control the motor direction and stop operation, while a potentiometer controls the motor speed.

## Project Overview

HC-05 Bluetooth modules normally operate as serial UART devices. For communication between two Arduino boards, one module needs to operate as a Master and the other as a Slave.

The communication flow used in this project is:

```text
Master Arduino
     |
     | UART
     v
Master HC-05
     |
     | Bluetooth
     v
Slave HC-05
     |
     | UART
     v
Slave Arduino
     |
     v
L293D Motor Driver
     |
     v
12V DC Motor
```

The Master sends commands containing the motor direction and speed. The Slave receives these commands, controls the motor through an L293D motor driver, and sends a response back to the Master.

## Features

- Wireless communication between two Arduino UNO boards
- HC-05 Master-Slave Bluetooth configuration
- HC-05 configuration using AT commands
- Bidirectional UART communication
- Motor direction control
- Motor speed control using PWM
- Command/response communication protocol
- Connection-status indication using the Arduino onboard LED

## Hardware Requirements

- Arduino UNO R3 × 2
- HC-05 Bluetooth Module × 2
  - Modules should have an AT-mode button and 6 pins
- 1 kΩ resistors × 2
- 2 kΩ resistors × 2
- L293D motor driver IC × 1
- Breadboard × 2
- USB Type-A to Type-B cable
- Jumper wires
- 12V power supplies
- 12V DC fan or DC motor
- Push buttons × 3
- 10 kΩ potentiometer × 1

## Software Requirements

- Arduino IDE 2.1.1 or later
- SoftwareSerial library

---

# 1. Configuring the HC-05 Bluetooth Modules

Before using the modules for Arduino-to-Arduino communication, configure one HC-05 as the Master and the other as the Slave.

The configuration is performed using the HC-05 AT command mode.

The HC-05 communicates with the Arduino through UART. Since the Arduino UNO's hardware serial interface is normally used for USB communication and programming, pins 2 and 3 are used as a software serial interface.

### HC-05 to Arduino UNO UART Connection

Use:

- Arduino pin 2 → HC-05 TXD
- Arduino pin 3 → HC-05 RXD through a voltage divider

The HC-05 RX pin should not be driven directly from the Arduino's 5V TX signal. A voltage divider using 1 kΩ and 2 kΩ resistors is used to reduce the voltage.

The HC-05 TXD pin can be connected directly to Arduino pin 2.

Connect:

```text
HC-05 VCC → Arduino 5V
HC-05 GND → Arduino GND
HC-05 TXD → Arduino Pin 2
HC-05 RXD → Arduino Pin 3 through voltage divider
```

> **Important:** HC-05 breakout boards vary. Check the documentation for your particular module before applying power or assuming the logic-level specifications.

---

# 2. Putting the HC-05 into AT Command Mode

The HC-05 must be placed in command mode before changing its role or other configuration parameters.

Follow these steps:

1. Connect the HC-05 to the Arduino UNO according to the UART wiring described above.
2. Upload the Arduino serial communication sketch to the UNO.
3. Disconnect the USB cable from the PC.
4. Open the Arduino IDE Serial Monitor.
5. Set the Serial Monitor baud rate to **9600** and configure the line ending as **Both NL & CR**.
6. Press and hold the button on the HC-05 module.
7. While holding the button, connect the Arduino to the PC using the USB cable.
8. Continue holding the button until the module enters AT command mode.

The HC-05 should indicate that it has entered command mode. If it does not, repeat the procedure.

Once command mode is active, AT commands can be sent through the Arduino Serial Monitor.

---

# 3. Configure the Slave HC-05

The HC-05 is commonly supplied in the Slave role by default. The typical default parameters are:

```text
Baud Rate : 9600 bps
Data      : 8 bits
Stop Bits : 1
Parity    : None
Handshake : None
Passkey   : 1234 or 0000
Name      : HC-05
```

With the module in AT command mode, send the following commands.

### Test Communication

Send:

```text
AT
```

Expected response:

```text
OK
```

This confirms that the Arduino and HC-05 are communicating correctly.

### Change the Module Name

Send:

```text
AT+NAME=SLAVE
```

Expected response:

```text
OK
```

### Set the Role to Slave

Send:

```text
AT+ROLE=0
```

Expected response:

```text
OK
```

### Read the Bluetooth Address

Send:

```text
AT+ADDR?
```

The HC-05 returns its Bluetooth address.

For example:

```text
+ADDR:21:13:3B:B60
```

The exact response format can vary between HC-05 firmware versions.

**Record the Slave module's address.** This address will be required when configuring the Master module.

After configuration, disconnect the Slave module from the PC and remove it from the configuration circuit.

---

# 4. Configure the Master HC-05

Connect the second HC-05 module to the Arduino UNO and place it into AT command mode using the same procedure.

### Test Communication

Send:

```text
AT
```

Expected response:

```text
OK
```

### Change the Module Name

Send:

```text
AT+NAME=MASTER
```

Expected response:

```text
OK
```

### Set the Role to Master

Send:

```text
AT+ROLE=1
```

Expected response:

```text
OK
```

### Set Fixed Connection Mode

Send:

```text
AT+CMODE=0
```

Expected response:

```text
OK
```

This configures the Master to connect to the specific Bluetooth address stored using the `BIND` command.

### Bind the Master to the Slave

Use the Slave module's address obtained with `AT+ADDR?`.

For example:

```text
AT+BIND=0021,13,03BB60
```

Here, the address is formatted for the `BIND` command.

The exact address must be replaced with the address returned by your Slave module.

After sending the command, verify the stored address with:

```text
AT+BIND?
```

If the returned address is incorrect or all zeroes, repeat the `AT+BIND` command.

> **Note:** HC-05 firmware variants can use slightly different address formatting. If your module reports the address in a different format, follow the formatting expected by that firmware.

After successful configuration, disconnect the Master module from the PC.

---

# 5. HC-05 AT Commands Used

The main commands required for this project are:

```text
AT
AT+NAME=SLAVE
AT+ROLE=0
AT+ADDR?
```

for the Slave, and:

```text
AT
AT+NAME=MASTER
AT+ROLE=1
AT+CMODE=0
AT+BIND=<SLAVE_ADDRESS>
AT+BIND?
```

for the Master.

The exact AT command set depends on the HC-05 firmware. Some HC-05 clones and firmware variants may support different commands or command syntax.

---

# 6. Master-Slave Arduino Communication

After configuring both Bluetooth modules, they can be connected to their respective Arduino boards.

The Master Arduino sends commands to the Slave Arduino through the Bluetooth link.

The Slave Arduino interprets the received commands and controls the motor accordingly.

In this project:

```text
Master Arduino
    |
    +-- Button 1 → Clockwise
    |
    +-- Button 2 → Stop
    |
    +-- Button 3 → Anticlockwise
    |
    +-- Potentiometer → Motor Speed
    |
    v
Master HC-05
    |
    | Bluetooth
    |
    v
Slave HC-05
    |
    v
Slave Arduino
    |
    v
L293D
    |
    v
12V DC Motor
```

The Slave also sends a response to the Master after receiving commands.

---

# 7. Communication Protocol

A simple command/response protocol is used to make the communication between the two Arduino boards more reliable.

## Alive Command

The Master periodically sends an Alive command to determine whether the Slave is connected.

Command:

```text
*ALIVE#
```

Response:

```text
*OK#
```

The `*` character marks the beginning of the message and `#` marks the end.

## Motor Control Command

Motor commands are transmitted in the following format:

```text
*DIRECTION_BYTE,SPEED_BYTE#
```

For example:

```text
*1,255#
```

Here:

- `1` represents the motor direction
- `255` represents the motor speed
- `*` indicates the start of the command
- `#` indicates the end of the command

The Slave responds with:

```text
*OK#
```

after successfully receiving and processing the command.

The exact direction byte values depend on the Arduino code used in the project.

---

# 8. Master Arduino Circuit

![Master Arduino Circuit](https://playwithcircuit.com/wp-content/uploads/2024/09/Wiring-Diagram-of-Master-Arduino.webp)


The Master Arduino contains the user controls.

The HC-05 provides the wireless communication, while the potentiometer and push buttons generate the motor-control commands.

### Master Connections

The HC-05 is connected to the Arduino using the same UART arrangement described earlier:

```text
Arduino Pin 2 → HC-05 TXD
Arduino Pin 3 → HC-05 RXD through 1kΩ/2kΩ voltage divider
HC-05 VCC → 5V
HC-05 GND → GND
```

The potentiometer is connected to:

```text
Potentiometer wiper → Arduino A0
```

The three push buttons are connected to:

```text
Button 1 → Arduino Pin 7
Button 2 → Arduino Pin 6
Button 3 → Arduino Pin 5
```

Their functions are:

```text
Pin 7 → Clockwise rotation
Pin 6 → Motor stop
Pin 5 → Anticlockwise rotation
```

The Arduino onboard LED on pin 13 is used as a connection-status indicator.

### Master LED Status

The onboard LED indicates the Bluetooth connection state:

- **LED ON:** Master is successfully connected to the Slave.
- **LED OFF:** Master is disconnected from the Slave.
- **LED blinking:** Communication/frame errors or dropped commands may be occurring.

If the LED indicates communication problems, reduce the distance between the Master and Slave modules and check the power supply and wiring.

---

# 9. Slave Arduino Circuit
![Slave Arduino Circuit](https://playwithcircuit.com/wp-content/uploads/2024/09/Wiring-Diagram-of-Slave-Arduino.png)

The Slave Arduino receives commands from the HC-05 and controls a 12V DC motor through an L293D motor driver.

The Bluetooth connections remain:

```text
Arduino Pin 2 → HC-05 TXD
Arduino Pin 3 → HC-05 RXD through voltage divider
HC-05 VCC → 5V
HC-05 GND → GND
```

## L293D Connections

The L293D is used to drive the 12V DC motor.

Connect:

```text
L293D Pin 14 → VCC
L293D Pins 4 and 5 → Arduino GND
Arduino Pin 6 → L293D Pin 1 (Enable)
Arduino Pin 8 → L293D Input Pin 2
Arduino Pin 9 → L293D Input Pin 1
```

Connect the motor to:

```text
Motor → L293D Pins 3 and 6
```

The 12V motor supply is connected to the L293D motor-supply input:

```text
12V positive → L293D Pin 8
12V negative → GND at L293D Pins 4 and 5
```

The Arduino pin assignments are therefore:

```text
Arduino Pin 6 → L293D Enable
Arduino Pin 8 → L293D Input 2
Arduino Pin 9 → L293D Input 1
```

The Arduino generates PWM on pin 6 to control the motor speed.

Pins 8 and 9 determine the motor direction.

---

# 10. Power Supply Considerations

Use separate power supplies for the Arduino/Bluetooth circuitry and the 12V motor circuit.

A DC motor or fan can draw significantly more current as its operating conditions change. Motor current can introduce voltage fluctuations and electrical noise that may interfere with the Arduino or Bluetooth module.

For reliable operation:

- Power the Arduino and HC-05 from a suitable regulated supply.
- Provide the required 12V supply to the L293D motor circuit.
- Connect the grounds of the relevant power supplies together so that the Arduino and motor-driver signals have a common reference.
- Keep motor wiring away from sensitive signal wiring where practical.

> **Important:** Do not connect a 12V supply directly to the Arduino 5V pin or the HC-05 VCC pin.

---

# 11. How the Project Works

The Slave circuit should be powered before the Master circuit because the Master HC-05 is configured to connect to the specific address of the Slave.

When both circuits are powered:

1. The Slave HC-05 becomes available for connection.
2. The Master HC-05 searches for the configured Slave address.
3. The Bluetooth modules establish the connection.
4. The Master Arduino detects the connection.
5. The Master sends an Alive command.
6. The Slave responds with `*OK#`.
7. The Master reads the push buttons and potentiometer.
8. The corresponding motor command is generated.
9. The command is transmitted wirelessly to the Slave.
10. The Slave parses the command.
11. The Arduino controls the L293D according to the direction and speed values.
12. The Slave sends an acknowledgement to the Master.

This creates a simple bidirectional wireless control system.

---

# 12. Example Motor Control

The three push buttons provide the basic motor commands.

### Clockwise

Pressing the clockwise button causes the Master to send a command containing the corresponding direction value and the current speed value.

Example:

```text
*1,255#
```

### Stop

Pressing the stop button causes the Slave to stop the motor.

### Anticlockwise

Pressing the anticlockwise button sends the opposite direction command to the Slave.

### Speed Control

The potentiometer connected to A0 provides an analog value between 0 and 1023.

The Master Arduino maps this value to the motor-control range used by the Slave, typically:

```text
0–255
```

The resulting value is transmitted as the speed byte.

For example:

```text
*1,128#
```

represents a direction command with a speed value of 128.

---

# 13. Troubleshooting

### HC-05 does not enter AT mode

Check that the module has an AT-mode button and follow the correct power-up sequence. The timing and button behavior can vary between HC-05 breakout boards.

### `AT` does not return `OK`

Check:

- RX/TX wiring
- Baud rate
- Serial Monitor line ending
- AT-mode status
- SoftwareSerial configuration

Remember that:

```text
Arduino TX → HC-05 RX
Arduino RX → HC-05 TX
```

### Master does not connect to Slave

Verify:

- Slave address obtained using `AT+ADDR?`
- Master `AT+BIND` value
- Master role is `1`
- Slave role is `0`
- Master `AT+CMODE=0`
- Both modules are powered
- Slave is powered before the Master
- Modules are within Bluetooth range

### Bluetooth connection repeatedly drops

Check the power supplies first. Motor noise and voltage fluctuations can affect the Arduino and Bluetooth module.

Also verify that the Master and Slave share a common ground where required and that the motor has an appropriate separate supply.

### Motor does not rotate

Check:

- L293D power connections
- Motor connections
- Arduino pins 6, 8, and 9
- Motor-driver enable signal
- 12V motor supply
- Received command format
- Direction and speed values
---

# 15. Original Tutorial

This GitHub project is based on the detailed tutorial published on **Play with Circuit**.

For the complete explanation, schematics, configuration procedure, and additional reference material, see:

**Wireless Communication between two Arduino Boards using HC-05 Bluetooth Modules**

https://playwithcircuit.com/master-slave-communication-between-two-arduino-boards/

---

# 16. Applications

The same Master-Slave Bluetooth architecture can be adapted for many Arduino projects, including:

- Wireless motor control
- Robot control
- Remote actuator control
- Wireless sensor nodes
- Home automation prototypes
- Industrial control prototypes
- Remote monitoring systems
- Arduino-to-Arduino data transfer
- Wireless relay control
- Educational Bluetooth communication projects

The command protocol can also be extended to transmit sensor readings, actuator states, alarms, and other application-specific data.

---

# 17. Key Takeaways

This project demonstrates how two Arduino UNO boards can communicate wirelessly using HC-05 Bluetooth modules.

The important steps are:

1. Configure one HC-05 as a Slave.
2. Configure the second HC-05 as a Master.
3. Obtain the Slave Bluetooth address.
4. Bind the Master to the Slave address.
5. Connect the HC-05 modules to the Arduino boards through UART.
6. Establish a simple command/response protocol.
7. Use the Master Arduino to generate control commands.
8. Process the commands on the Slave Arduino.
9. Control the motor through the L293D motor driver.

Once this basic communication framework is working, the same approach can be extended to more complex wireless Arduino systems.
