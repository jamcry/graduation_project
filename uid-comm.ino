#include <MFRC522Hack.h>
#include <MFRC522Extended.h>
#include <deprecated.h>
#include <MFRC522Debug.h>
#include <require_cpp11.h>
#include <MFRC522.h>
#include <SPI.h>

#define SS_PIN 10
#define RST_PIN 9
#define LED_R A0
#define LED_G A1
#define LED_B A2
#define BUT_Y 2
#define BUT_N 3
//for seral comm.
char receivedChar;
boolean newData = false;

MFRC522 mfrc522(SS_PIN , RST_PIN);


  void ledOn(char color){
    switch(color){
      case 'R':
      digitalWrite(LED_R , HIGH);
      break;

      case 'G':
      digitalWrite(LED_G , HIGH);
      break;

      case 'B':
      digitalWrite(LED_B , HIGH);
      break;
      }
  }
  void ledOff(char color){
    switch(color){
      case 'R':
      digitalWrite(LED_R , LOW);
      break;

      case 'G':
      digitalWrite(LED_G , LOW);
      break;

      case 'B':
      digitalWrite(LED_B , LOW);
      break;
      }
  }

char recvOneChar() {
    if (Serial.available() > 0) {
        receivedChar = Serial.read();
        newData = true;
        return receivedChar;
    }
}

void showNewData() {
    if (newData == true) {
        Serial.print("This just in ... ");
        Serial.println(receivedChar);
        newData = false;
    }
}
unsigned long getCardID(int showCardData = 0){
  /* this function gets the uid of the rfid card
   and returns the uid in unsigned long format */
  /* this fn. should be called with argument 1
  if printing the card data to the serial monitor
  is needed. */


  unsigned long UID_unsigned;
   UID_unsigned =  mfrc522.uid.uidByte[0] << 24;
   UID_unsigned += mfrc522.uid.uidByte[1] << 16;
   UID_unsigned += mfrc522.uid.uidByte[2] <<  8;
   UID_unsigned += mfrc522.uid.uidByte[3];

   if(showCardData==1){
   Serial.println("UID Unsigned int");
   Serial.println(UID_unsigned);

   String UID_string =  (String)UID_unsigned;
   long UID_LONG=(long)UID_unsigned;

   Serial.println("UID Long :");
   Serial.println(UID_LONG);

   Serial.println("UID String :");
   Serial.println(UID_string); } // print card data to the serial monitor
   int UID_int = (int) UID_unsigned;
   return UID_int; }


//void printCardName(int cardUID){}

void setup() {
  pinMode(LED_R , OUTPUT);
  pinMode(LED_G , OUTPUT);
  pinMode(LED_B , OUTPUT);
  pinMode(BUT_Y , INPUT);
  pinMode(BUT_N , INPUT);

  Serial.begin(9600); // Init. serial comm. with the PC
  SPI.begin(); // Init. SPI bus for MFRC522
  mfrc522.PCD_Init(); // Init. MFRC522 card
}

void loop() {
    // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent() )
    return;

  // Verify if the NUID has been readed
  if ( ! mfrc522.PICC_ReadCardSerial() )
    return;

  int cardUID = getCardID();

  Serial.print(cardUID);
  Serial.print("\n");
  char canVote = ' ';
  while(canVote == ' '){
    canVote = recvOneChar();
    if(canVote == 'Y'){
      ledOn('G');
      getVote();
      break;
    }
    else if(canVote == 'N'){
      ledOn('R');
      break;
    }
    else
    continue;
  }


//get vote HERE




}

void getVote(){
  while(1){
    if(digitalRead(BUT_Y) == HIGH){
      Serial.print("YES");
      Serial.print('\n');
    }
    else if(digitalRead(BUT_N) == HIGH){
      Serial.print("NO");
      Serial.print('\n');
    }
    else
    continue;
  }
}
