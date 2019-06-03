//LCD libraries
#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>
#define I2C_ADDR 0x26 // I2C address of the LCD display
#define BACKLIGHT_PIN 3
#define En_pin 2
#define Rw_pin 1
#define Rs_pin 0
#define D4_pin 4
#define D5_pin 5
#define D6_pin 6
#define D7_pin 7
//init. LCD display
LiquidCrystal_I2C lcd(I2C_ADDR, En_pin, Rw_pin, Rs_pin, D4_pin, D5_pin, D6_pin, D7_pin);
#define BUZ A3      //buzzer
#define G_BUTTON 4  //green button
#define W_BUTTON 5  //white button
#define OK_BUTTON 6 //black button
#define B_BUTTON 2  //blue button
#define R_BUTTON 3  //red button1
char receivedChar;  //the received chars from serial port is stored in this variable
const byte numChars = 32;
char receivedChars[numChars];
boolean newData = false;
void lcdClearLine(int line)
{
   //this fn. cleans the desired line on the lcd display
   lcd.setCursor(0, line);
   String blank;
   for (int i = 1; i <= 16; i++)
   {
      blank += " ";
   }
   lcd.print(blank);
}
void lcdPrint(int line, String text)
{
   //shortcut fn. to print text on the lcd display
   //it first cleans the line and then prints the text
   lcdClearLine(line);
   lcd.setCursor(0, line);
   lcd.print(text);
}
void buzzerOK()
{
   //fn. to activate buzzer in a sequence
   digitalWrite(BUZ, HIGH);
   delay(150);
   digitalWrite(BUZ, LOW);
   delay(150);
   digitalWrite(BUZ, HIGH);
   delay(150);
   digitalWrite(BUZ, LOW);
}
char recvOneChar()
{
   //this fn. is used to receive a single char
   //that is printed on the serial
   if (Serial.available() > 0)
   {
      //if a new char is received, it is stored in 'receivedChar'
      //and the fn. returns to 'receivedChar'
      receivedChar = Serial.read();
      newData = true;
      return receivedChar;
   }
}
void recvWithStartEndMarkers()
{
   //this fn. is used to receive multiple character messages
   //which is enclosed by '<' and '>'
   static boolean recvInProgress = false;
   static byte ndx = 0;
   char startMarker = '<';
   char endMarker = '>';
   char rc;
   while (Serial.available() > 0 && newData == false)
   {
      rc = Serial.read();
      if (recvInProgress == true)
      {
         if (rc != endMarker)
         {
            receivedChars[ndx] = rc;
            ndx++;
            if (ndx >= numChars)
            {
               ndx = numChars - 1;
            }
         }
         else
         {
            receivedChars[ndx] = '\0'; // terminate the string
            recvInProgress = false;
            ndx = 0;
            newData = true;
         }
      }
      else if (rc == startMarker)
      {
         recvInProgress = true;
      }
   }
}
void setup()
{
   pinMode(BUZ, OUTPUT);
   pinMode(B_BUTTON, INPUT);
   pinMode(G_BUTTON, INPUT);
   pinMode(B_BUTTON, INPUT);
   pinMode(W_BUTTON, INPUT);
   pinMode(OK_BUTTON, INPUT);
   Serial.begin(19200); // Init. serial comm. with the PC
   lcd.begin(16, 2);
   lcd.setBacklightPin(BACKLIGHT_PIN, POSITIVE);
   lcd.setBacklight(HIGH);
   lcd.home();
   lcd.print(0, "- ELECTION 18' -");
   lcdPrint(1, "Waiting for Card ");
}
char canStart;
void loop()
{
   while (true)
   {
      recvWithStartEndMarkers();
      String message(receivedChars);
      // lcdPrint(0 , receivedChars);
      if (message == "start")
      {
         // receivedChars[0] = '0';
         break;
      }
      else
      {
         lcd.setCursor(0, 0);
         lcd.print("- ELECTION 18' -");
         lcd.setCursor(0, 1);
         lcd.print("Waiting for card");
         continue;
      }
   }
   lcdPrint(0, " USE BUTTONS TO ");
   lcdPrint(1, " SELECT A PARTY ");
   buzzerOK();
   getVote();
   lcdPrint(0, "- ELECTION 18' -");
   lcdPrint(1, "Waiting for Card ");
   newData = false;
   receivedChars[0] =
       0;
}
bool confirmSelection(String vote, int pin)
{
   String text = vote + " -> CONFIRM";
   String text2 = "BLACK -> CANCEL";
   lcdPrint(0, text);
   lcdPrint(1, text2);
   bool voteStatus;
   while (1)
   {
      if (digitalRead(pin) == HIGH)
      {
         lcdPrint(
             0, "SUCCESS !");
         lcdPrint(
             1, vote + " SELECTED");
         voteStatus = true;
         break;
      }
      else if (digitalRead(OK_BUTTON) == HIGH)
      {
         lcdPrint(0, " VOTE CANCELLED");
         lcdPrint(1, " SELECT AGAIN");
         voteStatus = false;
         break;
      }
      else
         continue;
   }
   return voteStatus;
}
void getVote()
{
   while (1){
      if (digitalRead(R_BUTTON) == HIGH)
      {
         lcdPrint(0, "RED SELECTED");
         lcdPrint(1, "Please wait...");
         delay(1000);
         if (confirmSelection("RED", R_BUTTON) == true)
         {
            lcdPrint(0, "VOTE CONFIRMED");
            Serial.print("VRED");
            Serial.print('\n');
            delay(2000);
            lcdPrint(0, "");
            lcdPrint(1, "");
            break;
         }
         else
            continue;
      }
      else if (digitalRead(G_BUTTON) == HIGH)
      {
         lcdPrint(
             0, "GREEN SELECTED");
         lcdPrint(
             1, "Please wait...");
         delay(1000);
         if (confirmSelection("GREEN", G_BUTTON) == true)
         {
            lcdPrint(0, "VOTE CONFIRMED");
            Serial.print("VGREEN");
            Serial.print('\n');
            delay(2000);
            lcdPrint(0, "");
            lcdPrint(1, "");
            break;
         }
         else
            continue;
      }
      else if (digitalRead(B_BUTTON) == HIGH)
      {
         lcdPrint(0, "BLUE SELECTED");
         lcdPrint(1, "Please wait...");
         delay(1000);
         if (confirmSelection("BLUE", B_BUTTON) == true)
         {
            lcdPrint(0, "VOTE CONFIRMED");
            Serial.print("VBLUE");
            Serial.print('\n');
            delay(2000);
            lcdPrint(0, "");
            lcdPrint(1, "");
            break;
         }
         else
            continue;
      }
      else if (digitalRead(W_BUTTON) == HIGH)
      {
         lcdPrint(0, "BLANK SELECTED");
         lcdPrint(1, "Please wait...");
         delay(1000);
         if (confirmSelection("BLANK", W_BUTTON) == true)
         {
            lcdPrint(0, "VOTE CONFIRMED");
            Serial.print("VBLANK");
            Serial.print('\n');
            delay(2000);
            lcdPrint(0, "");
            lcdPrint(1, "");
            break;
         }
         else
            continue;
      }
      else
         continue;
   }
}