#include <SD.h>

File myFile;

void setup()
{
  //pinMode(10, OUTPUT);
  //digitalWrite(10, HIGH);
  Serial.begin(115200);
  Serial.print("Iniciando SD ...");
  if (!SD.begin(10)) {
    Serial.println("No se pudo inicializar");
    return;
  }
  Serial.println("inicializacion exitosa");
 
  myFile = SD.open("archivo.txt");//abrimos  el archivo 
  if (myFile) {
    Serial.println("archivo.txt:");
    while (myFile.available()) {
    	Serial.write(myFile.read());
    }
    myFile.close(); //cerramos el archivo
  } else {
    Serial.println("Error al abrir el archivo");
  }
}

void loop()
{
  
}