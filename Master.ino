/* 
Interfacing HC-05 (ZS-040) Bluetooth Module with Slave Arduino UNO using pin 2(Rx) and 3(Tx) and Contol the Fan connected to Motor using another Arduino UNO,
which acts as Master by www.playwithcircuit.com 

This is Master Arduino Code.
*/

// Include Soft Serial Library this library makes DIO pins as Serial Pins
#include <SoftwareSerial.h>
//Create software serial object to communicate with HC-05
// Now pin 2 and pin 3 of Arduino are Serial Rx & Tx pin Respectively
SoftwareSerial BTSerial(2, 3);

// Define MACROS related to communication with Master Board
#define START_CHAR '*'
#define END_CHAR '#'
#define MAX_BUFFFER 8


// Define the pins connected to buttons or POT to control the Motor connected with Slave
// button to Rotate motor in Clockwise Direction
#define ROTATE_CLOCKWISE 7
// button to Stop motor
#define ROTATE_STOP 6
// button to Rotate motor in Anti-Clockwise Direction
#define ROTATE_ANTI_CLOCKWISE 5
// Pot to control motor's Speed
#define ROTATE_SPEED A0
// LED to Indicate successfull communication
#define LED_GREEN 13
// State Pin to check if module is connected to Slave
// if state pin is high it means Master module is connected to Slave module
#define STATE_PIN 4

// Declare variables related to communication with Arduino Master
char serialInput;
int dataIndex = 0;
int rspSize = 0;
bool bo_cmd_ok = false;
// whether the string receiving is completed.
bool dataRcvd = false;
// To receive Raw response
char dataBuffer[MAX_BUFFFER] = { 0 };
// To save exact response
char rspBuffer[MAX_BUFFFER] = { 0 };

// Alive command
const char aliveCmd[MAX_BUFFFER] = { START_CHAR, 'A', 'L', 'I', 'V', 'E', END_CHAR, 0x00 };
// Standard OK response
const char stdRsp[5] = { START_CHAR, 'O', 'K', END_CHAR, 0x00 };


// Declare variable related to motor control
int motor_Speed = 0;
int motor_direction = 0;

// Function to clear bluetooth Buffer
void flushBTRcv();
// Function to receive Slave's Response
int checkResponse(void);

// Inline function to check if button is pressed packed with debouncing logic
inline bool chkButtonState(int pinNum, int checkState, int debounceDelay) {
  if (((digitalRead(pinNum) == checkState) ? true : false) == true) {
    delay(debounceDelay);
    return (((digitalRead(pinNum) == checkState) ? true : false) == true);
  } else {
    return false;
  }
}

void setup() {
  int retVal = 1;
  // Initialize motor control pins as INPUTS
  pinMode(ROTATE_CLOCKWISE, INPUT_PULLUP);
  pinMode(ROTATE_STOP, INPUT_PULLUP);
  pinMode(ROTATE_ANTI_CLOCKWISE, INPUT_PULLUP);
  pinMode(ROTATE_SPEED, INPUT);
  pinMode(STATE_PIN, INPUT_PULLUP);
  // Initialize Status LED as OUTPUT and Turn it OFF
  pinMode(LED_GREEN, OUTPUT);
  digitalWrite(LED_GREEN, LOW);
  // Begin the soft Serial port and set the data rate for the SoftwareSerial port at 9600 to communicate with Bluetooth Module in Data Mode
  BTSerial.begin(9600);
  // wait for state pin to turn HIGH else remain in this loop
  while(chkButtonState(STATE_PIN,LOW,0));    
  do {
    // provide a delay of 200 ms
    delay(200);
    //! Send Alive Command
    BTSerial.print(aliveCmd);
    retVal = checkResponse();
    if (retVal == 1) {
      // Turn Green OFF
      digitalWrite(LED_GREEN, LOW);
    } else {
      // Turn GREEN LED ON
      digitalWrite(LED_GREEN, HIGH);
      break;
    }
  } while (retVal);
}

