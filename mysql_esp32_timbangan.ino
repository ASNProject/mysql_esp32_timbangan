/*
Alamat DirectAdmin: http://www.loadtrash.my.id:2222
Username: loadtras
Password: 87db[V4;jQuTY5
*/

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

// json 6.18.5
#include <ArduinoJson.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClient.h>

//WiFiClient client; // jika offline local host
WiFiClientSecure client; // jika online

const char* ssid     = "iotwifi";
const char* password = "Project123";

String ip_server = "https://loadtrash.my.id/";
String serverName =  ip_server + "timbangan5/write-data.php";
StaticJsonDocument<200> doc;
String pilihunit="UNIT5";

unsigned long previousMillis = 0;    
const long interval = 500;
float berat,lberat;
unsigned long timer=0;
#define pinled       2
#define pinbuzzer    23
String in="";

void connectwifi(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Connect Wifi");
  lcd.setCursor(0,1);
  lcd.print(ssid);
  digitalWrite(pinbuzzer,1);
  delay(500);
  digitalWrite(pinbuzzer,0);
  WiFi.begin(ssid, password);
  Serial.print("Connecting wifi: ");
  Serial.println(ssid);
  while (WiFi.status() != WL_CONNECTED) {
  delay(1000);
  digitalWrite(pinled,digitalRead(pinled)^1);
  
  lcd.setCursor(0,1);
  lcd.print("Wifi Code:");
  lcd.print(WiFi.status());
  
  }
  Serial.println();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Connected");
  delay(1000);
    
  Serial.println("Connected");
  Serial.print("IP DEVICE: ");
  Serial.println(WiFi.localIP());
}

void baca_timbangan(){
  if(Serial2.available()){
    char c = Serial2.read();
    Serial.write(c);
    if(isDigit(c)||c=='-'||c=='.')in+=c;
    if(c=='('){
    berat = in.toFloat();
    in="";
    }
  }
}
void setup() {
  
  pinMode(pinled, OUTPUT);
  pinMode(pinbuzzer, OUTPUT);
  
  // Initialize serial port
  Serial.begin(9600);
  Serial2.begin(2400);
  
  lcd.begin();
  lcd.backlight();

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(pilihunit);
  delay(1000);
  connectwifi(); 
  Serial.println(serverName);  
}   

int flag=0;
void loop() {
  baca_timbangan();
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;  
    digitalWrite( pinled, digitalRead( pinled ) ^ 1 );

    if(lberat!=berat){
      lberat=berat;
    }
    if(berat<0.1)timer=0;
    
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("BERAT:");
    lcd.print(berat);
    lcd.print("KG");
    lcd.setCursor(15,0);
    if(lberat==berat&&berat>0.1){
    timer++;
    if(flag==0){
      flag=1;
      lcd.write('_');
      }
    else {
      flag=0;
      lcd.write(0xff);
      } 
    }
    else{
    lcd.write('_');  
    }
    lcd.setCursor(0,1);
    if(timer==6&&berat>0.1){
    lcd.print("LOCK");
    // tulis_database(String(berat));
    postData(String(berat));
    }
    else lcd.print("UNLOCK");
  }
 
  test_manual();
}

void test_manual(){
  
  if(Serial.available()){
    char c = Serial.read();
    if(c!='#'){
      if(isDigit(c)||c=='.'||c=='-')in+=c;
    }
    if(c=='#'){
    Serial.println(in);
      tulis_database(in);
      in="";
    }
  }
  
}


void tulis_database(String _berat){
  
    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      //WiFiClient client; // jika offline
      WiFiClientSecure client; // jika online
      client.setInsecure();
      HTTPClient http;
      Serial.println("send database: ");
      String serverPath = serverName;
      /*
      serverPath += "?nama=";
      serverPath += "bejo";
      serverPath += "&nik=";
      serverPath += "121234";
      serverPath += "&alamat=";
      serverPath += "klx";
      serverPath += "&berat=";
      serverPath += _berat;
      */
      serverPath += "?berat=";
      serverPath += _berat;
      Serial.println(serverPath);
      // Your Domain name with URL path or IP address with path
      http.begin(client, serverPath.c_str());
      
      // Send HTTP GET request
      int httpResponseCode = http.GET();
        lcd.setCursor(7,1);
        lcd.print("CODE:");
        lcd.print(httpResponseCode);
        
      if (httpResponseCode>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();
        Serial.println(payload);
      }
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
       if(httpResponseCode==200){
        digitalWrite(pinbuzzer,1);
        delay(200);
        digitalWrite(pinbuzzer,0);
        }
        else{
        digitalWrite(pinbuzzer,1);
        delay(1000);
        digitalWrite(pinbuzzer,0);  
        }
      // Free resources
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
      connectwifi(); 
    }   
delay(1000);    
}

void postData(String berat) {
  if (WiFi.status() == WL_CONNECTED){
    HTTPClient http;
    Serial.println("Mengirim data: ");
    String serverPath = "https://app-574b3b2b-05f4-4a14-a811-fd215c1e4fdf.cleverapps.io/load";

    http.begin(serverPath.c_str());
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    /// Ganti code dengan Kode timbangan yang lainnya
    String postData = "code=T1&password=123456&value=" + berat; 
    Serial.println("Sending data: " + postData);

      // Send the POST request
    int httpResponseCode = http.POST(postData);

    lcd.setCursor(7, 1);
    lcd.print("CODE:");
    lcd.print(httpResponseCode);

    if (httpResponseCode == 200) {
      digitalWrite(pinbuzzer, 1);
      delay(200);
      digitalWrite(pinbuzzer, 0);
    } else {
      digitalWrite(pinbuzzer, 1);
      delay(1000);
      digitalWrite(pinbuzzer, 0);
    }

    // Free resources
    http.end();
  } else {
    Serial.println("WiFI Disconnected");
    connectwifi();
  }
  delay(1000);
}
