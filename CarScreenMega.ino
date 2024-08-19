#include "Nextion.h"
#include <SoftwareSerial.h>
#include <mcp_can.h>
#include <SPI.h>
#include <LoRa.h>
/*----------------------*/
long unsigned int rxId;
unsigned char len = 0;
unsigned char rxBuf[8];
char msgString[128];                      // Array to store serial string
#define CAN0_INT 4                        // Set INT to pin 2
int canCS = 8;
MCP_CAN CAN0(canCS);      // Set CS to pin 10 //47 yani //hayır hayır 8
#define nss 10
#define rst 9
#define dio0 2
/*----------------------------------------------------------------------------Ekran Kodu-------------------------------------------------------------------------*/
/*
 * hız = 0x110
 * batarya sıcaklık  = 0x131
 * batarya gerilim = 0x132
 * motor gerilim = 0x133
 * akım = 0x134
 * SoC = 0x135
 * izolasyon izleme = 0x136
 */ 


int i = 0;
int arabahiz=0;
int izo = 0;

float pilsicaklik = 3270;
float pilgerilim = 11200;
float akim = 0000;
float pilmiktari = 3000;  //SoC


  void hizPopCallback(void *ptr); // n0 hız demek.
  void x0PopCallback(void *ptr);
  void x1PopCallback(void *ptr);
  void x2PopCallback(void *ptr);
  void x4PopCallback(void *ptr);
  void x5PopCallback(void *ptr);
  
//    Declare a number object (page id:1, component id:4, component name "n0").


NexNumber hiz = NexNumber(1, 5, "hiz");
NexNumber x0 = NexNumber(0, 6, "x0");
NexNumber x1 = NexNumber(0, 7, "x1");
NexNumber x2 = NexNumber(0, 8, "x2");
NexNumber x4 = NexNumber(0, 10, "x4");
NexNumber x5 = NexNumber(1, 7, "x5");


//    Register object n0, b0, b1, to the touch event list.



  NexTouch *nex_listen_list[] =
  {
  &hiz,
  &x0,
  &x1,
  &x2,
  &x4,
  &x5,
  NULL
  };

  
//  number component pop callback function.


  void hizPopCallback(void *ptr)
  {
  dbSerialPrintln("hizPopCallback");
  }





  void x0PopCallback(void *ptr)
  {
  dbSerialPrintln("x0PopCallback");
  }




  void x1PopCallback(void *ptr)
  {
  dbSerialPrintln("x1PopCallback");
  }



  void x2PopCallback(void *ptr)
  {
  dbSerialPrintln("x2PopCallback");
  }
  void x4PopCallback(void *ptr)
  {
  dbSerialPrintln("x4PopCallback");
  }
  void x5PopCallback(void *ptr)
  {
  dbSerialPrintln("x5PopCallback");
  }
/*----------------------------------------------------------------------------Ekran Kodu-------------------------------------------------------------------------*/

int decimal(char hex [])
{
  long long decimal = 0, base = 1;
  int i = 0, value, length;

  length = strlen(hex);
  for (i = length--; i >= 0; i--)
  {
    if (hex[i] >= '0' && hex[i] <= '9')
    {
      decimal += (hex[i] - 48) * base;
      base *= 16;
    }
    else if (hex[i] >= 'A' && hex[i] <= 'F')
    {
      decimal += (hex[i] - 55) * base;
      base *= 16;
    }
    else if (hex[i] >= 'a' && hex[i] <= 'f')
    {
      decimal += (hex[i] - 87) * base;
      base *= 16;
    }
  }

  return decimal;
}

/*********************************************************************************************************Canbus Receiver*************************************************************************************/

