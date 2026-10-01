/* 
Interfacing HC-05 (ZS-040) Bluetooth Module with Slave Arduino UNO using pin 2(Rx) and 3(Tx) and Contol the Fan connected to Motor using another Arduino UNO,
which acts as Master by www.playwithcircuit.com 

This is Slave Arduino Code.
*/

// Include Soft Serial Library this library makes DIO pins as Serial Pins
#include <SoftwareSerial.h>
//Create software serial object to communicate with HC-05
SoftwareSerial BTSerial(2,3);  // Now pin 2 and pin 3 of Arduino are Serial Rx & Tx pin Respectively

// Define MACROS related to communication with Master Arduino UNO
#define START_CHAR '*'
#define END_CHAR '#'
#define MAX_BUFFFER 14
#define MAX_INDEX 2

#define MOTOR_DIRECTION_INDEX 0
#define MOTOR_SPEED_INDEX 1


// Define the pins connected to the L293D IC
#define MOTOR_EN 6   // Enable pin for Motor A
#define MOTOR_IN1 9  // Input 1 for Motor A
#define MOTOR_IN2 8  // Input 2 for Motor A

// Declare variables related to communication with Android app
char serialInput;
int dataIndex = 0;
char databuffer[MAX_BUFFFER] = { 0 };
char cmdbuffer[MAX_BUFFFER] = { 0 };
bool dataRcvd = false;  // whether the string receiving is completed.

char char_array[MAX_INDEX][4] = { 0 };
int int_array[MAX_INDEX] = { 0 };

int index;
int cmdsize = 0;
bool bo_cmd_ok = false;
int row = 0;
int row_index = 0;

// Alive command
const char aliveCmd[8] = { START_CHAR, 'A', 'L', 'I', 'V', 'E', END_CHAR, 0x00 };
// Standard OK response
const char stdRsp[5] = { START_CHAR, 'O', 'K', END_CHAR, 0x00 };

// Declare variable related to motor control
int motorSpeed = 0;
char previous_direction = 0;  // to store previous direction
bool one_time_flag = true;

int AsciitoInt(char* char_array);
void flushBTRcv();


void setup() {
  // Initialize motor control pins as outputs
  pinMode(MOTOR_EN, OUTPUT);
  pinMode(MOTOR_IN1, OUTPUT);
  pinMode(MOTOR_IN2, OUTPUT);
  // Begin the soft Serial port and set the data rate for the SoftwareSerial port at 9600 to communicate with Bluetooth Module in Data Mode
  BTSerial.begin(9600);
  // provide stability delay of 200 ms
  delay(200);
  // Set initial motor speed to zero
  analogWrite(MOTOR_EN, 0);
}

