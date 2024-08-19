

#include <mcp_can.h>
#include <SPI.h>
#define CAN0_INT 4   
MCP_CAN CAN0(10);     // Set CS to pin 10


//Variables Hiz okuma 
int _SensPin = 7; //arduino'da kullanılacak pin
int _VehicleSpeed =0;
int _CurrentTime; //Mevcut zaman
int _TimeLine = 0; //Hall en son ne zaman okundu
int _Period = 0;
int _MagnetSignal = 0;
bool _IsRead = 0;
int _ImmobilityTimer = 0;
static float _Radius = 0.0; //metre cinsinden radius değişkeni
static float _Pi = 3.14; //Pi sabit olarak 3,14 kalmalı
int canbusCounter = 0;
//

//




/*--------------------------------------------------------------Can-Bus Kodları----------------------------------------------------*/



byte convertSpeedtoByte(int num) {
  int result = 0;
  int digitCount = 1;
  while (num > 0) {
    result += (num % 2) * digitCount;
    digitCount *= 10;
    num /= 2;
  }
  return (byte)result;
}

void canbusSetup() {
  // Initialize MCP2515 running at 16MHz with a baudrate of 500kb/s and the masks and filters disabled.
  if (CAN0.begin(MCP_ANY, CAN_500KBPS, MCP_16MHZ) == CAN_OK) Serial.println("MCP2515 Initialized Successfully!");
  else Serial.println("Error Initializing MCP2515...");
  CAN0.setMode(MCP_NORMAL);   // Change to normal mode to allow messages to be transmitted
   
}
void canbusLoop(byte dataArray[1]) {
  // send data:  ID = 0x100, Standard CAN Frame, Data length = 8 bytes, 'data' = array of data bytes to send
  byte sndStat = CAN0.sendMsgBuf(0x110, 0, 1, dataArray);
  if (sndStat == CAN_OK) {
    Serial.println("Message Sent Successfully!");
  } else {
    Serial.println("Error Sending Message...");
  }
}

/*-------------------------------------------------------------Hız Okuma Kodları --------------------------------------------------*/
void VelocitySetup() {
  pinMode(_SensPin, INPUT);
  setRadius(0.29);
  _TimeLine = millis();
  
}



float detectSpeed(float circumfence, int period) { //Bu fonksiyon hareket halindeki aracın hızını hesaplar
  float tempSpeed = circumfence / period * 1000;
  return tempSpeed * 3.6; //converted to km/h from m/s
}

int detectPeriod() { //Bu fonksiyon hall sensörüne gelen her iki sinyal arası süreyi hesaplar.
  int period = 0;
  int temp = _TimeLine; //bir önceki süre saklandı.
  _TimeLine = millis(); // mevcut süre saklandı.
  period = _TimeLine - temp; // bir önceki süre  ve son ölçülen süre arasındaki fark hesaplandı
  return period;
}

float getCircumference() { //Bu fonksiyon tekerlek çevresini hesaplar.
  return _Radius * 2 * _Pi;
}

void setRadius(float radius) { //Bu fonksiyon radius değerini değiştirme amaçlı kullanılır
  _Radius = radius;
}




/************************************************************************************************************************************************************************/
void setup() {
  Serial.begin(115200);
  pinMode(_SensPin, INPUT);
  setRadius(0.29);
  _TimeLine = millis();
 // canbusSetup();
  Serial.println("Started");
}



//hatayı çözmek için: Eğer hız 1'den küçükse 0'dır. son...
void loop() {
  _MagnetSignal = digitalRead(_SensPin);

  if (_MagnetSignal == 0) { //hall okunduğu zaman if bloğuna giriş yapıldı
    _ImmobilityTimer = 5000;

    if (!_IsRead) { //Hall'dan sinyal alındığı anda gelen değeri kullanarak hız hesabı yapmak için giriş yapıldı

      _Period = detectPeriod();
      _VehicleSpeed = detectSpeed(getCircumference(), _Period);
      _IsRead = 1; //Hall sensöründe oluşan her manyetik alan için tek bir değer alınması hedeflendiğinden, magnetsignal 0 olana kadarki değerleri göz ardı etmek için 1'e çekildi.

     byte dataArray[1] = {byte(_VehicleSpeed)};

    //  canbusLoop(dataArray);
     
      Serial.println(_VehicleSpeed);
    }
  } else if (_ImmobilityTimer > 4) {
    _ImmobilityTimer = _ImmobilityTimer -  5;
  } else if (_ImmobilityTimer < 4) { //Araç duruyorsa hız 0'a çekildi. (hall 5 saniye okunmadıysa)
    _ImmobilityTimer = 5000;

    _VehicleSpeed = 0;

    byte dataArray[1] = {byte(_VehicleSpeed)};

     //   canbusLoop(dataArray);
     Serial.println(_VehicleSpeed);
   
   // Serial.println( String("0") + " Km / h");
  }

  if (_MagnetSignal != 0 && _IsRead != 0) { //Hall ve mıknatıs arasındaki bağlantı kesildiği anda IsRead tekrar 0'a çekildi. (Bir sonraki temasta tekrar ilk değeri alacak)
    _IsRead = 0;
 
  
  }

    

  
 delay(5);
  
}
