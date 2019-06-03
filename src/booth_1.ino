//RFID reader libraries
#include <MFRC522Hack.h>
#include <MFRC522Extended.h>
#include <deprecated.h>
#include <MFRC522Debug.h>
#include <require_cpp11.h>
#include <MFRC522.h>

//LCD libraries
#include <SPI.h>
#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>

#define I2C_ADDR 0x27 // I2C address of the LCD display
#define BACKLIGHT_PIN 3
#define En_pin 2
#define Rw_pin 1
#define Rs_pin 0
#define D4_pin 4
#define D5_pin 5
#define D6_pin 6
#define D7_pin 7

//init. LCD display
LiquidCrystal_I2C lcd(I2C_ADDR,En_pin,Rw_pin,Rs_pin,D4_pin,D5_pin,D6_pin,D7_pin);
#define SS_PIN 10
#define RST_PIN 9

#define BUZ A3 //buzzer
#define G_BUTTON 4 //green button
#define W_BUTTON 5 //white button
#define OK_BUTTON 6 //black button
#define B_BUTTON 2 //blue button
#define R_BUTTON 3 //red button1

char receivedChar; //the received chars from serial port is stored in this variable
boolean newData = false;

MFRC522 mfrc522(SS_PIN , RST_PIN);

void lcdClearLine(int line){
 //this fn. cleans the desired line on the lcd display
    lcd.setCursor(0 , line);
    String blank;
    for(int i = 1 ; i <= 16 ; i++){
        blank += " ";
    }
    lcd.print(blank);
}

void lcdPrint(int line , String text){
    //shortcut fn. to print text on the lcd display
    //it first cleans the line and then prints the text
    lcdClearLine(line);
    lcd.setCursor(0 , line);
    lcd.print(text);
}

void buzzerOK(){
    //fn. to activate buzzer in a sequence
    digitalWrite(BUZ,HIGH);
    delay(150);
    digitalWrite(BUZ,LOW);
    delay(150);
    digitalWrite(BUZ,HIGH);
    delay(150);
    digitalWrite(BUZ,LOW); 
}
char recvOneChar() {
    //this fn. is used to receive a single char
    //that is printed on the serial
    if (Serial.available() > 0) {
        //if a new char is received, it is stored in 'receivedChar'
        //and the fn. returns to 'receivedChar'
        receivedChar = Serial.read();
        newData = true;
        return receivedChar;
    }
}

unsigned long getCardID(int showCardData = 0){
    /* this function gets the uid of the rfid card
    and returns the uid in unsigned long format */
    /* this fn. should be called with argument 1
    if printing the card data to the serial monitor
    is needed. */
    unsigned long UID_unsigned;
    UID_unsigned = mfrc522.uid.uidByte[0] << 24;
    UID_unsigned += mfrc522.uid.uidByte[1] << 16;
    UID_unsigned += mfrc522.uid.uidByte[2] << 8;
    UID_unsigned += mfrc522.uid.uidByte[3];
    if(showCardData==1){
        Serial.println("UID Unsigned int");
        Serial.println(UID_unsigned);
        String UID_string = (String)UID_unsigned;
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
    pinMode(BUZ , OUTPUT);
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
    //checks if there is an RFID card is present in the field of the reader
    if ( mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial() ){
    //if a card is present, it gets the card UID and prints to the serial ports
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
    buzzerOK();
    getVote(); //call the fn. to start the voting process
    while(recvOneChar() != 'X'){
        int ac;
    }
}

bool confirmSelection(String vote , int pin){
    /*this fn. asks user to confirm his selection
    by pushing the same button again, or to cancel
    by pushing the black button.*/
    String text = vote + " -> CONFIRM";
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
            lcdPrint(0 , " VOTE CANCELLED");
            lcdPrint(1 , " SELECT AGAIN");
            voteStatus = false;
            break;
        }
        else
            continue;
    }
    return voteStatus;
}

void getVote(){
    /* this fn. waits for the button clicks
    after a selection is confirmed, it prints the vote
    information to serial port (sends to python script)*/
    while(1){
        checkForNewCard(); //while waitin for buttons, also check for new cards
        if(digitalRead(R_BUTTON) == HIGH){
            lcdPrint(0 , "RED SELECTED");
            lcdPrint(1 , "Please wait...");
            delay(1000);
            if(confirmSelection("RED" , R_BUTTON) == true){ //if selection is confirmed
                lcdPrint(0 , "VOTE CONFIRMED");
                Serial.println("");
                Serial.print("VRED");
                Serial.print('\n');
                delay(2000);
                lcdPrint(0 , "");
                //lcdPrint(1 , "");
                break;
            }
            else
                continue;
        }
        else if(digitalRead(G_BUTTON) == HIGH) {
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
        else if(digitalRead(B_BUTTON) == HIGH) {
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
        else if(digitalRead(W_BUTTON) == HIGH) {
            lcdPrint(0 , "BLANK SELECTED");
            lcdPrint(1 , "Please wait...");
            delay(1000);
            if(confirmSelection("BLANK" , W_BUTTON) == true){
                lcdPrint(0 , "VOTE CONFIRMED");
                Serial.println("");
                Serial.print("VBLANK");
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