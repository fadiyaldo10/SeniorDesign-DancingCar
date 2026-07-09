// --- PIN CONFIGURATIONS (Front Actuators Only) ---
// Front Left
const int FL_RPWM = 2;  const int FL_LPWM = 3;
const int FL_R_EN = 22; const int FL_L_EN = 23;

// Front Right
const int FR_RPWM = 4;  const int FR_LPWM = 5;
const int FR_R_EN = 24; const int FR_L_EN = 25;

// Rear Actuators (Pins kept only to turn them OFF safely if wired)
const int RL_R_EN = 26; const int RL_L_EN = 27;
const int RR_R_EN = 28; const int RR_L_EN = 29;

void setup() {
  // Configure Output Modes for Front
  pinMode(FL_RPWM, OUTPUT); pinMode(FL_LPWM, OUTPUT); pinMode(FL_R_EN, OUTPUT); pinMode(FL_L_EN, OUTPUT);
  pinMode(FR_RPWM, OUTPUT); pinMode(FR_LPWM, OUTPUT); pinMode(FR_R_EN, OUTPUT); pinMode(FR_L_EN, OUTPUT);

  // Configure Rear Enable Pins to pull low/disable
  pinMode(RL_R_EN, OUTPUT); pinMode(RL_L_EN, OUTPUT);
  pinMode(RR_R_EN, OUTPUT); pinMode(RR_L_EN, OUTPUT);
  digitalWrite(RL_R_EN, LOW); digitalWrite(RL_L_EN, LOW);
  digitalWrite(RR_R_EN, LOW); digitalWrite(RR_L_EN, LOW);

  // Enable Front motor drivers 
  digitalWrite(FL_R_EN, HIGH); digitalWrite(FL_L_EN, HIGH);
  digitalWrite(FR_R_EN, HIGH); digitalWrite(FR_L_EN, HIGH);

  // High-speed baud rate matching Jetson Orin Nano
  Serial.begin(115200); 
}

void loop() {
  // Listen for commands from the Jetson Nano
  if (Serial.available() >= 3) { // 2 data characters + '\n' newline
    String command = Serial.readStringUntil('\n');
    command.trim();

    if (command.length() == 2) {
      char fl_state = command.charAt(0);
      char fr_state = command.charAt(1);

      // --- EXECUTE FRONT LEFT ---
      if (fl_state == '1') {      // Move UP
        analogWrite(FL_RPWM, 255); analogWrite(FL_LPWM, 0);
      } else {                    // Move DOWN
        analogWrite(FL_RPWM, 0);   analogWrite(FL_LPWM, 255);
      }

      // --- EXECUTE FRONT RIGHT ---
      if (fr_state == '1') {      // Move UP
        analogWrite(FR_RPWM, 230); analogWrite(FR_LPWM, 0); // Kept your custom 230 tuning
      } else {                    // Move DOWN
        analogWrite(FR_RPWM, 0);   analogWrite(FR_LPWM, 230);
      }
    }
  }
}
