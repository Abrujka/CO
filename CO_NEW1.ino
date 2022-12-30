// ______________________________________ WIFI

#include <ArduinoJson.h>
#include <FS.h>

#include <WiFiManager.h>
WiFiManager wifiManager; // global wifiManager instance

char  IP[16];
char  GATEWAY[16] = {"192.168.1.1"};
char  SUBNET[16]  = {"255.255.255.0"};
char  DNS[16]     = {"8.8.8.8"};

WiFiManagerParameter custom_IP("I_P", "IP Numarası", IP, 16);
WiFiManagerParameter custom_GATEWAY("G_W", "Gateway Numarası", GATEWAY, 16);
WiFiManagerParameter custom_SUBNET("S_N", "Subnet Numarası", SUBNET, 16);
WiFiManagerParameter custom_DNS ("D_N_S", "DNS Numarası", DNS, 16);

//______________________________________ MQTT

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

WiFiUDP     ntpUDP;
NTPClient   timeClient(ntpUDP, "pool.ntp.org");

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
StaticJsonDocument <1024> json;
DeserializationError error;           //
JsonArray O = doc.createNestedArray("O");

//______________________________________ VARIABLES ______________________________________ //

File file ;
char *KeyIDMakina = "CO-00096";   // WIFI'DE GÖRÜNEN MAKİNA NUMARASI
char *N = "00096" ;               // JSON'DAKİ MAKİNA NUMARASI

String    I;                      // PERSONEL ID NUMARASI


String    dataArray;              // JSON SAAT/DAKİKA VERİ DEĞİŞKENİ


uint8_t   besdakka = 0;                   // 5 TUR İF DÖNGÜSÜNÜ DÖNDÜRÜR SONRA SIFIRLANIR
uint8_t   BesDakikaToplamAdet;            // 5 DAKİKADA GELEN TOPLAM SİNYALİ/ADETİ İÇERİĞİNDE TOPLAR.
int       BesDakikadaBirUrunSuresi = 10;
int       BirAdetUrunOrtalamaSuresi = 10;

bool    OperasyonArrayKontrol = false;


char    okuma[512];
char    IPler[512];
int     i;
int     j;

String    SaatDakika;         //RTC'DEN ÇEKİLEN SAAT VE DAKİKA BİLGİSİ
String    GunAyYil;           //RTC'DEN ÇEKİLEN GÜN - AY - YIL BİLGİSİ

uint16_t  bicakdurum = 0;     //ANALOG GİRİŞTEN OKUNAN SİNYAL KONTROL DEĞİŞKENİ
byte      prestate = 0;       // BIÇAK VURUŞUNUN AÇIKTAN KAPALIYA GEÇTİĞİNİ TESPİT, NEGATİF FEEDBACK SAĞLAYICI DEĞİŞKENİ



unsigned long bekleme = 0;    // 2 SANİYE GECİKMELİ BIÇAK SİNYALİ OKUMA

byte    AktifAn;// = 1;       //YEŞİL LED
byte    PasifAn;// = 3;       //KIRMIZI LED

byte    delaytime = 100;

bool      RFID_Kontrol = false; //RFID WHİLE İÇERİSİNDE KART OKUNMASINI BEKLETİR

uint8_t   SonIslemDakika = 0;
uint8_t   Dakika = 0;
uint8_t   DakikaAdedi = 1;

byte      RTCGun;             //RTC'DEKİ ANLIK GÜN BİLGİSİ
byte      KayitliGun;         //SPIFFS DE YER ALAN KAYITLI GÜN BİLGİSİ
int       KayitliAdet;        //SPIFFS'DE YER ALAN KAYITLI ADET BİLGİSİ
String    GunAdetJson;        //SPIFSS'DEKİ ADET VE GÜN BİLGİSİNİN JSON İŞLEMİ İÇİN AKTARILDIĞI DEĞİŞKEN


