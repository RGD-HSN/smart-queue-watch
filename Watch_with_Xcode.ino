#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);



#define JOIN_BUTTON 18
#define SOS_BUTTON 19
#define BUZZER 23


uint8_t currentRide = 0;



bool inQueue = false;

uint8_t currentRideID = 0;

unsigned long joinTime = 0;    
bool turnShown = false;       


unsigned long lastUpdate = 0;
const unsigned long updateInterval = 5000; 

bool queueActive = false;


BLEServer *pServer = NULL;
BLECharacteristic *pTxCharacteristic;

bool deviceConnected = false;
bool oldDeviceConnected = false;

uint8_t value[2] = {0,0};

#define SERVICE_UUID "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define RX_UUID      "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define TX_UUID      "6E400003-B5A3-F393-E0A9-E50E24DCCA9A"




bool joinPressed = false;
bool sosPressed = false;

unsigned long joinPressTime = 0;

bool joined = false;





void beep(int times)
{
  for(int i=0;i<times;i++)
  {
    tone(BUZZER, 2000);
    delay(150);
    noTone(BUZZER);
    delay(150);
  }
}




void showMessage(String msg)
{
  display.clearDisplay();

  display.setTextSize(2);
  display.setTextColor(WHITE);

  display.setCursor(0,20);
  display.println(msg);

  display.display();
}




void handleJoin(uint8_t rideID)
{
  inQueue = true;
  currentRideID = rideID;

  joinTime = millis();  
  turnShown = false;    

  beep(2);
  showMessage("Ride Joined");  
  Serial.println("Joined ride");
}


void handleCancel()
{
  inQueue = false;

  beep(1);

  showMessage("Cancelled");

  Serial.println("Cancelled ride");
}


void handleSOS()
{
  Serial.println("SOS");

  showMessage("SOS SENT");
  beep(4);

  value[0] = 0x03;
}




class MyServerCallbacks: public BLEServerCallbacks
{
  void onConnect(BLEServer* pServer)
  {
    deviceConnected = true;
    Serial.println("Client Connected");
  };

  void onDisconnect(BLEServer* pServer)
  {
    deviceConnected = false;
    Serial.println("Client Disconnected");
  }
};

void sendRideUpdate(uint8_t rideId, uint8_t pos, uint8_t wait)
{
  uint8_t data[4];

  data[0] = 0x10;
  data[1] = rideId;
  data[2] = pos;
  data[3] = wait;

  pTxCharacteristic->setValue(data, 4);
  pTxCharacteristic->notify();
}





class MyCallbacks: public BLECharacteristicCallbacks
{
  void onWrite(BLECharacteristic *pCharacteristic)
  {
    std::string rxValue = pCharacteristic->getValue();

    if(rxValue.length() > 0)
    {
      uint8_t cmd = rxValue[0];

      Serial.print("Received: ");
      Serial.println(cmd);

      if(cmd == 0x01) handleJoin(1);
      if(cmd == 0x03) handleCancel();
      if(cmd == 0x02) handleSOS();
    }
  }
};






void setup()
{
  Serial.begin(115200);

  pinMode(JOIN_BUTTON, INPUT);
  pinMode(SOS_BUTTON, INPUT);

  pinMode(BUZZER, OUTPUT);



  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println("OLED failed");
    while(true);
  }

  showMessage("Starting...");
  delay(1000);


  
  BLEDevice::init("Smart Queue Watch");

  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);

  pTxCharacteristic =
    pService->createCharacteristic(
      TX_UUID,
      BLECharacteristic::PROPERTY_NOTIFY
    );

  pTxCharacteristic->addDescriptor(new BLE2902());

  BLECharacteristic *pRxCharacteristic =
    pService->createCharacteristic(
      RX_UUID,
      BLECharacteristic::PROPERTY_WRITE
    );

  pRxCharacteristic->setCallbacks(new MyCallbacks());

  pService->start();

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);

  BLEDevice::startAdvertising();

  Serial.println("BLE Ready");

  showMessage("Ready");
}




void handleButtons()
{
  bool joinState = digitalRead(JOIN_BUTTON);
  bool sosState  = digitalRead(SOS_BUTTON);


 
  if(joinState && !joinPressed)
  {
    joinPressed = true;
    joinPressTime = millis();
  }

  if(!joinState && joinPressed)
  {
    joinPressed = false;

    if(millis() - joinPressTime > 1000)
      handleCancel();
    else
      handleJoin(1);
  }



  if(sosState && !sosPressed)
  {
    sosPressed = true;
    handleSOS();
  }

  if(!sosState)
    sosPressed = false;
}




void loop()
{
  handleButtons();



  if(deviceConnected)
  {
    pTxCharacteristic->setValue(value, 2);
    pTxCharacteristic->notify();
    delay(200);
  }


  if (!deviceConnected && oldDeviceConnected)
  {
    delay(500);
    BLEDevice::startAdvertising();
    oldDeviceConnected = deviceConnected;
  }

  if (deviceConnected && !oldDeviceConnected)
  {
    oldDeviceConnected = deviceConnected;
  }
  if(deviceConnected && queueActive)
{

  if(inQueue && millis() - lastUpdate > updateInterval)
{
  lastUpdate = millis();

}

}
{
  handleButtons();


  if(deviceConnected)
  {
    pTxCharacteristic->setValue(value, 2);
    pTxCharacteristic->notify();
    delay(200);
  }


  if(inQueue && !turnShown && millis() - joinTime >= 60000)
  {
    showMessage("YOUR TURN!");
    beep(4);
    turnShown = true;
    Serial.println("YOUR TURN");
  }
}


}

