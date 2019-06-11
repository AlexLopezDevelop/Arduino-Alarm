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

// -------------- Programa -------------- 
void setup() {
  Serial.begin(9600);  
  SPI.begin();      
  
  mfrc522.PCD_Init();   
   
  pinMode(ledVerdePIN , OUTPUT);  
  pinMode(ledRojoPIN , OUTPUT); 
  
  Serial.println("Programa Iniciado"); 
}

void loop() {
  if ( ! mfrc522.PICC_IsNewCardPresent())   // si no hay una tarjeta presente
    return;           // retorna al loop esperando por una tarjeta
  
  if ( ! mfrc522.PICC_ReadCardSerial())     // si no puede obtener datos de la tarjeta
    return;           // retorna al loop esperando por otra tarjeta
    
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
          
          Serial.print("\t");         // Salto de linea             
                    
          if(comparaUID(LecturaUID, Usuario1)){
            digitalWrite(ledVerdePIN, HIGH);
            digitalWrite(ledRojoPIN, LOW);
            Serial.println("Bienvenido Usuario 1");
          } else if(comparaUID(LecturaUID, Usuario2)){
            digitalWrite(ledVerdePIN, HIGH);
            digitalWrite(ledRojoPIN, LOW);
            Serial.println("Bienvenido Usuario 2");
          }else {
            digitalWrite(ledRojoPIN, HIGH);
            digitalWrite(ledVerdePIN, LOW);
            Serial.println("No te conozco");          
          }       
                  mfrc522.PICC_HaltA();     // detiene comunicacion con tarjeta                
}

boolean comparaUID(byte lectura[],byte usuario[]) // funcion comparaUID
{
  for (byte i=0; i < mfrc522.uid.size; i++){    // bucle recorre de a un byte por vez el UID
  if(lectura[i] != usuario[i])        // si byte de UID leido es distinto a usuario
    return(false);          // retorna falso
  }
  return(true);           // si los 4 bytes coinciden retorna verdadero
}