unsigned long GeriSayimBasla = 0;                 // 5 DAKİKALIK DÖNGÜSEL KONTROL İÇİN MİLLİS İLE ÇALIŞAN BAŞLANGIÇ DEĞİŞKENİ
unsigned long GeriSayimSon = 0;                   // 5 DAKİKALIK DÖNGÜSEL KONTROL İÇİN MİLLİS İLE ÇALIŞAN SONLANMA DEĞİŞKENİ
unsigned long AktifPasifGecisGeriSayimBasla = 0;  // YEŞİL(AKTİF) DURUMUNDAN KIRMIZI(PASİF) DURUMA GEÇİŞ GERİ SAYIM KONRTOL BAŞLANGIÇ DEĞİŞKENİ
unsigned long AktifPasifGecisGeriSayimSon = 0 ;   // YEŞİL(AKTİF) DURUMUNDAN KIRMIZI(PASİF) DURUMA GEÇİŞ GERİ SAYIM KONRTOL SONLANMA DEĞİŞKENİ
unsigned long BicakSinyalBasla = 0;               // OTOMATİK BIÇAK VURUŞ SİNYALİ GERİ SAYIM BAŞLANGIÇ DEĞİŞKENİ
unsigned long BicakSinyalSon = 0;                 // OTOMATİK BIÇAK VURUŞ SİNYALİ GERİ SAYIM SONLANMA DEĞİŞKENİ

byte      JsonMqtt_len;
String    JsonMqtt;
char      JsonMqtt_char;

uint8_t   currentHour;    //NTP'DEN ÇEKİLEN SAAT BİLGİSİ
uint8_t   currentMinute;  //NTP'DEN ÇEKİLEN DAKİKA BİLGİSİ
uint8_t   currentSecond;  //NTP'DEN ÇEKİLEN SANİYE BİLGİSİ
struct    tm *ptm;        //NTP'DEN ÇEKİLEN STRUCT YAPISI
uint8_t   monthDay;       //NTP'DEN ÇEKİLEN GÜN BİLGİSİ
uint8_t   currentMonth;   //NTP'DEN ÇEKİLEN AY BİLGİSİ
uint16_t  currentYear;    //NTP'DEN ÇEKİLEN YIL BİLGİSİ

uint16_t  randNumber;     //RANDOM SAYI OLUŞTURMA DEĞİŞKENİ



uint16_t k = 0;           // SPIFFS'DEN ALINAN VERİLERİN OKUMASINDA CHAR DİZİLİMİ İÇİN DÖNGÜDE KULLANILAN SAYI DEĞİŞKENİ
String KaliciIpler;       // SPIFFS'DEN ÇEKİLEN KAYITLI IP'LERİN AKTARILDIĞI DEĞİŞKEN
byte wifiAttempt = 0;     // DONGU İÇERİSİNDE İNTERNETİ KESİLDİĞİNDE, SINIRLI BAĞLANTI DENEMESİNİ SAĞLAYAN DEĞİŞKEN


//______________________________________ SETUP ______________________________________ //

void setup()
{

  Serial.begin(115200);

  randNumber = random(2000, 5000);
  delay(randNumber);
  
  pinMode(AktifAn, OUTPUT);
  pinMode(PasifAn, OUTPUT);


  lc.shutdown(0, false);
  /* Set the brightness to a medium values */
  lc.setIntensity(0, 8);
  /* and clear the display */
  lc.clearDisplay(0);


  SPI.begin();
  Wire.begin();

  SegnmentIntro();

  delay(10);
  RFID();
  Serial.println("RFID");

  WIFI_SPIFFS_SORGU();

  Serial.println("WIFI_SPIFFS_SORGU");


  IPAddress _ip, _gw, _sn, _dns;
  _ip.fromString(IP);
  _gw.fromString(GATEWAY);
  _sn.fromString(SUBNET);
  _dns.fromString(DNS);

  wifiManager.setSTAStaticIPConfig(_ip, _gw, _sn, _dns);

  wifiManager.addParameter(&custom_IP);
  wifiManager.addParameter(&custom_GATEWAY);
  wifiManager.addParameter(&custom_SUBNET);
  wifiManager.addParameter(&custom_DNS);

  wifiManager.setCleanConnect(true);
  //wifiManager.setTitle("Carioca");
  wifiManager.setConnectTimeout(5);
  wifiManager.setConnectRetries(1);
  wifiManager.setConfigPortalBlocking(false);

  wifiManager.setSaveParamsCallback(saveParamsCallback);

  if (wifiManager.getWiFiIsSaved())
    wifiManager.setEnableConfigPortal(false);

  wifiManager.autoConnect(KeyIDMakina);

  //  if (wifiManager.autoConnect(KeyIDMakina)) {

  if (WiFi.status() == WL_CONNECTED && WiFi.getMode() == WIFI_STA)
  {
    Serial.println("yeeyyy Connected");

    Serial.println("İnternet Bağlantısı <<< SAĞLAND I>>>");
    delay(15);
    NTP_GET_TIME();
    Serial.println("NTP_GET_TIME");
    delay(15);
    RTC_SET();
    Serial.println("RTC_SET");
    SetDisplayBirAdetUrunSuresi();
  }

  else
  {
    Serial.println("________________________________________ATLADI");
  }

  RTC_GET();
  Serial.println("RTC_GET");

  AdetVeGunKontrol();
  Serial.println("AdetVeGunKontrol");

  SetDisplayUrunAdet();
  j = BirAdetUrunOrtalamaSuresi;
  SetDisplayBirAdetUrunSuresi();

  Serial.print("Setup Time: ");
  Serial.print(millis());
  Serial.println(" ms ");
}

