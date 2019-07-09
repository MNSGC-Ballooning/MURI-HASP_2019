void setup() {
 pinMode(35, OUTPUT);
}

void loop() {
  digitalWrite(35, HIGH);
  delay (750);
  digitalWrite(35, LOW);
  delay (250);
}