void setup()
{
  Serial.begin(250000);
  
    nexInit();

    //Register the pop event callback function of the current number component.
    hiz.attachPop(hizPopCallback);
    x0.attachPop(x0PopCallback);
    x1.attachPop(x1PopCallback);
    x2.attachPop(x2PopCallback);
    x4.attachPop(x4PopCallback);
    x5.attachPop(x5PopCallback);

    dbSerialPrintln("setup done");


    
    pinMode(canCS, OUTPUT);

   
    
     // Initialize MCP2515 running at 16MHz with a baudrate of 500kb/s and the masks and filters disabled.
    if(CAN0.begin(MCP_ANY, CAN_500KBPS, MCP_16MHZ) == CAN_OK)
      Serial.println("MCP2515 Initialized Successfully!");
    else
      Serial.println("Error Initializing MCP2515...");

     pinMode(CAN0_INT, INPUT);    


    
    Serial.println("MCP2515 Receive Example...");
    CAN0.setMode(MCP_NORMAL);                     // Set operation mode to normal so the MCP2515 sends acks to received data.


    pinMode(nss, OUTPUT);
    
  //Lora
  
  while (!Serial);

  Serial.println("LoRa Sender");

  LoRa.setPins(nss, rst, dio0);
   
  if (!LoRa.begin(915E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  LoRa.setSpreadingFactor(10);
 
 LoRa.setSignalBandwidth(62.5E3);
 
 LoRa.crc();
}

struct CanMessage{
  String ID;
  int data; 
}cnm;


void loop()
{
      
      
    
    if(!digitalRead(CAN0_INT))                         // If CAN0_INT pin is low, read receive buffer
    {
       CAN0.readMsgBuf(&rxId, &len, rxBuf);      // Read data: len = data length, buf = data byte(s)
    
    if((rxId & 0x80000000) == 0x80000000)     // Determine if ID is standard (11 bits) or extended (29 bits)
      sprintf(msgString, "Extended ID: 0x%.8lX  DLC: %1d  Data:", (rxId & 0x1FFFFFFF), len);
    else
      sprintf(msgString, "Standard ID:0x%.3lX;DLC:%1d;Data:", rxId, len);
    
  
    Serial.print(msgString);
    
    if((rxId & 0x40000000) == 0x40000000){    // Determine if message is a remote request frame.
      sprintf(msgString, " REMOTE REQUEST FRAME");
      Serial.print(msgString);
    } else {
        
      for(byte i = 0; i<len; i++){
        sprintf(msgString, " 0x%.2X", rxBuf[i]);
        Serial.print(sprintf(msgString, " 0x%.2X", rxBuf[i]));
    }
    
    }
  
}   
    

    


    // ---------Data Convert1
    char hexa[2];
    for(int i = 0; i<128; i++){
      if(i>2 && i<5){
      hexa[i-3] = msgString[i];
      }
    }
    String ID= "";
    for(int j=0; j<128; j++){
      if(j>13 && j<17){
        ID= ID + (String) msgString[j];
      }
    }
     cnm.ID= ID;
     cnm.data =  decimal(hexa); 

if(cnm.ID == "110"){
  arabahiz = cnm.data;
}else if(cnm.ID == "000"){
  Serial.println("parazit var!");
}
if(cnm.ID == "136"){
  izo= cnm.data;
}





  
     Serial.println("gelenveri:" + cnm.ID+" "+ cnm.data);

  // send packet
  LoRa.beginPacket();

  String h = String((arabahiz)*100) + "," + String(pilsicaklik) + "," + String(pilgerilim) + "," + String(akim) + "," + String(pilmiktari+i) + "," + String(izo) ;
    //String h= "55555";
 // Serial.println(h);

  LoRa.print(h);  //hız
  Serial.println(h);

  LoRa.endPacket();


    if(i>15*25){
    pilsicaklik = pilsicaklik + 5;
    pilgerilim = pilgerilim + 2 ;
    akim = akim + 0;
    pilmiktari = pilmiktari - 0.2 ;
    i=0;
    }
    i= i+10;

    hiz.setValue(arabahiz*100);
    x0.setValue(pilsicaklik);  // pil sıcaklık
    x1.setValue(pilgerilim); //  pil gerilim
    x2.setValue(akim);   //  motor sıcaklık
    x4.setValue(pilmiktari+i);  //  SoC
    x5.setValue(izo); //izolasyon izleme cihazı

   

  
delay(5);
}
