#define LED_BUILTIN 2  // Onboard LED pin on NodeMCU

void setup() {
  // put your setup code here, to run once:

  pinMode(LED_BUILTIN, OUTPUT);  // Initialize the LED pin as an 

  // This should match the serial speed configured in VSCode
  Serial.begin(115200);

  // Allow time for the serial monitor to start
  delay(3000);

  Serial.println("Hello World!");

}

void loop() {
  // main code

  digitalWrite(LED_BUILTIN, LOW);  // Turn the LED on
  delay(1000);                     // Wait for a second
  digitalWrite(LED_BUILTIN, HIGH); // Turn the LED off
  Serial.println("Blink!");
  delay(1000);                     // Wait for a second

}
