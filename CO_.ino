// ______________________________________ WIFI

#include <ArduinoJson.h>
#include <FS.h>

#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
// wifimanager can run in a blocking mode or a non blocking mode
// Be sure to know how to process loops with no delay() if using non blocking

WiFiManager wifiManager; // global wifiManager instance
char IP[16];
char GATEWAY[16];
char SUBNET[16];

//______________________________________ MQTT

//#include <WiFi.h>
#include <PubSubClient.h>

WiFiClient espClient;

PubSubClient client(espClient);
const char* mqttServer = "91.191.173.237";
const int mqttPort = 1883;
const char* mqttUser = "mqtt";
const char* mqttPassword = "CariocaOzsumer3535";


// ______________________________________ RTC
#include "time.h"
#include <ErriezDS1307.h>
ErriezDS1307 rtc;
#define DATE_STRING_SHORT   3
#include <Wire.h> // must be included here so that Arduino library object file references work

// ______________________________________ NTP

#include <NTPClient.h>
#include <WiFiUdp.h>

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

// ______________________________________ RFID

#include <SPI.h>
#include <MFRC522.h>
#define RFID_RST_PIN         10
#define RFID_SS_PIN          15
MFRC522 mfrc522(RFID_SS_PIN, RFID_RST_PIN);   // Create MFRC522 instance
//#define countof(a) (sizeof(a) / sizeof(a[0]))
byte block;
byte len;
byte kartNumarasi[18];
//MFRC522 mfrc522(RFID_SS_PIN, RFID_RST_PIN);
//byte ID[4] = {0, 0, 0, 0};

// ______________________________________ MAX7219 DISPLAY DRIVER

#include "LedControl.h"
LedControl lc = LedControl(16, 0, 2, 1);

// ______________________________________ JSON

StaticJsonDocument<10000> doc;
StaticJsonDocument<1000> doc1;
StaticJsonDocument <5000> json;
DeserializationError error;           // = deserializeJson(doc, myFileSDCard);
JsonArray O = doc.createNestedArray("O");

//______________________________________ VARIABLES ______________________________________ //

File file ;
char *KeyIDMakina = "CO-00096";//__________________ WIFI'DE GÖRÜNEN MAKİNA NUMARASI
char *N = "00096" ; //__________________JSON'DAKİ MAKİNA NUMARASI

String I; //____________ PERSONEL ID NUMARASI


String dataArray; //

//String dosya;
//String dosyaDegiskeni;

uint8_t besdakka = 0;
uint8_t BesDakikaToplamAdet;
bool besDakika = false;
uint16_t BesDakikadaBirUrunSuresi = 10;
uint16_t BirAdetUrunOrtalamaSuresi = 10;

bool InternetKontrolBlink;

bool OperasyonArrayKontrol = false;

String GunlukAdet;
char okuma[512];
char IPler[512];
int i = 0;
int j = 0;
//int sayi = 0 ;

String SaatDakika;
String GunAyYil;
//String sayii;

uint16_t bicakdurum = 0;
byte prestate = 0;
byte bicakVur = 0;

//uint16_t BicakSinyalPin = A0;
unsigned long bekleme = 0;

byte AktifAn;//= 3;
byte PasifAn;//= 1;
//uint8_t DataBos = 12;

bool RFID_Kontrol = false;

uint8_t  SonIslemDakika = 0;
uint8_t  Dakika = 0;
uint8_t DakikaAdedi = 1;

uint8_t  RTCGun;
uint8_t  KayitliGun;
uint16_t  KayitliAdet;
String GunAdetJson;

unsigned long startTime;
unsigned long endTime;
//int duration;
//byte timerRunning;

unsigned long GeriSayimBasla = 0;
unsigned long GeriSayimSon = 0;
unsigned long DataKontrolBasla = 0;
unsigned long DataKontrolSon = 0 ;
unsigned long InternetKontrolBasla = 0;
unsigned long InternetKontrolSon = 0;
unsigned long BicakSinyalBasla = 0;
unsigned long BicakSinyalSon = 0;

uint8_t   JsonMqtt_len;
String JsonMqtt;
char JsonMqtt_char;


uint8_t  currentHour;
uint8_t  currentMinute;
uint8_t  currentSecond;
struct tm *ptm;
uint8_t  monthDay;
uint8_t  currentMonth;
uint16_t  currentYear;

uint16_t  randNumber;

//byte AktifLed = 32;
//byte PasifLed = 35;


