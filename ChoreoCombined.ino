// Pins for the LEDs (Mapped based on your physical setup)
const int micPin = A0; // MAX9814 or KY-038 Analog output
const int A1_A2_Front = 10;  // Green LED on pin 10
const int A3_A4_Back = 11;   // Blue LED on pin 11
const int Front_Red = 12;    // Red LED on pin 12
const int Back_Yellow = 13;  // Yellow LED on pin 13

// Bass Filtering Variables
float filteredSignal = 512.0; // Starts at the center reference voltage
const float lpFilterCutoff = 0.15; // Low-Pass Filter coefficient

// Variables for Bass Envelope (Volume of the Bass)
int bassMax = 0;
int bassMin = 1024;
unsigned long sampleStart = 0;
const int sampleWindow = 40; // 40ms window catches low frequencies (25Hz)

// Variables for Tracking Bass Volume History
const int historySize = 20;
int bassHistory[historySize];
int historyIndex = 0;
long historySum = 0;

// Bass Beat Threshold 
const float beatThresholdMultiplier = 1.30;

// Choreography Variables
int beatCount = 0; 
unsigned long lastBeatTime = 0;
const int beatDebounce = 300; // Prevents double-triggering

// Mode/Pattern Selection Tracking
int currentMode = 1;       // Start with pattern 1
int totalBeatCounter = 0;  // Tracks total beats to decide when to switch modes
const int beatsPerMode = 16; // Change choreography pattern every 16 beats

void setup() {
  pinMode(A1_A2_Front, OUTPUT);
  pinMode(Front_Red, OUTPUT);
  pinMode(A3_A4_Back, OUTPUT);
  pinMode(Back_Yellow, OUTPUT);
  
  Serial.begin(9600);
  
  // Initialize history array
  for (int i = 0; i < historySize; i++) {
    bassHistory[i] = 0;
  }

  // Seed the random number generator using noise from an unused analog pin (A5)
  randomSeed(analogRead(A5));
  
  Serial.println("--- MULTI-MODE BASS CHOREOGRAPHY STARTED ---");
}

