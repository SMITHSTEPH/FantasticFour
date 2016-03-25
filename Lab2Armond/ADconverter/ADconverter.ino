
int val = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  pinMode(A3, INPUT);
  val = analogRead(A3);
  Serial.println(val);
}
