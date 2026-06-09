// Pins for the LEDs (Simulating the Actuators)
const int micPin = A0;       // MAX9814 or KY-038 Analog output
const int A1_A3_Left = 10;   // Green & Yellow wired to 10 & 11 (Left Side)
const int A2_A4_Right = 12;  // Red & Blue wired to 12 & 13 (Right Side)

// Bass Filtering Variables
float filteredSignal = 512.0; // Starts at the center reference voltage
const float lpFilterCutoff = 0.15; // Low-Pass Filter coefficient (Lower = tighter bass, less treble)

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
const int beatDebounce = 300; // Prevents the car from double-triggering on a single long bass thump

void setup() {
  pinMode(A1_A3_Left, OUTPUT);
  pinMode(A2_A4_Right, OUTPUT);
  pinMode(11, OUTPUT); // Visual LED pairs
  pinMode(13, OUTPUT);
  
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
    
    // Low-Pass Filter Formula: It smooths out high frequency jaggedness, leaving only low bass waves
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

    // 4. THE CHOREOGRAPHY SWITCH
    switch (beatCount) {
      case 1:
        digitalWrite(A1_A3_Left, HIGH);  digitalWrite(11, HIGH); 
        digitalWrite(A2_A4_Right, LOW);  digitalWrite(13, LOW);  
        Serial.println("Action: LEFT UP / RIGHT LOW");
        break;
        
      case 2:
        digitalWrite(A1_A3_Left, LOW);   digitalWrite(11, LOW);  
        digitalWrite(A2_A4_Right, HIGH); digitalWrite(13, HIGH); 
        Serial.println("Action: LEFT LOW / RIGHT UP");
        break;
        
      case 3:
        digitalWrite(A1_A3_Left, HIGH);  digitalWrite(11, HIGH); 
        digitalWrite(A2_A4_Right, LOW);  digitalWrite(13, LOW);  
        Serial.println("Action: LEFT UP / RIGHT LOW");
        break;
        
      case 4:
        digitalWrite(A1_A3_Left, LOW);   digitalWrite(11, LOW);  
        digitalWrite(A2_A4_Right, HIGH); digitalWrite(13, HIGH); 
        Serial.println("Action: LEFT LOW / RIGHT UP");
        break;
    }
  }
}
