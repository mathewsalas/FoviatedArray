int32_t i = 1;
int32_t t = 0;
int32_t scale = ((int32_t)1)<<23-1;
int32_t testVal;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(230400);

  while(i){
    i = Serial.read() == 'A' ? 0 : 1;
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  t = (t+1)%50000;
  testVal = (int32_t)(scale*0.5*sin(2*3.14159*1000*t/50000));
  Serial.write(testVal & 0xFF);
  Serial.write(testVal>>8 & 0xFF);
  Serial.write(testVal>>16 & 0xFF);
}