void loop() {
  // variable to read POT input
  int potValue = 0;  
  // return value of function checkResponse() 
  int retVal = 1;
  // Read Motor Direction

  // check if Clockwise rotation button is pressed
  if (chkButtonState(ROTATE_CLOCKWISE, LOW, 20) == true) {
    motor_direction = 1;
  }
  // check if Anti-Clockwise rotation button is pressed
  else if (chkButtonState(ROTATE_ANTI_CLOCKWISE, LOW, 20) == true) {
    motor_direction = 2;
  }
  // check if Stop rotation button is pressed
  else if (chkButtonState(ROTATE_STOP, LOW, 20) == true) {
    motor_direction = 0;
  }

  // Read the value from the potentiometer
  potValue = analogRead(ROTATE_SPEED);
  motor_Speed = map(potValue, 0, 1023, 0, 255);

  //Send Motor Control command to Slave
  BTSerial.print(START_CHAR);
  BTSerial.print(motor_direction);
  BTSerial.print(',');
  BTSerial.print(motor_Speed);
  BTSerial.print(END_CHAR);

  // Check Reponse
  retVal = checkResponse();
  if (retVal == 1) {
    // Turn Green OFF
    digitalWrite(LED_GREEN, LOW);
  } else {
    // Turn GREEN LED ON
    digitalWrite(LED_GREEN, HIGH);
  }
  // Send next command after 100 ms
  delay(100);
}


// Flush Extra character from BT Rx buffer
void flushBTRcv() {
  char ret_char;
  while (BTSerial.available() > 0) {
    ret_char = BTSerial.read();
  }
}

// this function is used to Receive "*OK#" response from Slave
// If exact response is received it returns 0 else it returns 1
int checkResponse(void) {
  int retVal = 1;
  int counter = 0;
  delay(10);            // delay to get first character
  // Get Data From HC-05
  while (BTSerial.available()) {
    // get the new byte
    serialInput = BTSerial.read();
    dataBuffer[dataIndex++] = serialInput;
    // if the incoming character is a END_CHAR character , set a flag so the main loop can
    // do something about it
    if ((serialInput == END_CHAR) || (dataIndex == MAX_BUFFFER)) {
      dataIndex = 0;
      dataRcvd = true;
      flushBTRcv();
    }
    delay(10);              // 10 ms delay after receiving every character
    if (counter++ > 350) {  // this provides delay of 3500 ms or 3.5 seconds
      dataRcvd = false;
      dataIndex = 0;
      flushBTRcv();
      break;
    }
  }

  if (dataRcvd == true) {
    rspSize = 0;
    // Check for start and end character
    memset(rspBuffer, 0x00, sizeof(rspBuffer));

    // Extract command from RAW data sent
    for (int i = 0; i < MAX_BUFFFER; i++) {
      if (dataBuffer[i] == START_CHAR) {
        for (int j = 0; j < MAX_BUFFFER; j++) {
          rspBuffer[j] = dataBuffer[j + i];
          rspSize++;
          if (rspBuffer[j] == END_CHAR) {
            bo_cmd_ok = true;
            break;
          }
        }
      }
    }
    if (bo_cmd_ok == true) {
      //! check for Command's Response if its OK it means Slave is connected to Master
      if (memcmp(rspBuffer, stdRsp, 4) == 0) {
        // Reply OK
        retVal = 0;
      } else {
        retVal = 1;
      }
    } else {
      retVal = 1;
    }
    dataRcvd = false;
    bo_cmd_ok = false;
    memset(dataBuffer, 0x00, sizeof(dataBuffer));
  } else {
    retVal = 1;
  }

  // reset array and variables 
  memset(dataBuffer, 0x00, sizeof(dataBuffer));
  bo_cmd_ok = false;
  dataRcvd =false;

  // return result
  return retVal;
}
