// =============================
// BTS7960 4-ACTUATOR CONTROL (FL, FR, RL, RR)
// + BEAT-TRIGGERED DUAL SERVO CHOREOGRAPHY
//
// SERIAL COMMAND FORMAT:
// FL FR RL RR SERVO MODE
// Example: 101011
//
// Character 0: Front Left actuator
// Character 1: Front Right actuator
// Character 2: Rear Left actuator
// Character 3: Rear Right actuator
// Character 4: Servo trigger
// Character 5: Servo choreography mode
//
// Modes:
// 1 = Synced swing
// 2 = Mirrored swing
// 3 = Stepped 4-position sweep
// 4 = Random independent angles
// 5 = Synced swing, reused for big moments
// =============================

#include <Servo.h>

// ---------- Front Left ----------
const int FL_RPWM = 2;
const int FL_LPWM = 3;
const int FL_R_EN = 22;
const int FL_L_EN = 23;

// ---------- Front Right ----------
const int FR_RPWM = 4;
const int FR_LPWM = 5;
const int FR_R_EN = 24;
const int FR_L_EN = 25;

// ---------- Rear Left ----------
const int RL_RPWM = 6;
const int RL_LPWM = 7;
const int RL_R_EN = 26;
const int RL_L_EN = 27;

// ---------- Rear Right ----------
const int RR_RPWM = 8;
const int RR_LPWM = 9;
const int RR_R_EN = 28;
const int RR_L_EN = 29;


// ---------- Servos ----------
const int RIGHT_SERVO_PIN = 11;
const int LEFT_SERVO_PIN  = 10;

Servo rightServo;
Servo leftServo;


// ---------- Servo Angle Settings ----------
const int SERVO_MIN_ANGLE = 30;
const int SERVO_MAX_ANGLE = 150;

// Intermediate positions for Mode 3
const int SERVO_POSITION_1 = 30;
const int SERVO_POSITION_2 = 70;
const int SERVO_POSITION_3 = 110;
const int SERVO_POSITION_4 = 150;


// ---------- Servo State ----------
int servoState = 0;

// Used by Mode 3
int sweepPosition = 0;


// ============================================================
// SETUP
// ============================================================

void setup()
{
    Serial.begin(115200);

    // ---------- Motor Pins ----------
    pinMode(FL_RPWM, OUTPUT);
    pinMode(FL_LPWM, OUTPUT);
    pinMode(FL_R_EN, OUTPUT);
    pinMode(FL_L_EN, OUTPUT);

    pinMode(FR_RPWM, OUTPUT);
    pinMode(FR_LPWM, OUTPUT);
    pinMode(FR_R_EN, OUTPUT);
    pinMode(FR_L_EN, OUTPUT);

    pinMode(RL_RPWM, OUTPUT);
    pinMode(RL_LPWM, OUTPUT);
    pinMode(RL_R_EN, OUTPUT);
    pinMode(RL_L_EN, OUTPUT);

    pinMode(RR_RPWM, OUTPUT);
    pinMode(RR_LPWM, OUTPUT);
    pinMode(RR_R_EN, OUTPUT);
    pinMode(RR_L_EN, OUTPUT);


    // ---------- Enable BTS7960 Drivers ----------
    digitalWrite(FL_R_EN, HIGH);
    digitalWrite(FL_L_EN, HIGH);

    digitalWrite(FR_R_EN, HIGH);
    digitalWrite(FR_L_EN, HIGH);

    digitalWrite(RL_R_EN, HIGH);
    digitalWrite(RL_L_EN, HIGH);

    digitalWrite(RR_R_EN, HIGH);
    digitalWrite(RR_L_EN, HIGH);


    // ---------- Stop All Actuators ----------
    analogWrite(FL_RPWM, 0);
    analogWrite(FL_LPWM, 0);

    analogWrite(FR_RPWM, 0);
    analogWrite(FR_LPWM, 0);

    analogWrite(RL_RPWM, 0);
    analogWrite(RL_LPWM, 0);

    analogWrite(RR_RPWM, 0);
    analogWrite(RR_LPWM, 0);


    // ---------- Attach Servos ----------
    rightServo.attach(RIGHT_SERVO_PIN);
    leftServo.attach(LEFT_SERVO_PIN);

    // Starting position
    rightServo.write(SERVO_MIN_ANGLE);
    leftServo.write(SERVO_MIN_ANGLE);


    Serial.println("Arduino Ready");
}


// ============================================================
// ACTUATOR CONTROL
// ============================================================

