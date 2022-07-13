// MAC Address of responder - edit as required
// uint8_t broadcastAddress[] = {0xC8, 0xC9, 0xA3, 0xF9, 0x0E, 0xF4}; 0x16, 0xD0}
 
 //ESP32
 
#include <esp_now.h>
#include <WiFi.h>

// MAC Address of responder - edit as required
uint8_t broadcastAddress[] = {0xC8, 0xC9, 0xA3, 0xF9, 0x16, 0xD0};
  
 //Displays
 
//SSD1306 libraries
#include <SPI.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Wire.h>

#define OLED_RESET -1 // 4 ANGUS
//Adafruit_SSD1306 display(OLED_RESET);

// OLED I2C bus address
#define OLED_address  0x3c //0x3c 
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);//ANGUS

// ST7789 libraries//

#include <SPI.h>            // for use with ST7789 using TFT_eSPI library
#include <TFT_eSPI.h>       // Hardware-specific library
TFT_eSPI tft = TFT_eSPI();  // Invoke custom library

// This CODE has been updated to use 2 encoders with the built-in switch and 
// posts to the serial monitor. 
 
 // Rotary Encoder Inputs
 #define CLK1 33 //encoder 1
 #define DT1 25  
 #define SW1 26
 
#define CLK2 34 //encoder 2
#define DT2 35
#define SW2 32

// Current and previous value of the counter tuned by the rotary
int curVal1 = 0;
int prevVal1 = 0;

int curVal2 = 0;
int prevVal2 =0;

// Seven states of FSM A (finite state machine)
#define IDLE_11 0
#define SCLK_01 1
#define SCLK_00 2
#define SCLK_10 3
#define SDT_10 4
#define SDT_00 5
#define SDT_01 6
int Astate = IDLE_11;

int Bstate = IDLE_11;

