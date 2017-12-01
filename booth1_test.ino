 #include <MFRC522Hack.h>
#include <MFRC522Extended.h>
#include <deprecated.h>
#include <MFRC522Debug.h>
#include <require_cpp11.h>
#include <MFRC522.h>
#include <SPI.h>
#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>

#define I2C_ADDR    0x27 // <<----- Add your address here.  Find it from I2C Scanner
#define BACKLIGHT_PIN     3
#define En_pin  2
#define Rw_pin  1
#define Rs_pin  0
#define D4_pin  4
#define D5_pin  5
#define D6_pin  6
#define D7_pin  7
LiquidCrystal_I2C  lcd(I2C_ADDR,En_pin,Rw_pin,Rs_pin,D4_pin,D5_pin,D6_pin,D7_pin);

#define SS_PIN 10
#define RST_PIN 9
#define LED_R A0
#define LED_G A1
#define LED_B A2
#define BUZ A3
#define G_BUTTON 4
#define W_BUTTON 5
#define OK_BUTTON 6
#define B_BUTTON 2
#define R_BUTTON 3

//for seral comm.
char receivedChar;
boolean newData = false;

MFRC522 mfrc522(SS_PIN , RST_PIN);

void lcdClearLine(int line){
  lcd.setCursor(0 , line);
  String blank;
  for(int i = 1 ; i <= 16 ; i++){
    blank += " ";
  }
  lcd.print(blank);
}

void lcdPrint(int line , String text){
    lcdClearLine(line);

  lcd.setCursor(0 , line);

  lcd.print(text);
}

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

void buzzerOK(){
    digitalWrite(BUZ,HIGH);
    delay(150);
    digitalWrite(BUZ,LOW);
    delay(150);
    digitalWrite(BUZ,HIGH);
    delay(150);
    digitalWrite(BUZ,LOW); }

void buzzerFail(){
  digitalWrite(BUZ,HIGH);
  delay(500);
  digitalWrite(BUZ,LOW);
    delay(500);
  digitalWrite(BUZ,HIGH);
  delay(500);
  digitalWrite(BUZ,LOW);

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

// TODO : Change this function to get HEX UID and store in an array
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
    Serial.println(UID_string);
  } // print card data to the serial monitor

  int UID_int = (int) UID_unsigned;
  return UID_int;
}

void setup() {
  pinMode(LED_R , OUTPUT);
  pinMode(LED_G , OUTPUT);
  pinMode(LED_B , OUTPUT);
  pinMode(BUZ   , OUTPUT);
  pinMode(B_BUTTON , INPUT);
  pinMode(G_BUTTON , INPUT);
  pinMode(B_BUTTON , INPUT);
  pinMode(W_BUTTON , INPUT);
  pinMode(OK_BUTTON , INPUT);

  Serial.begin(9600); // Init. serial comm. with the PC
  SPI.begin(); // Init. SPI bus for MFRC522
  mfrc522.PCD_Init(); // Init. MFRC522 card

  lcd.begin(16,2);
  lcd.setBacklightPin(BACKLIGHT_PIN , POSITIVE);
  lcd.setBacklight(HIGH);
  lcd.home();
  lcdPrint(0 , "- ELECTION 18' -");
  lcdPrint(1 , "Waiting for Card ");
}

char canVote;

void checkForNewCard(){
  if ( mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial() ){
    int newCardUID = getCardID();
    Serial.print("R");
    Serial.print(newCardUID);
    Serial.print("\n");
    delay(1500);
  }
}

void loop() {
    // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent() )
    return;

  // Verify if the NUID has been readed
  if ( ! mfrc522.PICC_ReadCardSerial() )
    return;

  int cardUID = getCardID();

  Serial.print("R");
  Serial.print(cardUID);
  Serial.println("\n");
  delay(1000);

do {
  checkForNewCard();
int c; // do nothing
} while(recvOneChar() != 'S');

  lcdPrint(0 , " USE BUTTONS TO ");
  lcdPrint(1 , " SELECT A PARTY ");
  ledOn('G');
  buzzerOK();
  getVote();
  while(recvOneChar() != 'X'){
    int ac;
  }
}


bool confirmSelection(String vote , int pin){

  String text = vote + "  -> CONFIRM";
  String text2 = "BLACK -> CANCEL";
  lcdPrint(0 , text);
  lcdPrint(1 , text2);
  bool voteStatus;
  while(1){
    checkForNewCard();
    if(digitalRead(pin) == HIGH){
      lcdPrint(0 , "SUCCESS !");
      lcdPrint(1 , vote + " SELECTED");

      voteStatus = true;
      break;
    }
    else if(digitalRead(OK_BUTTON)==HIGH){
      lcdPrint(0 , " VOTE  CANCELLED");
      lcdPrint(1 , "  SELECT  AGAIN");
      voteStatus = false;
      break;
    }
    else
    continue;
  }
  return voteStatus;

}

void getVote(){
  while(1){
    checkForNewCard();
    if(digitalRead(R_BUTTON) == HIGH){
      lcdPrint(0 , "RED SELECTED");
      lcdPrint(1 , "Please wait...");
      delay(1000);
      if(confirmSelection("RED" , R_BUTTON) == true){
      lcdPrint(0 , "VOTE CONFIRMED");
      Serial.println("");
      Serial.print("VRED");
      Serial.print('\n');
      lcdPrint(1 , "serial write vred");
            delay(2000);
      lcdPrint(0 , "");
      //lcdPrint(1 , "");
        break;
      }
      else
      continue;
    }
    else if(digitalRead(G_BUTTON) == HIGH){
      lcdPrint(0 , "GREEN SELECTED");
      lcdPrint(1 , "Please wait...");
      delay(1000);
      if(confirmSelection("GREEN" , G_BUTTON) == true){
     lcdPrint(0 , "VOTE CONFIRMED");
     Serial.println("");
        Serial.print("VGREEN");
       Serial.print('\n');
       delay(2000);
       lcdPrint(0 , "");
       lcdPrint(1 , "");
        break;
      }
      else
      continue;
    }
    else if(digitalRead(B_BUTTON) == HIGH){
      lcdPrint(0 , "BLUE SELECTED");
      lcdPrint(1 , "Please wait...");
      delay(1000);
      if(confirmSelection("BLUE" , B_BUTTON) == true){
     lcdPrint(0 , "VOTE CONFIRMED");
     Serial.println("");
        Serial.print("VBLUE");
       Serial.print('\n');
       delay(2000);
       lcdPrint(0 , "");
       lcdPrint(1 , "");
        break;
      }
      else
      continue;
    }
    else if(digitalRead(W_BUTTON) == HIGH){
      lcdPrint(0 , "WHITE SELECTED");
      lcdPrint(1 , "Please wait...");
      delay(1000);
      if(confirmSelection("WHITE" , W_BUTTON) == true){
     lcdPrint(0 , "VOTE CONFIRMED");
     Serial.println("");
        Serial.print("VWHITE");
       Serial.print('\n');
       delay(2000);
       lcdPrint(0 , "");
       lcdPrint(1 , "");
        break;
      }
      else
      continue;
    }
    else
    continue;
  }
}
