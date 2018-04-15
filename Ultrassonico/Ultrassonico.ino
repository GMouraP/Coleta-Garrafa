uint8_t EmPwmCmd[4] = {
  0x44,0x22,0xbb,0x01};
  
  int ultrapwm = 5, ultratrig = 4;
  
  void setup(){
  ultraSetup(ultrapwm, ultratrig);
  Serial.begin(9600);
}

void loop(){
  Serial.println(ultraCheck(ultrapwm, ultratrig));
  delay(50);
}


void ultraSetup(int ultraPWM, int ultraTrig){
  pinMode(ultraTrig, OUTPUT);                           // A low pull on pin COMP/TRIG
  digitalWrite(ultraTrig, HIGH);                        // Set to HIGH
  pinMode(ultraPWM, INPUT);                             // Sending Enable PWM mode command
}
/*------------------------------------------*/
int ultraCheck(int ultrapwm, int ultratrig){
  unsigned int distance = 0;
  digitalWrite(ultratrig, LOW);
  digitalWrite(ultratrig, HIGH);
  unsigned long medicao = pulseIn(ultrapwm, LOW);
  if(medicao < 50000 && medicao >= 2) distance = medicao/50;
  else return 50000;
  return distance;
}