// '1' = Extend
// Anything else = Retract
void driveActuator(char command, int rpwmPin, int lpwmPin)
{
    if (command == '1')
    {
        // Extend
        analogWrite(rpwmPin, 0);
        analogWrite(lpwmPin, 255);
    }
    else
    {
        // Retract
        analogWrite(rpwmPin, 255);
        analogWrite(lpwmPin, 0);
    }
}


// ============================================================
// MODE 1 AND MODE 5
// SYNCED SWING
//
// Both servos move to the same angle.
// ============================================================

void syncedSwing()
{
    servoState = !servoState;

    int angle;

    if (servoState)
    {
        angle = SERVO_MAX_ANGLE;
    }
    else
    {
        angle = SERVO_MIN_ANGLE;
    }

    rightServo.write(angle);
    leftServo.write(angle);
}


// ============================================================
// MODE 2
// MIRRORED SWING
//
// The servos move to opposite sides:
//
// Right servo: 30°  -> 150°
// Left servo:  150° -> 30°
//
// Then they reverse.
// ============================================================

void mirroredSwing()
{
    servoState = !servoState;

    if (servoState)
    {
        rightServo.write(SERVO_MAX_ANGLE);
        leftServo.write(SERVO_MIN_ANGLE);
    }
    else
    {
        rightServo.write(SERVO_MIN_ANGLE);
        leftServo.write(SERVO_MAX_ANGLE);
    }
}


// ============================================================
// MODE 3
// STEPPED 4-POSITION SWEEP
//
// Each trigger advances both servos to the next position:
//
// Step 1: 30°
// Step 2: 70°
// Step 3: 110°
// Step 4: 150°
//
// Then the sequence loops back to 30°.
// ============================================================

void steppedSweep()
{
    int angle;

    switch (sweepPosition)
    {
        case 0:
            angle = SERVO_POSITION_1;
            break;

        case 1:
            angle = SERVO_POSITION_2;
            break;

        case 2:
            angle = SERVO_POSITION_3;
            break;

        case 3:
            angle = SERVO_POSITION_4;
            break;

        default:
            angle = SERVO_POSITION_1;
            break;
    }

    rightServo.write(angle);
    leftServo.write(angle);

    sweepPosition++;

    if (sweepPosition >= 4)
    {
        sweepPosition = 0;
    }
}


// ============================================================
// MODE 4
// RANDOM INDEPENDENT ANGLES
//
// Each servo receives its own random angle.
// ============================================================

void randomIndependentAngles()
{
    int rightAngle = random(SERVO_MIN_ANGLE, SERVO_MAX_ANGLE + 1);
    int leftAngle  = random(SERVO_MIN_ANGLE, SERVO_MAX_ANGLE + 1);

    rightServo.write(rightAngle);
    leftServo.write(leftAngle);
}


// ============================================================
// SERVO CHOREOGRAPHY SELECTOR
// ============================================================

void triggerServoChoreography(char mode)
{
    switch (mode)
    {
        case '1':
            // Mode 1: Synced swing
            syncedSwing();
            break;


        case '2':
            // Mode 2: Mirrored swing
            mirroredSwing();
            break;


        case '3':
            // Mode 3: Stepped 4-position sweep
            steppedSweep();
            break;


        case '4':
            // Mode 4: Random independent angles
            randomIndependentAngles();
            break;


        case '5':
            // Mode 5: Synced swing
            // Reused for all-on/all-off big moments
            syncedSwing();
            break;


        default:
            // Unknown mode
            Serial.println("Unknown servo mode");
            break;
    }
}


// ============================================================
// MAIN LOOP
// ============================================================

void loop()
{
    if (Serial.available())
    {
        String command = Serial.readStringUntil('\n');

        command.trim();


        Serial.print("Received: ");
        Serial.println(command);


        // New command length is 6 characters
        if (command.length() != 6)
        {
            Serial.println("Invalid command length");
            return;
        }


        // ---------- Parse Command ----------
        char fl = command.charAt(0);
        char fr = command.charAt(1);
        char rl = command.charAt(2);
        char rr = command.charAt(3);

        char servoFlag = command.charAt(4);
        char servoMode = command.charAt(5);


        // ---------- Actuators ----------
        driveActuator(fl, FL_RPWM, FL_LPWM);
        driveActuator(fr, FR_RPWM, FR_LPWM);
        driveActuator(rl, RL_RPWM, RL_LPWM);
        driveActuator(rr, RR_RPWM, RR_LPWM);


        // ---------- Servos ----------
        if (servoFlag == '1')
        {
            triggerServoChoreography(servoMode);
        }


        Serial.println("Motors Updated");
    }
}