uint16_t k = 0;
String KaliciIpler;
//______________________________________ SETUP ______________________________________ //


void setup()
{
  Serial.begin(115200);

  delay(20);
  Serial.println("Başladık...");
  delay(20);

  //pinMode(BicakSinyalPin, INPUT);
  pinMode(AktifAn, OUTPUT);
  pinMode(PasifAn, OUTPUT);
  // pinMode(DataBos, OUTPUT);

  //  pinMode(14, OUTPUT);
  //  pinMode(25, OUTPUT);
  //  digitalWrite (14, HIGH);
  //  digitalWrite (25, HIGH);

  lc.shutdown(0, false);
  /* Set the brightness to a medium values */
  lc.setIntensity(0, 8);
  /* and clear the display */
  lc.clearDisplay(0);
  delay(20);
  SPI.begin();
  delay(20);
  Wire.begin();
  Serial.println("SPIFFS_Check geliyorrr...");
  SPIFFS_Check();
  Serial.println("SPIFFS_Check");
  delay(250);

  delay(15);
  WifiManager_Setup();
  Serial.println("WifiManager_Setup");
  delay(250);

  NTP_GET_TIME();
  Serial.println("NTP_GET_TIME");
  delay(250);

  RTC_SET();
  Serial.println("RTC_SET");
  delay(250);

  RTC_GET();
  Serial.println("RTC_GET");
  delay(250);


  AdetVeGunKontrol();
  Serial.println("AdetVeGunKontrol");
  delay(250);

  RFID();
  Serial.println("RFID");
  delay(15);

  SetUrunAdet();
  j = BirAdetUrunOrtalamaSuresi;
  SetOrtSure();

  //  randNumber = random(10000);
  //  BicakSinyalSon = randNumber + 10000;
}

// ______________________________________ MAIN LOOP ______________________________________ //


void loop()
{
  wifiManager.process();
  //  BicakSinyalBasla = millis();
  //  randNumber = random(10000);
  //  while (BicakSinyalBasla - BicakSinyalSon >= 15000 + randNumber )
  //  {
  //
  //    bicakdurum = 667;
  //    SetUrunAdet();
  //digitalWrite(bicakdurum, HIGH);
  Operasyon_Basla();
  //    wifiManager.process();
  //    Serial.println("Sayi Geldi");
  //
  //    BicakSinyalSon = BicakSinyalBasla;
  //  }
  //  bicakdurum = 0;
  //  prestate = 0;
}

// ______________________________________ Operasyon ______________________________________ //

void Operasyon_Basla()
{

  bicakdurum = analogRead(A0);
  // && (millis() - bekleme > 2000)
  if (bicakdurum > 666 && prestate == 0 ) // Bıçak vurdumu kontrol eder.
  { //Bıçak vurdu,
    besDakika = true;
    BirAdetUrunOrtalamaSuresi = BesDakikadaBirUrunSuresi;
    GunAdetJson = "";
    i++;
    //doc1["SonGun"]=
    doc1["ToplamAdet"] = i;

    AdetWrite();
    delay(10);

    OperasyonJsonOlustur();
    SetUrunAdet();
    //display.showNumber(i);

    //bekleme = millis();

    prestate = 1;
    delay(10);
  }
  else if (bicakdurum < 666) // Bıçak eski konumuna dönüp dönmediğini kontrol eder.
  {
    prestate = 0;   //bicak eski konumuna döndü, prestate'i sıfırla
    delay(10);
  }

  BesDakikaDongu();

}

// ______________________________________  Opsay ile Adet  ______________________________________ //
void OperasyonJsonOlustur()
{
  OperasyonArrayKontrol = true;
  RTC_GET();
  //Serial.println("sondk: " + String(SonIslemDakika) + "günceldk: " + String(Dakika));

  if (SonIslemDakika != Dakika)
  {
    besdakka++;
    dataArray = (SaatDakika + "|" + DakikaAdedi);
    Serial.println(dataArray);
    if (DakikaAdedi != 0)
    {
      O.add(dataArray);

      doc["N"] = N;
      doc["I"] = I;

      //bedakkatoplamadet = bedakkatoplamadet + int(doc["ToplamAdet"]);

      BesDakikaToplamAdet += DakikaAdedi;
      Serial.println("");

      Serial.print("BesDakikaToplamAdet: ");

      Serial.println(BesDakikaToplamAdet);
      serializeJson(doc, Serial);

      JsonMqtt = "";
      serializeJson(doc, JsonMqtt);
      //digitalWrite(DataBos, LOW);
    }
    DakikaAdedi = 1;
  }
  else
  {
    ++DakikaAdedi;
  }
  if (besdakka == 5) {
    BesDakikadaBirUrunSuresi = 300 / BesDakikaToplamAdet ;
    //displaya.showNumber(BesDakikadaBirUrunSuresi);
    BirAdetUrunOrtalamaSuresi = BesDakikadaBirUrunSuresi;
    besdakka = 0;
    BesDakikaToplamAdet = 0;
  }
  SonIslemDakika = Dakika;
  //AdetWrite();
  Serial.println("");
  Serial.println("__________________");
  //AdetRead();
  delay(10);
}

