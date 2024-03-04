void setup(){
  pinMode(13, INPUT);
  pinMode(12, INPUT);
  pinMode(14, INPUT);
  Serial.begin(9600);
}

void loop(){
  Serial.print(analogRead(13));
  Serial.print(" ");
  Serial.print(analogRead(12));
  Serial.print(" ");
  Serial.println(analogRead(14));
}
