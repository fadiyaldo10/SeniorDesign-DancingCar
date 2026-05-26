const int RPWM = 4;  const int LPWM = 5;
const int R_EN = 24; const int L_EN = 25;

void setup() {
  pinMode(RPWM, OUTPUT); pinMode(LPWM, OUTPUT);
  pinMode(R_EN, OUTPUT); pinMode(L_EN, OUTPUT);
  digitalWrite(R_EN, HIGH); digitalWrite(L_EN, HIGH);
  Serial.begin(9600);
  Serial.println("Commands: 'max' = fully extend, 'min' = fully retract, 'stop' = stop");
}

void loop() {
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();

    if (cmd == "max") {
      Serial.println("Extending to MAX...");
      analogWrite(RPWM, 255); analogWrite(LPWM, 0);
      delay(5000);
      analogWrite(RPWM, 0); analogWrite(LPWM, 0);
      Serial.println("Done. Measure now.");
    }
    else if (cmd == "min") {
      Serial.println("Retracting to MIN...");
      analogWrite(RPWM, 0); analogWrite(LPWM, 255);
      delay(5000);
      analogWrite(RPWM, 0); analogWrite(LPWM, 0);
      Serial.println("Done. Measure now.");
    }
    else if (cmd == "stop") {
      analogWrite(RPWM, 0); analogWrite(LPWM, 0);
      Serial.println("Stopped.");
    }
    else {
      Serial.println("Unknown command. Use: max, min, stop");
    }
  }
}