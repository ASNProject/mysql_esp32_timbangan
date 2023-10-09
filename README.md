# mysql_esp32_timbangan

API
```
https://app-574b3b2b-05f4-4a14-a811-fd215c1e4fdf.cleverapps.io/load
```


Post API 
```
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
```