//______________________________________  MAIN LOOP  ______________________________________ //
void loop()
{

  //BicakSinyalBasla = millis();
  //randNumber = random(10000);
  //  while (BicakSinyalBasla - BicakSinyalSon >= 15000 + randNumber )
  //  {
  // bicakdurum = 667;
  // SetDisplayUrunAdet();
  //digitalWrite(bicakdurum, HIGH);

  Operasyon_Basla();

  wifiManager.process();
  
  // Serial.println("Sayi Geldi");
  // BicakSinyalSon = BicakSinyalBasla;

  //  }
  //bicakdurum = 0;
  // prestate = 0;

}
//______________________________________  saveParamsCallback  ______________________________________ //
void saveParamsCallback () {

  Serial.println("Get Params:");

  strcpy(IP, custom_IP.getValue());
  strcpy(GATEWAY, custom_GATEWAY.getValue());
  strcpy(SUBNET, custom_SUBNET.getValue());
  strcpy(DNS, custom_DNS.getValue());

  IPAddress _ip, _gw, _sn, _dns;
  _ip.fromString(IP);
  _gw.fromString(GATEWAY);
  _sn.fromString(SUBNET);
  _dns.fromString(DNS);

  wifiManager.setSTAStaticIPConfig(_ip, _gw, _sn, _dns);

  if (WiFi.status() == WL_CONNECTED && WiFi.getMode() == WIFI_STA)
  {

    Serial.println("İnternet Bağlantısı <<< SAĞLAND I>>>");

    NTP_GET_TIME();
    Serial.println("NTP_GET_TIME");

    RTC_SET();
    Serial.println("RTC_SET");

    SetDisplayBirAdetUrunSuresi();
  }
  else
  {
    Serial.println("________________________________________ATLADI");
  }

  if (strlen(IP) > 0)
  {
    Serial.println("******* STATİC IP GİRİLDİ *********");
    json["I_P"] = IP;
    json["Gate_Way"] = GATEWAY;
    json["Sub_Net"] = SUBNET;
    json["D_N_S"] = DNS;
    if (SPIFFS.exists("/config.json"))
    {
      SPIFFS.remove("/config.json");
    }
    file = SPIFFS.open("/config.json", "w");
    if (!file) {
      Serial.println("failed to open config file for writing");
    }
    KaliciIpler = " ";
    serializeJson(json, Serial);
    serializeJson(json, KaliciIpler);
    if ( file.print(KaliciIpler + " \n"))
    {
      Serial.println("IP Bilgileri Kaydı <<<BAŞARILI>>>");
    }
    else
    {
      Serial.println("IP Bilgileri Kaydı ___BAŞARISIZ___");
    }

  } else {
    Serial.println("____STATİC IP GİRİLMEDİ___");
  }
  file.close();
}
//______________________________________  Operasyon_Basla  ______________________________________ //
void Operasyon_Basla()
{

  bicakdurum = analogRead(A0);

  if (bicakdurum > 666 && prestate == 0 && (millis() - bekleme > 2000)) // Bıçak vurdumu kontrol eder.
  {
    //Bıçak vurdu,

    BirAdetUrunOrtalamaSuresi = BesDakikadaBirUrunSuresi;
    GunAdetJson = "";
    i++;
    doc1["SonGun"] = RTCGun;
    doc1["ToplamAdet"] = i;

    AdetWrite();

    OperasyonJsonOlustur();

    SetDisplayUrunAdet();

    bekleme = millis();

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
      SetDisplayUrunAdet();
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

    j = BirAdetUrunOrtalamaSuresi;
    SetDisplayBirAdetUrunSuresi();

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

}

//______________________________________  5 DAKİKA  ______________________________________ //
void BesDakikaDongu()
{

  AktifPasifGecisGeriSayimBasla = millis();
  while (AktifPasifGecisGeriSayimBasla - AktifPasifGecisGeriSayimSon >= 1000 && BirAdetUrunOrtalamaSuresi > 0)
  {
    BirAdetUrunOrtalamaSuresi--;
    AktifPasifGecisGeriSayimSon = AktifPasifGecisGeriSayimBasla;
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
    SetDisplayUrunAdet();
  }


  GeriSayimBasla = millis();
  while (GeriSayimBasla - GeriSayimSon >= 10000 && O.size() > 0 && wifiManager.getWiFiIsSaved())
  {
    Serial.println("-------------");

    JsonMqtt_len = JsonMqtt.length() + 1;
    char JsonMqtt_char[JsonMqtt_len];
    JsonMqtt.toCharArray(JsonMqtt_char, JsonMqtt_len);

    JsonMqtt = "";
    Serial.println(JsonMqtt);




    //______________________________________________________________________________



    if (WiFi.status() == WL_CONNECTED && WiFi.getMode() == WIFI_STA)
    {
      Serial.println("_______________________İLK______DENEME_____MQTT_____________");

      client.setServer(mqttServer, mqttPort);
      Serial.println("Sending message to MQTT topic..");
      client.connect("ESP32Client", mqttUser, mqttPassword );

      if (client.publish("Sinyal", JsonMqtt_char))
      {
        JsonArray O = doc.createNestedArray("O");
        Serial.println("Success sending message;");
        SetDisplayUrunAdet();
      }
      else
      {
        Serial.println("Error sending message");
      }
    }
    else
    {
      Serial.println("____________ATLADI______BİR______DAHA______DENEYECEK_______");

      if (wifiAttempt <= 3 && wifiManager.getWiFiIsSaved()) {

        wifiAttempt++;
        wifiManager.autoConnect(KeyIDMakina);
        Serial.println("Wifi Deneme Sayısı: " + String(wifiAttempt));
      }
      else
      {
        Serial.println("WİFİ DENEME SINIRINA ULAŞILDI = " + String(wifiAttempt));
        Serial.println("AP MODE KAPATILDI. Resetlenene kadar internet erişimi kesildi");
      }

      if (WiFi.status() == WL_CONNECTED && WiFi.getMode() == WIFI_STA)
      {
        //NTP_GET_TIME();
        client.setServer(mqttServer, mqttPort);
        Serial.println("Sending message to MQTT topic..");
        client.connect("ESP32Client", mqttUser, mqttPassword );
        delay(10);

        if (client.publish("Sinyal", JsonMqtt_char))
        {
          JsonArray O = doc.createNestedArray("O");
          Serial.println("Success sending message;");
          SetDisplayUrunAdet();
        }
        SetDisplayBirAdetUrunSuresi();
      }
      else {
        Serial.println("_______________________İKİNCİ_________________ATLADI");
      }
    }
    client.disconnect();
    GeriSayimSon = GeriSayimBasla;
  }
}


// ______________________________________ RFID WITH MASK ______________________________________ //
void RFID()
{

  mfrc522.PCD_Init();

  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

  byte block;
  byte len;

  MFRC522::StatusCode status;

  while (RFID_Kontrol != true)
  {

RFID_TekrarOku:

    //---------------------------------------------------------------------------------------

    if (  mfrc522.PICC_IsNewCardPresent()) {
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

      status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 1, &key, &(mfrc522.uid));
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
      Serial.println(I.length());
      if (I.toInt() < 1) I="";
    }
    else
    {
      if (mfrc522.PICC_IsNewCardPresent())
        goto Rfid_Yakala;
      digitalWrite (AktifAn, HIGH);
      digitalWrite (PasifAn, LOW);

      delay(750);

      if (mfrc522.PICC_IsNewCardPresent())
        goto Rfid_Yakala;

      digitalWrite (AktifAn, LOW);
      digitalWrite (PasifAn, HIGH);
      delay(750);
    }
  }
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();


  if (I == "99999") {
    wifiManager.resetSettings();
    //SPIFFS.format();
    Serial.println("SPIFFS FORMAT ___ WİFİ RESETLE");

    for (int Rstt = 0; Rstt <= 3; Rstt++) {
      lc.setChar(0, 5, '-', false);
      lc.setChar(0, 4, '-', false);
      lc.setChar(0, 3, '-', false);
      lc.setChar(0, 2, '-', false);
      lc.setChar(0, 1, '-', false);
      lc.setChar(0, 0, '-', false);
      delay(750);
      lc.clearDisplay(0);
      delay(750);
    }
  }
}

