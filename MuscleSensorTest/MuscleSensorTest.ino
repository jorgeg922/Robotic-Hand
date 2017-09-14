int led = 3;
int muscleValue = 0;
int ledValue = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(led, OUTPUT);
  Serial.begin(9600);
  Serial.println("Muscle Sensor Test");
}

void loop() {
  muscleValue = analogRead(A0);
  Serial.print("Muscle Value: ");
  Serial.println(muscleValue);
  analogWrite(led, muscleValue);
}
