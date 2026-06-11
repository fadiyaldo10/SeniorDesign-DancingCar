// Pins for the LEDs (Mapped based on your physical setup)
const int micPin = A0;       // MAX9814 or KY-038 Analog output
const int A1_A2_Front = 10;  // Green LED on pin 10
const int Front_Red = 12;    // Red LED on pin 12
const int A3_A4_Back = 11;   // Blue LED on pin 11
const int Back_Yellow = 13;  // Yellow LED on pin 13

// Bass Filtering Variables
float filteredSignal = 512.0;       // Starts at the center reference voltage
const float lpFilterCutoff = 0.15;  // Low-Pass Filter coefficient (Lower = tighter bass)

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

// Bass Beat Threshold (Adjust this to make the bass beat more/less sensitive)
const float beatThresholdMultiplier = 1.30; 

// Choreography Variables
int beatCount = 0; 
unsigned long lastBeatTime = 0;
const int beatDebounce = 300; // Prevents double-triggering on a single long bass thump

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
}

void loop() {
  bassMax = 0;
  bassMin = 1024;
  sampleStart = millis();

  // 1. COLLECT SAMPLES & APPLY DIGITAL LOW-PASS FILTER
  while (millis() - sampleStart < sampleWindow) {
    int rawSample = analogRead(micPin);
    
    // Low-Pass Filter Formula: smooths out high frequency noise, leaving only low bass waves
    filteredSignal = filteredSignal + lpFilterCutoff * (rawSample - filteredSignal);
    
    int currentFilteredValue = (int)filteredSignal;
    
    // Find the peaks of ONLY the filtered bass signal
    if (currentFilteredValue > bassMax) bassMax = currentFilteredValue;
    if (currentFilteredValue < bassMin) bassMin = currentFilteredValue;
  }
  
  // This is the volume (amplitude) of ONLY the bass frequencies
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
    
    if (beatCount > 4) {     
      beatCount = 1;
    }
    
    Serial.print("--- BASS BEAT DETECTED: ");
    Serial.println(beatCount);

    // 4. CHOREOGRAPHY SWITCH - INDIVIDUAL COLOR CYCLE
    switch (beatCount) {
      case 1:
        // Beat 1: Only Green is ON
        digitalWrite(A1_A2_Front, HIGH); // Green ON
        digitalWrite(Front_Red, LOW);    // Red OFF
        digitalWrite(Back_Yellow, LOW);  // Yellow OFF
        digitalWrite(A3_A4_Back, LOW);   // Blue OFF
        Serial.println("Action: GREEN");
        break;
        
      case 2:
        // Beat 2: Only Red is ON
        digitalWrite(A1_A2_Front, LOW);  // Green OFF
        digitalWrite(Front_Red, HIGH);   // Red ON
        digitalWrite(Back_Yellow, LOW);  // Yellow OFF
        digitalWrite(A3_A4_Back, LOW);   // Blue OFF
        Serial.println("Action: RED");
        break;
        
      case 3:
        // Beat 3: Only Yellow is ON
        digitalWrite(A1_A2_Front, LOW);  // Green OFF
        digitalWrite(Front_Red, LOW);    // Red OFF
        digitalWrite(Back_Yellow, HIGH); // Yellow ON
        digitalWrite(A3_A4_Back, LOW);   // Blue OFF
        Serial.println("Action: YELLOW");
        break;
        
      case 4:
        // Beat 4: Only Blue is ON
        digitalWrite(A1_A2_Front, LOW);  // Green OFF
        digitalWrite(Front_Red, LOW);    // Red OFF
        digitalWrite(Back_Yellow, LOW);  // Yellow OFF
        digitalWrite(A3_A4_Back, HIGH);  // Blue ON
        Serial.println("Action: BLUE");
        break;
    }
  }
}