// ______________________________________ RTC ______________________________________ //

// ______________________________________ SET
void RTC_SET()
{
  Wire.setClock(100000);
  rtc.setSquareWave(SquareWaveDisable);
  rtc.begin();

  rtc.setDateTime(int(currentHour), int(currentMinute), int(currentSecond), int(monthDay), int(currentMonth), int(currentYear), int(0));


  Serial.println("RTC <<<GÜNCELLENDİ>>>>");

  // Set date/time: 12:34:56 31 December 2020 Sunday

}

//______________________________________ GET
void RTC_GET()
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

  currentHour = timeClient.getHours() + 3;
  currentMinute = timeClient.getMinutes();
  currentSecond = timeClient.getSeconds();
  // Serial.println("Saniye: "+ currentSecond);
  // Serial.println("Dakika: "+ currentMinute);
  // Serial.println("Saat: "+ currentHour);
  tm *ptm = gmtime ((time_t *)&epochTime);

  monthDay = ptm->tm_mday;
  currentMonth = ptm->tm_mon + 1;
  currentYear = ptm->tm_year + 1900;
  //  Serial.println("Gün: "+ monthDay);
  //  Serial.println("Ay: "+ currentMonth);
  //  Serial.println("Yıl: "+ currentYear);
}

//______________________________________ AdetRead ______________________________________ //
void AdetRead()
{
  File file = SPIFFS.open("/json.txt", "r");
  k = 0;
  while (file.available())
  {
    //Serial.write(file.read());
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

//______________________________________ AdetWrite ______________________________________ //
void AdetWrite()
{
  GunAdetJson = " ";
  serializeJson(doc1, GunAdetJson);
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

// ______________________________________ WIFI_SPIFFS_SORGU ______________________________________ //
void WIFI_SPIFFS_SORGU()
{

  if (SPIFFS.begin()) {
    if (SPIFFS.exists("/config.json"))
    {
      //file exists, reading and loading
      Serial.println("reading config file");
      File file = SPIFFS.open("/config.json", "r");
      if (file && file > 0) {
        k = 0;
        while (file.available())
        {
          //    Serial.write(file.read());
          IPler[k] = file.read();
          k++;
        }

        KaliciIpler = String(IPler);
        Serial.println("KalıcıIpler: " + KaliciIpler);
        DeserializationError error = deserializeJson(json, KaliciIpler);
        serializeJsonPretty(json, Serial);
        if (json["I_P"]) {
          Serial.println("Parsin JSON..");
          strcpy(IP, json["I_P"]);
          strcpy(GATEWAY, json["Gate_Way"]);
          strcpy(SUBNET, json["Sub_Net"]);
          strcpy(DNS, json["D_N_S"]);

          Serial.println("Kayıtta bulunun IPler yüklendi!");

        } else {
          Serial.println("JSON yüklerken HATA Oluştu..!..");
        }
      }
    }
    else {
      Serial.println("failed to mount FS");
    }
    file.close();
  }
}

// ______________________________________ SetDisplayUrunAdet ______________________________________ //
void SetDisplayUrunAdet()
{
  if (O.size() >= 1)
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

  else
  {
    if (i >= 0 && i <= 9) {
      lc.setDigit(0, 0, i - (int)i / 10 * 10, true);
    }
    else if (i >= 10 && i <= 99) {
      lc.setDigit(0, 1, ((int)i / 10 * 10 - (int)i / 100 * 100) / 10, false);
      lc.setDigit(0, 0, i - (int)i / 10 * 10, true);
    }
    else if (i >= 100 && i <= 999) {
      lc.setDigit(0, 2, ((int)i / 100 * 100 - (int)i / 1000 * 1000) / 100, false);
      lc.setDigit(0, 1, ((int)i / 10 * 10 - (int)i / 100 * 100) / 10, false);
      lc.setDigit(0, 0, i - (int)i / 10 * 10, true);
    }
    else if (i >= 1000 && i <= 9999) {
      lc.setDigit(0, 3, (int)i / 1000, false);
      lc.setDigit(0, 2, ((int)i / 100 * 100 - (int)i / 1000 * 1000) / 100, false);
      lc.setDigit(0, 1, ((int)i / 10 * 10 - (int)i / 100 * 100) / 10, false);
      lc.setDigit(0, 0, i - (int)i / 10 * 10, true);
    }
  }
}

// ______________________________________ SetDisplayBirAdetUrunSuresi ______________________________________ //
void SetDisplayBirAdetUrunSuresi()
{
  if (WiFi.status() == WL_CONNECTED && WiFi.getMode() == WIFI_STA)
  {
    if (j >= 0 && j <= 9) {
      lc.setDigit(0, 5, j - (int)j / 10 * 10, true);
    }
    else if (j >= 10 && j <= 99) {
      lc.setDigit(0, 4, ((int)j / 10 * 10 - (int)j / 100 * 100) / 10, false);
      lc.setDigit(0, 5, j - (int)j / 10 * 10, true);
    }
    else if (j >= 100 && j <= 999) {
      lc.setDigit(0, 4, ((int)j / 100 * 100 - (int)j / 1000 * 1000) / 100, false);
      lc.setDigit(0, 5, ((int)j / 10 * 10 - (int)j / 100 * 100) / 10, true);
      lc.setDigit(0, 6, j - (int)j / 10 * 10, false);
    }
    else if (j >= 1000 && j <= 9999) {
      lc.setDigit(0, 4, (int)j / 1000, false);
      lc.setDigit(0, 5, ((int)j / 100 * 100 - (int)j / 1000 * 1000) / 100, true);
      lc.setDigit(0, 6, ((int)j / 10 * 10 - (int)j / 100 * 100) / 10, false);
      lc.setDigit(0, 7, j - (int)j / 10 * 10, false);
    }
  }
  else
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
}


//_______________________________ANİMASYON

void SegnmentIntro() {

  //____________________"c"
  noktalar();
  delay(delaytime);
  lc.setRow(0, 5, 0x80);
  delay(delaytime);
  lc.setRow(0, 5, 0x00);
  delay(delaytime);
  lc.setRow(0, 4, 0x08);
  noktalar();
  delay(delaytime);
  lc.setRow(0, 5, 0x80);
  delay(delaytime);
  lc.setRow(0, 5, 0x00);
  delay(delaytime);
  lc.setRow(0, 4, 0x0C);
  delay(delaytime);
  noktalar();
  delay(delaytime);
  lc.setRow(0, 5, 0x80);
  delay(delaytime);
  lc.setRow(0, 5, 0x00);
  delay(delaytime);
  lc.setRow(0, 4, 0x0D);
  delay(delaytime);

  //_____________________"o"
  noktalar();
  delay(delaytime);
  lc.setRow(0, 5, 0x08);
  delay(delaytime);
  noktalar();
  delay(delaytime);
  lc.setRow(0, 5, 0x0C);
  delay(delaytime);
  noktalar();
  delay(delaytime);
  lc.setRow(0, 5, 0x0D);
  delay(delaytime);
  noktalar();
  delay(delaytime);
  lc.setRow(0, 5, 0x1D);
  delay(delaytime);

  //_____________________"c"|"o"
  lc.clearDisplay(0);
  delay(delaytime);
  lc.setRow(0, 5, 0x1D);
  lc.setRow(0, 4, 0x0D);
  delay(delaytime);
  lc.clearDisplay(0);
  delay(delaytime);
  lc.setRow(0, 5, 0x1D);
  lc.setRow(0, 4, 0x0D);
  delay(delaytime);
  lc.clearDisplay(0);
  delay(delaytime);
  lc.setRow(0, 5, 0x1D);
  lc.setRow(0, 4, 0x0D);
  delay(delaytime);
}
void noktalar() {
  lc.setRow(0, 0, 0x80);
  delay(delaytime);
  lc.setRow(0, 0, 0x00);
  lc.setRow(0, 1, 0x80);
  delay(delaytime);
  lc.setRow(0, 1, 0x00);
  lc.setRow(0, 2, 0x80);
  delay(delaytime);
  lc.setRow(0, 2, 0x00);
  lc.setRow(0, 3, 0x80);
  delay(delaytime);
  lc.setRow(0, 3, 0x00);
}