void loop() {
  // Get Data From HC-05
  while (BTSerial.available()) {
    // get the new byte
    serialInput = BTSerial.read();
    databuffer[dataIndex++] = serialInput;
    delay(10); // delay of 10 ms to get next character
    // if the incoming character is a END_CHAR character or databuffer is full
    // set a flag to true so that buffer shall not over flow
    if ((serialInput == END_CHAR) || (dataIndex == MAX_BUFFFER)) {
      dataIndex = 0;
      dataRcvd = true;
      flushBTRcv();
    }
  }

  if (dataRcvd == true) {
    cmdsize = 0;
    // Check for start and end character
    memset(cmdbuffer, 0x00, sizeof(cmdbuffer));

    // Extract command from RAW data sent
    for (int i = 0; i < MAX_BUFFFER; i++) {
      if (databuffer[i] == START_CHAR) {
        for (int j = 0; j < MAX_BUFFFER; j++) {
          cmdbuffer[j] = databuffer[j + i];
          cmdsize++;
          if (cmdbuffer[j] == END_CHAR) {
            bo_cmd_ok = true;
            break;
          }
        }
      }
      if (bo_cmd_ok == true) {
        break;
      }
    }
    dataRcvd = false;
    memset(databuffer, 0x00, sizeof(databuffer));

    if (bo_cmd_ok == true) {
      //! check for Alive Command
      if (memcmp(cmdbuffer, aliveCmd, 7) == 0) {
        // Reply OK
        BTSerial.print(stdRsp);
        bo_cmd_ok = false;
      }
    }
  }

  // if commmand is succesfully extracted from data buffer and saved in command buffer
  if (bo_cmd_ok == true) {
    // Reset all variables and array
    index = 0;
    row = 0;
    row_index = 0;
    bo_cmd_ok = false;
    memset(char_array, 0x00, sizeof(char_array));
    memset(int_array, 0x00, sizeof(int_array));

    // as index 0 is the start character hence it is incremented by 1
    index++;

    // save two different command in character array one is motor status and another is motor speed
    for (; index < cmdsize; index++) {
      if (cmdbuffer[index] == ',' || cmdbuffer[index] == '#') {
        row++;
        row_index = 0;
        continue;
      } else {
        char_array[row][row_index++] = cmdbuffer[index];
      }
    }

    for (int i = 0; i < MAX_INDEX; i++) {
      int_array[i] = AsciitoInt(&char_array[i][0]);
    }

    // Fan Motor Control
    if (int_array[MOTOR_DIRECTION_INDEX] == 1) {  // Clock-Wise case
      // this is done when suddendly direction changes then due to inertia it should not break
      if (previous_direction == 2) {
        digitalWrite(MOTOR_IN1, LOW);
        digitalWrite(MOTOR_IN2, LOW);
        analogWrite(MOTOR_EN, 0);
        delay(1000);
      }
      motorSpeed = map(int_array[MOTOR_SPEED_INDEX], 0, 255, 20, 255);
      digitalWrite(MOTOR_IN1, HIGH);
      digitalWrite(MOTOR_IN2, LOW);
      analogWrite(MOTOR_EN, motorSpeed);
      one_time_flag = true;
      previous_direction = int_array[MOTOR_DIRECTION_INDEX];

      // Reply OK
      BTSerial.print(stdRsp);
    } else if (int_array[MOTOR_DIRECTION_INDEX] == 2) {  // Anti Clock-Wise case
      // this is done when suddendly direction changes then due to inertia it should not break
      if (previous_direction == 1) {
        digitalWrite(MOTOR_IN1, LOW);
        digitalWrite(MOTOR_IN2, LOW);
        analogWrite(MOTOR_EN, 0);
        delay(1000);
      }
      motorSpeed = map(int_array[MOTOR_SPEED_INDEX], 0, 255, 20, 255);
      digitalWrite(MOTOR_IN1, LOW);
      digitalWrite(MOTOR_IN2, HIGH);
      analogWrite(MOTOR_EN, motorSpeed);
      one_time_flag = true;
      previous_direction = int_array[MOTOR_DIRECTION_INDEX];

      // Reply OK
      BTSerial.print(stdRsp);
    } else if (int_array[MOTOR_DIRECTION_INDEX] == 0) {  // Stop case
      digitalWrite(MOTOR_IN1, LOW);
      digitalWrite(MOTOR_IN2, LOW);
      analogWrite(MOTOR_EN, 0);
      if (one_time_flag == true) {
        delay(1000);
        one_time_flag = false;
      }
      previous_direction = int_array[MOTOR_DIRECTION_INDEX];

      // Reply OK
      BTSerial.print(stdRsp);
    } else {
      // do nothing in case of wrong value and do not reply
    }
  }
}

// Convert Ascii character received into integer
int AsciitoInt(char* char_array) {
  int ret_val = 0;
  int arra_len = strlen(char_array);
  for (int i = 0; i < arra_len; i++) {
    ret_val += (char_array[i] - 0x30);
    ret_val *= 10;
  }
  return (ret_val / 10);
}

// Flush Extra chracter from BT Rx buffer
void flushBTRcv() {
  char ret_char;
  while (BTSerial.available() > 0) {
    ret_char = BTSerial.read();
  }
}
