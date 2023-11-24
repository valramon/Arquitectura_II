#include <EEPROM.h>

char nick[] = "SAR";
char nicka[2][4];

void setup() {
  
  // put your setup code here, to run once:/*
  EEPROM.put(0, 0);
  EEPROM.put(100, 0);
  EEPROM.put(200, 0);
  EEPROM.put(300, 0);
  EEPROM.put(1000, "___");
  EEPROM.put(1100, "___");
  EEPROM.put(1200, "___");
  EEPROM.put(1300, "___");
  Serial.begin(9600);
  //EEPROM.put(0, nick);
  for(int i = 0; i < 3; i++){
    //EEPROM.write(i, nick[i]);
    //Serial.print(nick[i]);
    //Serial.print(EEPROM.get(i, nicka));
  }
  //EEPROM.write(3, "\n");
  /*
  EEPROM.get(0, nicka[0]);
  EEPROM.get(0, nicka[1]);
  Serial.println(nicka[0]);
  Serial.println(nicka[1]);*/
}

void loop() {
  // put your main code here, to run repeatedly:

}
