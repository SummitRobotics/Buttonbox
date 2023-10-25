
int x; 
void setup() { 
    pinMode(5, INPUT_PULLUP);
    Serial.begin(115200); 
    Serial.setTimeout(1); 
} 
void loop() { 
    //while (!Serial.available()); 
    //Serial.read();

    // Read and send the state of a button
    x = digitalRead(5);
	  Serial.print(x);
    delay(100); 
} 
