const int RPWM = 6;  const int LPWM = 7;
const int R_EN = 26; const int L_EN = 27;

void setup() {
  pinMode(RPWM, OUTPUT); pinMode(LPWM, OUTPUT);
  pinMode(R_EN, OUTPUT); pinMode(L_EN, OUTPUT);
  digitalWrite(R_EN, HIGH); digitalWrite(L_EN, HIGH);
  Serial.begin(9600);
}

void loop() {
  Serial.println("RR Up");
  analogWrite(RPWM, 255); analogWrite(LPWM, 0);
  delay(2000);
  analogWrite(RPWM, 0); analogWrite(LPWM, 0);
  delay(500);
  Serial.println("RR Down");
  analogWrite(RPWM, 0); analogWrite(LPWM, 255);
  delay(2000);
  analogWrite(RPWM, 0); analogWrite(LPWM, 0);
  delay(500);
}