void loop() {
  bassMax = 0;
  bassMin = 1024;
  sampleStart = millis();

  // 1. COLLECT SAMPLES & APPLY DIGITAL LOW-PASS FILTER
  while (millis() - sampleStart < sampleWindow) {
    int rawSample = analogRead(micPin);
    filteredSignal = filteredSignal + lpFilterCutoff * (rawSample - filteredSignal);
    int currentFilteredValue = (int)filteredSignal;
    
    if (currentFilteredValue > bassMax) bassMax = currentFilteredValue;
    if (currentFilteredValue < bassMin) bassMin = currentFilteredValue;
  }
  
  // Volume (amplitude) of ONLY the bass frequencies
  int bassPeakToPeak = bassMax - bassMin;

  // 2. TRACK THE RUNNING AVERAGE OF THE BASS
  historySum -= bassHistory[historyIndex];       
  bassHistory[historyIndex] = bassPeakToPeak;        
  historySum += bassHistory[historyIndex];
  historyIndex = (historyIndex + 1) % historySize; 
  float averageBassVolume = (float)historySum / historySize;

  // 3. TRIGGER BEAT CHOREOGRAPHY BASED ON BASS SPIKES ONLY
  if (bassPeakToPeak > (averageBassVolume * beatThresholdMultiplier) && bassPeakToPeak > 20 && (millis() - lastBeatTime > beatDebounce)) {
    
    lastBeatTime = millis();
    beatCount++;             
    totalBeatCounter++;
    
    if (beatCount > 4) {     
      beatCount = 1;
    }
    
    // Check if it's time to cycle to the next choreography pattern
    if (totalBeatCounter >= beatsPerMode) {
      totalBeatCounter = 0;
      currentMode++;
      if (currentMode > 5) {
        currentMode = 1; // Loop back to the first pattern
      }
      Serial.print(">>> SWITCHING TO CHOREOGRAPHY PATTERN MODE: ");
      Serial.println(currentMode);
    }
    
    Serial.print("Mode [");
    Serial.print(currentMode);
    Serial.print("] - Beat: ");
    Serial.println(beatCount);

    // 4. THE MASTER CHOREOGRAPHY SWITCH
    switch (currentMode) {
      
      // ==========================================
      // MODE 1: Alternating Left / Right
      // ==========================================
      case 1:
        switch (beatCount) {
          case 1: case 3:
            digitalWrite(A1_A2_Front, HIGH); digitalWrite(A3_A4_Back, HIGH); // Left side simulation (Green + Blue)
            digitalWrite(Front_Red, LOW);    digitalWrite(Back_Yellow, LOW);  // Right side off
            Serial.println("Action: LEFT ON / RIGHT OFF");
            break;
          case 2: case 4:
            digitalWrite(A1_A2_Front, LOW);  digitalWrite(A3_A4_Back, LOW);
            digitalWrite(Front_Red, HIGH);   digitalWrite(Back_Yellow, HIGH); // Right side simulation (Red + Yellow)
            Serial.println("Action: LEFT OFF / RIGHT ON");
            break;
        }
        break;

      // ==========================================
      // MODE 2: Front vs Back / Pairs
      // ==========================================
      case 2:
        switch (beatCount) {
          case 1: case 3:
            digitalWrite(A1_A2_Front, HIGH); digitalWrite(Front_Red, HIGH); 
            digitalWrite(A3_A4_Back, LOW);   digitalWrite(Back_Yellow, LOW); 
            Serial.println("Action: Green/Red HIGH | Blue/Yellow LOW");
            break;
          case 2: case 4:
            digitalWrite(A1_A2_Front, LOW);  digitalWrite(Front_Red, LOW);  
            digitalWrite(A3_A4_Back, HIGH);  digitalWrite(Back_Yellow, HIGH); 
            Serial.println("Action: Green/Red LOW | Blue/Yellow HIGH");
            break;
        }
        break;

      // ==========================================
      // MODE 3: Sequential Individual Color Cycle
      // ==========================================
      case 3:
        switch (beatCount) {
          case 1:
            digitalWrite(A1_A2_Front, HIGH); digitalWrite(Front_Red, LOW); digitalWrite(Back_Yellow, LOW); digitalWrite(A3_A4_Back, LOW);
            Serial.println("Action: GREEN");
            break;
          case 2:
            digitalWrite(A1_A2_Front, LOW); digitalWrite(Front_Red, HIGH); digitalWrite(Back_Yellow, LOW); digitalWrite(A3_A4_Back, LOW);
            Serial.println("Action: RED");
            break;
          case 3:
            digitalWrite(A1_A2_Front, LOW); digitalWrite(Front_Red, LOW); digitalWrite(Back_Yellow, HIGH); digitalWrite(A3_A4_Back, LOW);
            Serial.println("Action: YELLOW");
            break;
          case 4:
            digitalWrite(A1_A2_Front, LOW); digitalWrite(Front_Red, LOW); digitalWrite(Back_Yellow, LOW); digitalWrite(A3_A4_Back, HIGH);
            Serial.println("Action: BLUE");
            break;
        }
        break;

      // ==========================================
      // MODE 4: Random Color Selection
      // ==========================================
      case 4:
        { 
          int randomBeat = random(1, 5);
          switch (randomBeat) {
            case 1:
              digitalWrite(A1_A2_Front, HIGH); digitalWrite(Front_Red, LOW); digitalWrite(Back_Yellow, LOW); digitalWrite(A3_A4_Back, LOW);
              Serial.println("Action: RANDOM GREEN");
              break;
            case 2:
              digitalWrite(A1_A2_Front, LOW); digitalWrite(Front_Red, HIGH); digitalWrite(Back_Yellow, LOW); digitalWrite(A3_A4_Back, LOW);
              Serial.println("Action: RANDOM RED");
              break;
            case 3:
              digitalWrite(A1_A2_Front, LOW); digitalWrite(Front_Red, LOW); digitalWrite(Back_Yellow, HIGH); digitalWrite(A3_A4_Back, LOW);
              Serial.println("Action: RANDOM YELLOW");
              break;
            case 4:
              digitalWrite(A1_A2_Front, LOW); digitalWrite(Front_Red, LOW); digitalWrite(Back_Yellow, LOW); digitalWrite(A3_A4_Back, HIGH);
              Serial.println("Action: RANDOM BLUE");
              break;
          }
        }
        break;

      // ==========================================
      // MODE 5: All On / All Off Flash
      // ==========================================
      case 5:
        switch (beatCount) {
          case 1: case 3:
            digitalWrite(A1_A2_Front, HIGH); digitalWrite(Front_Red, HIGH); digitalWrite(Back_Yellow, HIGH); digitalWrite(A3_A4_Back, HIGH);
            Serial.println("Action: ALL ON");
            break;
          case 2: case 4:
            digitalWrite(A1_A2_Front, LOW); digitalWrite(Front_Red, LOW); digitalWrite(Back_Yellow, LOW); digitalWrite(A3_A4_Back, LOW);
            Serial.println("Action: ALL OFF");
            break;
        }
        break;
    }
  }
}
