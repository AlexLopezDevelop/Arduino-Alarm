#include <SPI.h>
#include <MFRC522.h>
#include <Keypad.h>
#define SS_PIN 53
#define RST_PIN 49

MFRC522 mfrc522(SS_PIN, RST_PIN);

// -------------- RFID -------------- 
byte LecturaUID[4];         // Almacena el UID leido
byte Usuario1[4]= {0xC0, 0xFD, 0x4C, 0xA8} ;    // UID de tarjeta
byte Usuario2[4]= {0x06, 0x76, 0x25, 0xD9} ;   

const int ledRojoPIN = 48;
const int ledVerdePIN = 47;

// -------------- Keypad  -------------- 

const byte FILAS = 4;
const byte COLUMNAS = 4;

char keys[FILAS][COLUMNAS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte pinesFilas[FILAS] = {39,38,37,36};
byte pinesColumnas[COLUMNAS] = {35,34,33,32};

Keypad teclado = Keypad(makeKeymap(keys), pinesFilas, pinesColumnas, FILAS, COLUMNAS);

char TECLA;
char CLAVE[5];
char CLAVE_MAESTRA[5] = "1234";
byte INDICE = 0;

// -------------- Zumbador -------------- 

float sinVal;
int toneVal;

// -------------- UltraSonidos -------------- 

const int EchoPin = 40;
const int TriggerPin = 42;
float distancia;
long tiempo;

// -------------- Sensor de movimiento -------------- 

int SenMov = 25;

// -------------- Programa -------------- 

int alarmaActivada = 0;

void setup() {
  Serial.begin(9600);  
  SPI.begin();      
  
  mfrc522.PCD_Init();   

  pinMode(31, OUTPUT); // Definimos el pin 8 como salida del zumbador.
   
  pinMode(ledVerdePIN , OUTPUT);  
  pinMode(ledRojoPIN , OUTPUT); 

  pinMode(TriggerPin, OUTPUT);
  pinMode(EchoPin, INPUT);

  pinMode(SenMov, INPUT);
  
  Serial.println("Programa Iniciado");
  Serial.print("\n");         // Salto de linea  

  // Alarma desactivada
  digitalWrite(ledRojoPIN, HIGH);
  digitalWrite(ledVerdePIN, LOW); 
  
  Serial.println("Introducir RFID");
  Serial.print("\n");         // Salto de linea   
}

void loop() {

      if ( ! mfrc522.PICC_IsNewCardPresent())   // si no hay una tarjeta presente
    return;           // retorna al loop esperando por una tarjeta
  
  if ( ! mfrc522.PICC_ReadCardSerial())     // si no puede obtener datos de la tarjeta
    return;           // retorna al loop esperando por otra tarjeta
    Serial.print("\n");         // Salto de linea   
    Serial.print("UID:");       // muestra texto UID:
    for (byte i = 0; i < mfrc522.uid.size; i++) { // bucle recorre de a un byte por vez el UID
      if (mfrc522.uid.uidByte[i] < 0x10){   // si el byte leido es menor a 0x10
        Serial.print(" 0");       // imprime espacio en blanco y numero cero
        }
        else{           // sino
          Serial.print(" ");        // imprime un espacio en blanco
          }
          Serial.print(mfrc522.uid.uidByte[i], HEX);    // imprime el byte del UID leido en hexadecimal
          LecturaUID[i]=mfrc522.uid.uidByte[i];     // almacena en array el byte del UID leido      
          }
          
          Serial.print("\n");         // Salto de linea             
                    
          if(comparaUID(LecturaUID, Usuario1)){
            Serial.println("Bienvenido Usuario 1, introduce la contraseña");
            Serial.print("\n");         // Salto de linea  
            while (entrarPassword() != 1) { //Mirar hasta que introduzca la contraseña correcta
              }
          } else if(comparaUID(LecturaUID, Usuario2)){
            Serial.println("Bienvenido Usuario 2, introduce la contraseña");
            Serial.print("\n");         // Salto de linea 
            while (entrarPassword() != 1) { //Mirar hasta que introduzca la contraseña correcta
              } 
          }else {
            Serial.println("RFID No valido");          
          }       
                  mfrc522.PICC_HaltA();     // detiene comunicacion con tarjeta  

          while (alarmaActivada == 1) { // Mientras la alarma este activada
            ultrasonidos();
            sensorMovimiento();
          }
}

int entrarPassword() {
    TECLA = teclado.getKey();
  
  if (TECLA) {
    CLAVE[INDICE] = TECLA;
    INDICE++;
    Serial.print(TECLA);  
  }

  if (INDICE == 4) {
    if (!strcmp(CLAVE, CLAVE_MAESTRA)){
            digitalWrite(ledVerdePIN, HIGH);
            digitalWrite(ledRojoPIN, LOW); 
            Serial.print("\n");         // Salto de linea  
            Serial.println("Alarma Activada"); 
            alarmaActivada = 1;
      return 1;
    } else {
      Serial.print("\n");         // Salto de linea  
      Serial.println("Contraseña Incorrecta");
      INDICE = 0;
    }
  }

      return 0;
}

boolean comparaUID(byte lectura[],byte usuario[]) // funcion comparaUID
{
  for (byte i=0; i < mfrc522.uid.size; i++){    // bucle recorre de a un byte por vez el UID
  if(lectura[i] != usuario[i])        // si byte de UID leido es distinto a usuario
    return(false);          // retorna falso
  }
  return(true);           // si los 4 bytes coinciden retorna verdadero
}

void sonarAlarma(){
     for(int x=0; x<180; x++){
            // convertimos grados en radianes para luego obtener el valor.
            sinVal = (sin(x*(3.1412/180)));
            // generar una frecuencia a partir del valor sin
            toneVal = 2000+(int(sinVal*1000));
            tone(31, toneVal);
            delay(2); 
     }   
}


void ultrasonidos() {
  
  digitalWrite(TriggerPin, HIGH);  //se envía un pulso para activar el sensor
  delayMicroseconds(10);
  digitalWrite(TriggerPin, LOW);

  // medimos el pulso de respuesta
  tiempo = (pulseIn(EchoPin, HIGH)/2); 
  
  distancia = float(tiempo * 0.0343);

   Serial.print("Distancia: "); // imprime la distancia en el Monitor Serie
   Serial.println(distancia);
   

  if (distancia <= 5) {
    delay(30);
    while(1 == 1){
      sonarAlarma();
    }
  }
}

void sensorMovimiento(){
  if (digitalRead(SenMov) == HIGH){
    while(1 == 1){
      sonarAlarma();
    }
  }
  
}
