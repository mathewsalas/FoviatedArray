//SPI Communication Library
#include <SPI.h>

//Connection and Control Pins
const int dataReadyPin = 49; //I'm not sure if this needs a specific pin, for the time being I've set it to a random digital pin
const int chipSelectPin = 53;

struct int24{
  unsigned int data : 24;
}

void setup() {
  Serial.begin(9600);

  //Start SPI
  SPI.begin();

  //Initialize data ready and chip select pins
  pinMode(dataReadyPin, INPUT);
  pinMode(chipSelectPin, OUTPUT);

  //Configuration
  //(Registers reserved pins are currently written to the reset value, might be subject to change as I'm not sure
  //if this is a valid way to do this)
  writeRegister(0x04, 0x00); //Write the MUX Register: MUX[1:0]: Normal Input Polarity
  writeRegister(0x05, 0b01001010) //Write the CONFIG1 Register: REF_RNG: High Ref(Not Sure about this), INP_RNG: 1x, 
                                  //VCM: disabled, REFP_BUF: Enabled(unsure), AINP_BUF: Enabled, AINN_BUF: Disabled
  writeRegister(0x06, 0b00000100) //Write the CONFIG2 Register: EXT_RNG: Standard, SDO_MODE: output, START_MODE[1:0]: start/stop
                                  //SPEED_MODE: Low-Speed, STBY_MODE: Idle, PWDN: Normal
  writeRegister(0x07, 0b00000000) //Write the CONFIG3 Register: DELAY[2:0]: no delay, FILTER[4:0]: wideband, OSR = 50
                                  //This set the sampling rate of 50kSPS
  writeRegister(0x08, 0b10100000) //Write the CONFIG4 Register: CLK_SEL: External, CLK_DIV: No divide, OUT_DRV: full drive
                                  //DATA: 24bit, SPI_CRC: no byte/disabled, REG_CRC: disabled, STATUS: no byte/disabled
  
  //Begin Conversion
  writeRegister(0x03, 0b00000010); //Write to control Register: RESET[5:0]: noop, START: start conversion, STOP: noop
}

void loop() {
  // put your main code here, to run repeatedly:
  if(digitalRead(dataReadyPin) == HIGH){
    struct int24 data = ConversionData();
    
  }

}

struct int24 ConversionData(){
  //Start SPI Frame
  digitalWrite(chipSelectPin, LOW);

  //Return 24 bits of data from the ADC
  //the ADS127L11 uses 0x00 command for returning conversion data
  byte MSB = SPI.transfer(0x00);
  byte MID = SPI.transfer(0x00);
  byte LSB = SPI.transfer(0x00);

  //End SPI Frame
  digitalWrite(chipSelectPin, HIGH);

  //return 24 bits of data
  return (MSB << 16) | (MID << 8) | (LSB << 0);
}

void writeRegister(byte Register, byte Value){
  //ADS127L11 Uses the first 4 bits for commands, 00h for noop, 40h for reading, and 80h for writing
  //ADS127L11 Register Range from 0h to Fh in the lower nible
  //ADS127L11 Requires the input frame to match the size of the output frame (3 bytes in this case)
  byte command = 0x80 | Register;

  //Start SPI Frame
  digitalWrite(chipSelectPin, LOW);
  
  SPI.transfer(0x00); //Padding Byte
  SPI.transfer(command); //Command and Register Address
  SPI.transfer(Value); //Value to be written

  //End SPI Frame
  digitalWrite(chipSelectPin, HIGH);
}
