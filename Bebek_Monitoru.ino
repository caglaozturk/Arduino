#define BLYNK_PRINT Serial    // Comment this out to disable prints and save space
#include <ESP8266_Lib.h>
#include <BlynkSimpleShieldEsp8266.h>

#include "DFRobotDFPlayerMini.h"

// BLYNK uygulamasından email e gelen kod
char auth[] = "9970370163114e8fbbf0b5df2bad506b";

// WİFİ AYARLARI
char ssid[] = "Tilgin-9XqoaRtYz9zQ";
char pass[] = "beTjFTsoAFrun";
  

// or Software Serial on Uno, Nano...
#include <SoftwareSerial.h>
SoftwareSerial EspSerial(2, 3); // RX, TX

//  ESP8266 baud rate:
#define ESP8266_BAUD 9600

ESP8266 wifi(&EspSerial);



//DHT Setup
#include "DHT.h"
#define DHTPIN 12     // DHT11 DATA pin ARDUİNO PİN 12
DHT dht(DHTPIN, DHT11); 

float h = 0.1; // NEM
float t = 0.1; // SICAKLIK
unsigned long previousMillis = 0;
 
#define LED 13
#define SES 7 // ses modulunun OUT pini arduino pin 7
#define role 8 // röle pini
boolean durum = true;

// MP3 SETUP
SoftwareSerial mySoftwareSerial(10, 11); // RX, TX
DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);

BLYNK_WRITE(V2)
{
  int pinValue = param.asInt(); // uygulamadi müzik butonuna basılırsa müzik çal

 if(pinValue == 1){
    myDFPlayer.play(1);  // ilk mp3 dosyasını çal
  }
  else
  {
    myDFPlayer.pause();  //pause the mp3
  }
}


void setup()
{

  pinMode(SES,INPUT_PULLUP);
  pinMode(role, OUTPUT);
  digitalWrite(role, HIGH); // LOW olunca röle calısır
  
  dht.begin();
  // Set console baud rate
  Serial.begin(9600);
   mySoftwareSerial.begin(9600); // mp3 modül serial 
  delay(10);
  // Set ESP8266 baud rate
  EspSerial.begin(ESP8266_BAUD); // ESP baud RATE
  delay(10);

  Blynk.begin(auth, wifi, ssid, pass);

 // timer.setInterval(1000L, blinkLedWidget);
 // eMail(); //   email gonder.

 pinMode(LED, OUTPUT); // arduino 13 numaralı pindeki LED, çıkış olarak tanımla
 digitalWrite(LED, LOW); // LED i söndür


// ***** MP3 player ayarları  ********  //

if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.

    Serial.println(F("\t !!!!!!!!!!!!!!!!!!!!!!!!!!! "));
  }
  //Serial.println(F("MP3 PLAYER online."));
 
 myDFPlayer.setTimeOut(500); //Set serial communictaion time out 500ms

  //----Set volume----
  myDFPlayer.volume(30);  //Set volume value (0~30).   30  max
  myDFPlayer.volumeUp(); //Volume Up
  myDFPlayer.volumeDown(); //Volume Down
  
  //----Set different EQ----
  myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);
//  myDFPlayer.EQ(DFPLAYER_EQ_POP);
//  myDFPlayer.EQ(DFPLAYER_EQ_ROCK);
//  myDFPlayer.EQ(DFPLAYER_EQ_JAZZ);
//  myDFPlayer.EQ(DFPLAYER_EQ_CLASSIC);
//  myDFPlayer.EQ(DFPLAYER_EQ_BASS);
  
  //----Set device we use SD as default----
//  myDFPlayer.outputDevice(DFPLAYER_DEVICE_U_DISK);
  myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);
//  myDFPlayer.outputDevice(DFPLAYER_DEVICE_AUX);
//  myDFPlayer.outputDevice(DFPLAYER_DEVICE_SLEEP);
//  myDFPlayer.outputDevice(DFPLAYER_DEVICE_FLASH);


  Blynk.notify("PROGRAM ONLINE! "); // bildirim gönder
}


void loop()
{               
 
     // DHT SENSOR
     // 30 saniyede bir DHT sensor verilerini al
     // virtual pin V5 ve V6 ya gönder.
     // V5 NEM
     // V6 SICAKLIK
    
    SICAKLIK(); // her 30 saniyede bir sıcaklık ve nem i ölç

    Serial.println(digitalRead(SES));
    if(durum == true){  // durum true ise ses varmı diye kontrol et. müzik calarken false yap
     GURULTU();         // gürültü varmı?
    }
    Blynk.run();

     if(digitalRead(SES) == HIGH){     // ses yoksa
          delay(1000);                    // 1 saniye bekle
         // if(digitalRead(SES) == HIGH){   // yine ses yoksa emin olmak için
           // delay(1000);                  // 1 saniye daha bekle
            if(digitalRead(SES) == HIGH){ // hala ses yoksa müzik bitmiş demektir.
              durum = true;               // ses algılamayı aktif et. yani sesi dinle
            }
          //}
        }
        
}


 

void eMail(){
  Blynk.email("cagla@ozturkcagla.com", "Bebek", "Heyyy Bebek Ağlıyor!....");
  }
  


void DHTsensor(){
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
 h = (float)dht.readHumidity();
  // Read temperature as Celsius (the default)
 t = (float)dht.readTemperature();
 
}



void SICAKLIK(){
  unsigned long currentMillis = millis();
    if(currentMillis - previousMillis >= 30000) // 30 saniyede bir
    {   
      previousMillis = currentMillis;              
      DHTsensor();
      Blynk.virtualWrite(V5, h);
      Blynk.virtualWrite(V6, t);

      Serial.print("SICAKLIK: ");
      Serial.println(t);
      Serial.print("    ");
      Serial.print("NEM: ");
      Serial.println(h);
      
    }
}
void GURULTU(){

   if(digitalRead(SES)== LOW){ // eğer ses modülü ses algılarsa
      delay(100);
       // if(digitalRead(SES) == LOW){
        
             
            //  delay(1000); // 5 sn bekle
              
              if(digitalRead(SES) == LOW){ // eğer 5 saniye sonra SES modülü hala ses algılıyorsa mp3 çal
                 Blynk.notify(" HEYYYY BEBEK AĞLIYOR!... "); // bildirim gönder
                 myDFPlayer.play(1);  // ilk mp3 dosyasını çal
                 eMail();  //*************   <<----- email gönder
                 
                 Serial.println("MÜZİK ÇALIYOR");
                 Serial.println("TELEFONA BİLDİRİM GÖNDERİLDİ!");
                 
                 Blynk.virtualWrite(V1, 255); // app deki LED i yak
                 durum = false; // ses algılamayı devre dışı bırak çünkü müzik çalıyor.
                 delay(1000);
              }
        //}
    } else {
        Blynk.virtualWrite(V1, 0); // ses yoksa app deki LEDi söndür
        
        
    }
    
}