/*
// DATA STRUCTURE TRANSMIT // 

typedef struct TxStruct     // Define a data structure
{
  int OPPcurrentStateCLK1;
  int OPPcurrentStateCLK2;

int OPPcurrentStateDT1;
int OPPcurrentStateDT2; 
 
  int OPPcurrentStateSW1;
  int OPPcurrentStateSW2;
  
 } TxStruct;

// Create a structured object
TxStruct sentData;

// Peer info
esp_now_peer_info_t peerInfo;

// Callback function called when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
//  Serial.print("\r\nLast Packet Send Status:\t");
//  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}
// DATA STRUCTURE RECEIVE // 

typedef struct RxStruct
{
  int OPPcurrentStateCLK1;
  int OPPcurrentStateCLK2;

  int OPPcurrentStateDT1;
  int OPPcurrentStateDT2; 
 
  int OPPcurrentStateSW1;
  int OPPcurrentStateSW2;
  
}RxStruct;
RxStruct receivedData;

//-------------------------------------------------------------------------------------
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len)
{
  memcpy(&receivedData, incomingData, sizeof(receivedData));
}
//======================================================================================
*/
 void setup() { 

// Setup Serial Monitor and encoders
//  Serial.begin(250000);
//  Serial.println("Start...");

// Level HIGH will be default for all pins
  pinMode(SW1,INPUT_PULLUP);
  pinMode(CLK1,INPUT_PULLUP);
  pinMode(DT1,INPUT_PULLUP);

  pinMode(SW2,INPUT_PULLUP);
  pinMode(CLK2,INPUT_PULLUP);
  pinMode(DT2,INPUT_PULLUP);
  
// Both CLK and DT will trigger interrupts for all level changes
  attachInterrupt(digitalPinToInterrupt(CLK1), rotaryCLK1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(DT1), rotaryDT1, CHANGE);

  attachInterrupt(digitalPinToInterrupt(CLK2), rotaryCLK2, CHANGE);
  attachInterrupt(digitalPinToInterrupt(DT2), rotaryDT2, CHANGE);
//END encoders

// SET-UP CODE for SSD1306
/* Wire.begin();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3c);  // initialize with the I2C addr 0x3D (for the 128x64)
  display.clearDisplay();   // clears the screen and buffer 
          // end code for SSD1306
*/

// SET-UP CODE for ST7789
 tft.init();
 tft.setRotation(1);
 tft.fillScreen(TFT_BLACK);
 
// tft.setCursor(100, 0, 4);            // Code replaced by OWN and OPP
// tft.setTextColor(TFT_WHITE, TFT_BLACK);
// tft.println("LOCAL");
 
 tft.setCursor(0, 40, 4);
 tft.setTextColor(TFT_WHITE, TFT_BLACK);
 tft.println("dir: "); 
   
  tft.setCursor(90, 40, 4);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.println("val: ");  

  tft.setCursor(180, 40, 4);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.println("sw: ");  


 tft.setCursor(0, 80, 4);
 tft.setTextColor(TFT_WHITE, TFT_BLACK);
 tft.println("dir: "); 
   
  tft.setCursor(90, 80, 4);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.println("val: ");  

  tft.setCursor(180, 80, 4);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.println("sw: ");  

/*
// SET-UP CODE for ESP32 wifi
 WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK)
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_recv_cb(OnDataRecv);

  // Register the send callback
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }     */
} 
 
 void loop() { 

//code for encoder1 
// Handling of the optional switch integrated into some rotary encoders
  if (digitalRead(SW1)==LOW) {
    Serial.println("Pressed");
    while(!digitalRead(SW1));
  }
// Any change in counter value is displayed in Serial Monitor
  if (curVal1 != prevVal1) {
    Serial.println(curVal1);
    prevVal1 = curVal1;
  }
}
// State Machine transitions for CLK level changes
void rotaryCLK1() {
  if (digitalRead(CLK1)==LOW) {
    if (Astate==IDLE_11) Astate = SCLK_01;
    else if (Astate==SCLK_10) Astate = SCLK_00;
    else if (Astate==SDT_10) Astate = SDT_00;
  }
  else {
    if (Astate==SCLK_01) Astate = IDLE_11;
    else if (Astate==SCLK_00) Astate = SCLK_10;
    else if (Astate==SDT_00) Astate = SDT_10;
    else if (Astate==SDT_01) { Astate = IDLE_11; curVal1--; }
  }
}
// State Machine transitions for DT level changes
void rotaryDT1() {
  if (digitalRead(DT1)==LOW) {
    if (Astate==IDLE_11) Astate = SDT_10;
    else if (Astate==SDT_01) Astate = SDT_00;
    else if (Astate==SCLK_01) Astate = SCLK_00;
  }
  else {
    if (Astate==SDT_10) Astate = IDLE_11;
    else if (Astate==SDT_00) Astate = SDT_01;
    else if (Astate==SCLK_00) Astate = SCLK_01;
    else if (Astate==SCLK_10) { Astate = IDLE_11; curVal1++; }
  }
///////Encoder 2
// Handling of the optional switch integrated into some rotary encoders
  if (digitalRead(SW2)==LOW) {
    Serial.println("Pressed");
    while(!digitalRead(SW2));
  }
// Any change in counter value is displayed in Serial Monitor
  if (curVal2 != prevVal2) {
    Serial.println(curVal2);
    prevVal2 = curVal2;
  }
}
// State Machine transitions for CLK level changes
void rotaryCLK2() {
  if (digitalRead(CLK2)==LOW) {
    if (Bstate==IDLE_11) Bstate = SCLK_01;
    else if (Bstate==SCLK_10) Bstate = SCLK_00;
    else if (Bstate==SDT_10) Bstate = SDT_00;
  }
  else {
    if (Bstate==SCLK_01) Bstate = IDLE_11;
    else if (Bstate==SCLK_00) Bstate = SCLK_10;
    else if (Bstate==SDT_00) Bstate = SDT_10;
    else if (Bstate==SDT_01) { Bstate = IDLE_11; curVal2--; }
  }
}
// State Machine transitions for DT level changes
void rotaryDT2() {
  if (digitalRead(DT2)==LOW) {
    if (Bstate==IDLE_11) Bstate = SDT_10;
    else if (Bstate==SDT_01) Bstate = SDT_00;
    else if (Bstate==SCLK_01) Bstate = SCLK_00;
  }
  else {
    if (Bstate==SDT_10) Bstate = IDLE_11;
    else if (Bstate==SDT_00) Bstate = SDT_01;
    else if (Bstate==SCLK_00) Bstate = SCLK_01;
    else if (Bstate==SCLK_10) { Bstate = IDLE_11; curVal2++; }
  }  

//*********** graphic display SSD1306*******//
//OWN 
/*
  display.setTextColor(WHITE); display.clearDisplay();                   // code for SSD1306         
  display.setTextSize(2); display.setCursor(40,0); display.print("OWN");
  
  display.setTextSize(1); display.setCursor(0,20); display.print("dir1: ");
  display.setTextSize(1); display.setCursor(30,20); display.print(encdir1);//(receivedData.potValR);
  display.setTextSize(1); display.setCursor(55,20); display.print("val: ");
  display.setTextSize(1); display.setCursor(80,20); display.print(counter1);//(receivedData.potValR);
  display.setTextSize(1); display.setCursor(100,20); display.print("SW: ");
  display.setTextSize(1); display.setCursor(120,20); display.print(currentStateSW1);//(receivedData.potValR);

  display.setTextSize(1); display.setCursor(0,40); display.print("dir2: ");
  display.setTextSize(1); display.setCursor(30,40); display.print(encdir2);//(receivedData.potValR);
  display.setTextSize(1); display.setCursor(55,40); display.print("val: ");
  display.setTextSize(1); display.setCursor(80,40); display.print(counter2);//(receivedData.potValR); 
  display.setTextSize(1); display.setCursor(100,40); display.print("SW: ");
  display.setTextSize(1); display.setCursor(120,40); display.print(currentStateSW2);//(receivedData.potValR);
  
  display.display();  
*/
// OPP
/*
  display.setTextColor(WHITE); display.clearDisplay();                   // code for SSD1306         
  display.setTextSize(2); display.setCursor(40,0); display.print("OPP");
  
  display.setTextSize(1); display.setCursor(0,20); display.print("dir1: ");
  display.setTextSize(1); display.setCursor(30,20); display.print(OPPencdir1);//(receivedData.potValR);
  display.setTextSize(1); display.setCursor(55,20); display.print("val: ");
  display.setTextSize(1); display.setCursor(80,20); display.print(OPPcounter1);//(receivedData.potValR);
  display.setTextSize(1); display.setCursor(100,20); display.print("SW: ");
  display.setTextSize(1); display.setCursor(120,20); display.print(OPPcurrentStateSW1);//(receivedData.potValR);

  display.setTextSize(1); display.setCursor(0,40); display.print("dir2: ");
  display.setTextSize(1); display.setCursor(30,40); display.print(OPPencdir2);//(receivedData.potValR);
  display.setTextSize(1); display.setCursor(55,40); display.print("val: ");
  display.setTextSize(1); display.setCursor(80,40); display.print(OPPcounter2);//(receivedData.potValR); 
  display.setTextSize(1); display.setCursor(100,40); display.print("SW: ");
  display.setTextSize(1); display.setCursor(120,40); display.print(OPPcurrentStateSW2);//(receivedData.potValR);
  
  display.display();  
*/
//******* Active code ST7789 display*********//
//OWN 

 tft.setCursor(100, 0, 4);
 tft.setTextColor(TFT_WHITE, TFT_BLACK);
 tft.println("OWN");
 
 //tft.setCursor(40,40, 2); 
// tft.setTextColor(TFT_RED, TFT_BLACK);
// tft.println(encdir1);
 
tft.setCursor(130, 40, 4); 
 tft.setTextColor(TFT_RED, TFT_BLACK);
 tft.println(curVal1);

 tft.setCursor(220, 40, 4); 
 tft.setTextColor(TFT_RED, TFT_BLACK);
 tft.println(digitalRead(SW1));

// tft.setCursor(40,80, 2); 
// tft.setTextColor(TFT_GREEN, TFT_BLACK);
// tft.println(encdir2);
 
tft.setCursor(130, 80, 4); 
 tft.setTextColor(TFT_GREEN, TFT_BLACK);
 tft.println(curVal2);


 tft.setCursor(220, 80, 4); 
 tft.setTextColor(TFT_GREEN, TFT_BLACK);
 tft.println(digitalRead(SW2));
/*
// OPP

 tft.setCursor(100, 0, 4);
 tft.setTextColor(TFT_WHITE, TFT_BLACK);
 tft.println("OPP");
 
 tft.setCursor(40,40, 2); 
 tft.setTextColor(TFT_RED, TFT_BLACK);
 tft.println(OPPencdir1);
 
tft.setCursor(130, 40, 4); 
 tft.setTextColor(TFT_RED, TFT_BLACK);
 tft.println(OPPcounter1);

tft.setCursor(220, 40, 4); 
 tft.setTextColor(TFT_RED, TFT_BLACK);
 tft.println(OPPcurrentStateSW1);

 tft.setCursor(40,80, 2); 
 tft.setTextColor(TFT_GREEN, TFT_BLACK);
 tft.println(OPPencdir2);
 
tft.setCursor(130, 80, 4); 
 tft.setTextColor(TFT_GREEN, TFT_BLACK);
 tft.println(OPPcounter2);


tft.setCursor(220, 80, 4); 
 tft.setTextColor(TFT_GREEN, TFT_BLACK);
 tft.println(OPPcurrentStateSW2);
   */
 }