//______________________________________  5 DAKİKA  ______________________________________ //
void BesDakikaDongu()
{

  DataKontrolBasla = millis();


  while (DataKontrolBasla - DataKontrolSon >= 1000 && BirAdetUrunOrtalamaSuresi > 0)
  {
    BirAdetUrunOrtalamaSuresi--;
    //displaya.showNumber(BirAdetUrunOrtalamaSuresi);
    //Serial.println(BirAdetUrunOrtalamaSuresi);
    DataKontrolSon = DataKontrolBasla;
  }

  if (BirAdetUrunOrtalamaSuresi <= 0) {
    digitalWrite(PasifAn, HIGH);
    digitalWrite(AktifAn, LOW);
  }
  if (BirAdetUrunOrtalamaSuresi >= 1) {
    digitalWrite(PasifAn, LOW);
    digitalWrite(AktifAn, HIGH);
  }

  if ( O.size() >= 11)
  {
    JsonArray O = doc.createNestedArray("O");
    Serial.println("AŞIRI JSON ARRAY VERİSİ... VERİLER SİLİNDİ !");

    //digitalWrite(DataBos, HIGH);
  }
  if ( O.size() >= 1)
  {

    //digitalWrite(DataBos, LOW);
  }
  if ( O.size() == 0)
  {
    //digitalWrite(DataBos, HIGH);
  }


  GeriSayimBasla = millis();
  while (GeriSayimBasla - GeriSayimSon >= 300000 && O.size() > 0 )
  {

    Serial.println("-------------");

    JsonMqtt_len = JsonMqtt.length() + 1;
    char JsonMqtt_char[JsonMqtt_len];
    JsonMqtt.toCharArray(JsonMqtt_char, JsonMqtt_len);

    JsonMqtt = "";
    Serial.println(JsonMqtt);

    if (WiFi.status() == WL_CONNECTED )
    {
      client.setServer(mqttServer, mqttPort);

      Serial.println("Sending message to MQTT topic..");

      client.connect("ESP32Client", mqttUser, mqttPassword );

      if (client.publish("Sinyal", JsonMqtt_char))
      {
        JsonArray O = doc.createNestedArray("O");
        Serial.println("Success sending message;");
        //digitalWrite(DataBos, HIGH);
      }
      else
      {
        Serial.println("Error sending message");
      }
    }
    else
    {
      wifiManager.autoConnect(KeyIDMakina);
      if (WiFi.status() == WL_CONNECTED )
      {
        client.setServer(mqttServer, mqttPort);
        Serial.println("Sending message to MQTT topic..");
        client.connect("ESP32Client", mqttUser, mqttPassword );
        delay(25);

        if (client.publish("Sinyal", JsonMqtt_char))
        {
          JsonArray O = doc.createNestedArray("O");
          Serial.println("Success sending message;");
          //digitalWrite(DataBos, HIGH);
        }
      }
      else {
        Serial.println("Configportal running");
      }
    }
    client.disconnect();
    GeriSayimSon = GeriSayimBasla;
  }

  InternetKontrolBasla = millis();
  while (InternetKontrolBasla - InternetKontrolSon >= 10000 && WiFi.status() != WL_CONNECTED )
  {
    // if the LED is off turn it on and vice-versa:
    if (InternetKontrolBlink == LOW) {
      InternetKontrolBlink = HIGH;
    } else {
      InternetKontrolBlink = LOW;
    }
    // set the LED with the ledState of the variable:
    // digitalWrite(DataBos, InternetKontrolBlink);

    InternetKontrolSon = InternetKontrolBasla;
  }
}
// ______________________________________ RFID WITH MASK ______________________________________ //
void RFID()
{

  mfrc522.PCD_Init();

  //  // Prepare key - all keys are set to FFFFFFFFFFFFh at chip delivery from the factory.
  //  //some variables we need

  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

  byte block;
  byte len;

  MFRC522::StatusCode status;

  delay(10);

  while (RFID_Kontrol != true)
  {

RFID_TekrarOku:

    //---------------------------------------------------------------------------------------

    if (  mfrc522.PICC_IsNewCardPresent()) {
      //return;
Rfid_Yakala:

      // Select one of the cards
      if ( ! mfrc522.PICC_ReadCardSerial()) {
        goto RFID_TekrarOku;
      }
      Serial.println(F("**Card Detected:**"));

      byte buffer1[18];
      block = 4;
      len = 18;
      byte buffer2[18];
      block = 1;

      status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 1, &key, &(mfrc522.uid)); //line 834
      if (status != MFRC522::STATUS_OK) {
        Serial.print(F("Authentication failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        goto RFID_TekrarOku;

      } else {
        RFID_Kontrol = true;
      }
      status = mfrc522.MIFARE_Read(block, buffer2, &len);
      if (status != MFRC522::STATUS_OK) {
        goto RFID_TekrarOku;
      }

      I ;

      for (uint8_t i = 0; i < 16; i++) {
        I += (char)buffer2[i];
        Serial.write(buffer2[i]);
      }

      I.remove(5, 16);
      Serial.print("Kart Id: ");
      Serial.println(I);
    }
    else
    {
      Serial.println("Kart Okut");

      if (mfrc522.PICC_IsNewCardPresent()) {
        goto Rfid_Yakala;
      }
    }
    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
  }
}

// ______________________________________ RTC ______________________________________ //

void RTC_SET() // ______________________________________ SET
{
  Wire.setClock(100000);
  rtc.setSquareWave(SquareWaveDisable);
  rtc.begin();
  // if (!rtc.begin())
  //  {
  //    display.showString("rtc");
  //    delay(750);
  //    display.clear();
  //    delay(250);
  //  }
  if (WiFi.status() == WL_CONNECTED ) {
    // Set date/time: 12:34:56 31 December 2020 Sunday

    rtc.setDateTime(int(currentHour), int(currentMinute), int(currentSecond), int(monthDay), int(currentMonth), int(currentYear), int(0));
  }
}
void RTC_GET() //______________________________________ GET
{
  char name[DATE_STRING_SHORT + 1];

  uint8_t hour;
  uint8_t min;
  uint8_t sec;
  uint8_t mday;
  uint8_t mon;
  uint16_t year;
  uint8_t wday;

  // Read date/time
  rtc.getDateTime(&hour, &min, &sec, &mday, &mon, &year, &wday);

  delay(50);
  GunAyYil = (String(mday) + "/" + String(mon) + "/" + String(year));
  SaatDakika = (String(hour) + ":" + String(min));
  Dakika = int(min);
  RTCGun = int(mday);
  //Serial.println(Dakika);
  Serial.println(String(hour) + ":" + String(min) + ":" + String(sec));
  Serial.println(String(mday) + "/" + String(mon) + "/" + String(year));
}

// ______________________________________ NTP GET TIME ______________________________________//

void NTP_GET_TIME()
{
  timeClient.update();
  time_t epochTime = timeClient.getEpochTime();
  delay(10);
  currentHour = timeClient.getHours();
  currentMinute = timeClient.getMinutes();
  currentSecond = timeClient.getSeconds();
  // Serial.println("Saniye: "+ currentSecond);
  // Serial.println("Dakika: "+ currentMinute);
  // Serial.println("Saat: "+ currentHour);
  tm *ptm = gmtime ((time_t *)&epochTime);
  delay(10);
  monthDay = ptm->tm_mday;
  currentMonth = ptm->tm_mon + 1;
  currentYear = ptm->tm_year + 1900;
  //  Serial.println("Gün: "+ monthDay);
  //  Serial.println("Ay: "+ currentMonth);
  //  Serial.println("Yıl: "+ currentYear);

  delay(10);
}

// ______________________________________ SPIFFS ______________________________________ //

// ______________________________________ SPIFFS CHECK ______________________________________ //
void SPIFFS_Check()
{
  if (!SPIFFS.begin())
  {
    Serial.println("Error : mounting SPIFFS");
    Serial.println(" SPIFFS works well ");
  }
}

//______________________________________ SPIFFS READ ______________________________________ //
void AdetRead()
{
  File file = SPIFFS.open("/json.txt", "r");
  k = 0;
  while (file.available())
  {
    //    Serial.write(file.read());
    okuma[k] = file.read();
    k++;
  }

  GunAdetJson = String(okuma);
  Serial.println("GunAdetJson: " + GunAdetJson);
  DeserializationError error = deserializeJson(doc1, GunAdetJson);

  KayitliGun = int(doc1["SonGun"]);
  KayitliAdet = int(doc1["ToplamAdet"]);


  file.close();
}

//______________________________________ SPIFFS WRİTE ______________________________________ //
void AdetWrite()
{
  serializeJson(doc1, GunAdetJson);
  //  listAllFiles();
  //  SPIFFS.format();
  SPIFFS.remove("/json.txt");
  file = SPIFFS.open("/json.txt", "w");


  if ( file.print(GunAdetJson + " \n"))
  {
    Serial.println("Adet Kayıt Başarılı");
  }
  else
  {
    Serial.println("Adet Kayıt Başarısız");
  }
  file.close();
}

// ______________________________________ ADET GUN KONTROL ______________________________________ //
void AdetVeGunKontrol()
{

  AdetRead();

  if (RTCGun != KayitliGun)
  {

    KayitliAdet = 0;
    doc1["ToplamAdet"] = 0;

    KayitliGun = RTCGun;
    doc1["SonGun"] = RTCGun;

    AdetWrite();
    Serial.println("Gün Dönümü Değişkenler güncellendi");
  }

  i = KayitliAdet;
  Serial.println("RTCGun: " + String(RTCGun));
  Serial.println("KayitliGun: " + String(KayitliGun));
}

// ______________________________________ WIFI MANAGER ______________________________________ //
void WifiManager_Setup()
{
  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP

  if (SPIFFS.exists("/config.json"))
  {
    //file exists, reading and loading
    Serial.println("reading config file");
    File file = SPIFFS.open("/config.json", "r");

    k = 0;
    while (file.available())
    {
      //    Serial.write(file.read());
      IPler[k] = file.read();
      k++;
    }

    KaliciIpler = String(okuma);
    Serial.println("KalıcıIpler: " + KaliciIpler);
    DeserializationError error = deserializeJson(json, KaliciIpler);


    strcpy(IP, json["IP"]);
    strcpy(GATEWAY, json["GATEWAY"]);
    strcpy(SUBNET, json["SUBNET"]);
    //    KayitliGun = int(doc1["SonGun"]);
    //    KayitliAdet = int(doc1["ToplamAdet"]);


    file.close();


    //    if (file) {
    //      Serial.println("opened config file");
    //
    //
    //      size_t size = file.size();
    //      // Allocate a buffer to store contents of the file.
    //      std::unique_ptr<char[]> buf(new char[size]);
    //
    //      file.readBytes(buf.get(), size);
    //#if defined(ARDUINOJSON_VERSION_MAJOR) && ARDUINOJSON_VERSION_MAJOR >= 6
    //      DynamicJsonDocument json(10000);
    //
    //      auto deserializeError = deserializeJson(json, buf.get());
    //      serializeJson(json, Serial);
    //      if ( ! deserializeError ) {
    //#else
    //      DynamicJsonBuffer jsonBuffer;
    //      JsonObject& json = jsonBuffer.parseObject(buf.get());
    //      json.printTo(Serial);}
    //      if (json.success())
    //      {
    //#endif
    //        Serial.println("\nparsed json");
    //        strcpy(IP, json["IP"]);
    //        strcpy(GATEWAY, json["GATEWAY"]);
    //        strcpy(SUBNET, json["SUBNET"]);
    //
    //      }
    //      else
    //      {
    //        Serial.println("failed to load json config");
    //      }
  }





  //end read
  Serial.println(IP);
  //  Serial.println(api_token);
  //  Serial.println(mqtt_server);


  // The extra parameters to be configured (can be either global or just in the setup)
  // After connecting, parameter.getValue() will get you the configured value
  // id/name placeholder/prompt default length
  WiFiManagerParameter custom_IP("ip", "IP", IP, 16);
  WiFiManagerParameter custom_GATEWAY("gateway", "GATEWAY", GATEWAY, 16);
  WiFiManagerParameter custom_SUBNET("subnet", "SUBNET", SUBNET, 16);

  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around


  //set config save notify callback


  //wifiManager.setShowStaticFields(true);
  //set static ip

  IPAddress _ip, _gw, _sn;
  _ip.fromString(IP);
  _gw.fromString(GATEWAY);
  _sn.fromString(SUBNET);

  wifiManager.setSTAStaticIPConfig(_ip, _gw, _sn);

  //add all your parameters here
  wifiManager.addParameter(&custom_IP);
  wifiManager.addParameter(&custom_GATEWAY);
  wifiManager.addParameter(&custom_SUBNET);
  wifiManager.setConfigPortalBlocking(false);
  wifiManager.setSaveConfigCallback(saveConfigCallback);
  //wifiManager.resetSettings();

  wifiManager.autoConnect(KeyIDMakina);
  Serial.println("connected...yeey :)");

  //read updated parameters
  strcpy(IP, custom_IP.getValue());
  strcpy(GATEWAY, custom_GATEWAY.getValue());
  strcpy(SUBNET, custom_SUBNET.getValue());

  //save the custom parameters to FS

  if (strlen(IP) > 0)
  {
    //  Serial.println("saving config");
    //#if defined(ARDUINOJSON_VERSION_MAJOR) && ARDUINOJSON_VERSION_MAJOR >= 6
    //  DynamicJsonDocument json(1024);
    //#else
    //  DynamicJsonBuffer jsonBuffer;
    //  JsonObject& json = jsonBuffer.createObject();
    //#endif

    json["IP"] = IP;
    json["GATEWAY"] = GATEWAY;
    json["SUBNET"] = SUBNET;

    file = SPIFFS.open("/config.json", "w");
    if (!file) {
      Serial.println("failed to open config file for writing");
    }


    serializeJson(json, Serial);
    serializeJson(json, file);
    if ( file.print(KaliciIpler + " \n"))
    {
      Serial.println("Adet Kayıt Başarılı");
    }
    else
    {
      Serial.println("Adet Kayıt Başarısız");
    }
    //  json.printTo(Serial);
    //  json.printTo(file);

    file.close();
    //end save
  }

  Serial.println(WiFi.localIP());
  Serial.println(WiFi.gatewayIP());
  Serial.println(WiFi.subnetMask());
}
void saveConfigCallback() {
  Serial.println("Bağlantı Bulundu ! ");
}
void SetUrunAdet()
{
  if (i >= 0 && i <= 9) {
    lc.setDigit(0, 0, i - (int)i / 10 * 10, false);
  }
  else if (i >= 10 && i <= 99) {
    lc.setDigit(0, 1, ((int)i / 10 * 10 - (int)i / 100 * 100) / 10, false);
    lc.setDigit(0, 0, i - (int)i / 10 * 10, false);
  }
  else if (i >= 100 && i <= 999) {
    lc.setDigit(0, 2, ((int)i / 100 * 100 - (int)i / 1000 * 1000) / 100, false);
    lc.setDigit(0, 1, ((int)i / 10 * 10 - (int)i / 100 * 100) / 10, false);
    lc.setDigit(0, 0, i - (int)i / 10 * 10, false);
  }
  else if (i >= 1000 && i <= 9999) {
    lc.setDigit(0, 3, (int)i / 1000, false);
    lc.setDigit(0, 2, ((int)i / 100 * 100 - (int)i / 1000 * 1000) / 100, false);
    lc.setDigit(0, 1, ((int)i / 10 * 10 - (int)i / 100 * 100) / 10, false);
    lc.setDigit(0, 0, i - (int)i / 10 * 10, false);
  }
}

void SetOrtSure()
{
  if (j >= 0 && j <= 9) {
    lc.setDigit(0, 5, j - (int)j / 10 * 10, false);
  }
  else if (j >= 10 && j <= 99) {
    lc.setDigit(0, 4, ((int)j / 10 * 10 - (int)j / 100 * 100) / 10, false);
    lc.setDigit(0, 5, j - (int)j / 10 * 10, false);
  }
  else if (j >= 100 && j <= 999) {
    lc.setDigit(0, 4, ((int)j / 100 * 100 - (int)j / 1000 * 1000) / 100, false);
    lc.setDigit(0, 5, ((int)j / 10 * 10 - (int)j / 100 * 100) / 10, false);
    lc.setDigit(0, 6, j - (int)j / 10 * 10, false);
  }
  else if (j >= 1000 && j <= 9999) {
    lc.setDigit(0, 4, (int)j / 1000, false);
    lc.setDigit(0, 5, ((int)j / 100 * 100 - (int)j / 1000 * 1000) / 100, false);
    lc.setDigit(0, 6, ((int)j / 10 * 10 - (int)j / 100 * 100) / 10, false);
    lc.setDigit(0, 7, j - (int)j / 10 * 10, false);
  }
}
