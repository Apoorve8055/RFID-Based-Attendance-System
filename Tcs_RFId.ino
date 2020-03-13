     /*
     * @author: Apoorve Kumar Verma
     * RFID Based Attendance System
    */

    #include <ESP8266WebServer.h>
    #include <ESP8266HTTPClient.h>
    #include <ESP8266WiFi.h> 
    #include <SPI.h>
    #include <MFRC522.h>
    
    #define SS_PIN 4  //D2
    #define RST_PIN 5 //D1

    //Access point credentials
    const char* ssid = "iot";
    const char* pwd = "+++++++";
  
    WiFiServer server(85);  // open port 80 for server connection
    MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

   //wifi connection
    void wifi_con()
    {
      //********************* SPI MFRC522 Config *************************
      
      Serial.begin(115200);   // Initiate a serial communication
      SPI.begin();          // Initiate  SPI bus
      mfrc522.PCD_Init();   // Initiate MFRC522
      pinMode(LED_BUILTIN, OUTPUT);
      delay(10);
      
      //********************* connecting to wifi *******************************
      
      Serial.println();
      Serial.println();
      Serial.print("Connecting to wifi ");
      Serial.println(ssid);
      WiFi.begin(ssid, pwd);
      //attempt to connect to wifi
      while (WiFi.status() != WL_CONNECTED)
      {
        delay(500);
        Serial.print("."); //progress with dots until the wifi is connected
      }
        Serial.println("");
        //while connected print this
        Serial.println("Wifi connected");
        //starting the server
        Serial.println("Server started.");
        //get the ip address and print it
        Serial.print("This is your ip address: ");
        Serial.print("http://");
        Serial.print(WiFi.localIP());
        Serial.println("/");  
    }
    

    // BY PASS THE LINK USING USING GET METHOD  
    void http_upload_get(String para)
    {
        HTTPClient http;//Declare object of class HTTPClient
        http.begin(para);//Specify request destination
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");//Specify content-type header
        int httpCode = http.GET();//Send the request
        String payload = http.getString();//Get the response payload
        Serial.println(httpCode);//Print HTTP return code
        Serial.println(payload);//Print request response payload
        http.end();  //Close connection   
      
      }
      //==============================================================
String urldecode(String str)
{
    
    String encodedString="";
    char c;
    char code0;
    char code1;
    for (int i =0; i < str.length(); i++){
        c=str.charAt(i);
      if (c == '+'){
        encodedString+=' ';  
      }else if (c == '%') {
        i++;
        code0=str.charAt(i);
        i++;
        code1=str.charAt(i);
        c = (h2int(code0) << 4) | h2int(code1);
        encodedString+=c;
      } else{
        
        encodedString+=c;  
      }
      
      yield();
    }
    
   return encodedString;
}
 
String urlencode(String str)
{
    String encodedString="";
    char c;
    char code0;
    char code1;
    char code2;
    for (int i =0; i < str.length(); i++){
      c=str.charAt(i);
      if (c == ' '){
        encodedString+= '+';
      } else if (isalnum(c)){
        encodedString+=c;
      } else{
        code1=(c & 0xf)+'0';
        if ((c & 0xf) >9){
            code1=(c & 0xf) - 10 + 'A';
        }
        c=(c>>4)&0xf;
        code0=c+'0';
        if (c > 9){
            code0=c - 10 + 'A';
        }
        code2='\0';
        encodedString+='%';
        encodedString+=code0;
        encodedString+=code1;
        //encodedString+=code2;
      }
      yield();
    }
    return encodedString;
    
}

unsigned char h2int(char c)
{
    if (c >= '0' && c <='9'){
        return((unsigned char)c - '0');
    }
    if (c >= 'a' && c <='f'){
        return((unsigned char)c - 'a' + 10);
    }
    if (c >= 'A' && c <='F'){
        return((unsigned char)c - 'A' + 10);
    }
    return(0);
}
    
void setup() 
{
      wifi_con(); 
}

void loop()
{
    
  
 // +++++++++++++++++++++++ RFID Handling Featch UID +++++++++++++++++++++++++++ 
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
     digitalWrite(LED_BUILTIN, HIGH); //Card Touched
    return;
  }
  
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  //Show UID on serial monitor
  String content= "";
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  content.toUpperCase();
  Serial.println();
  Serial.println(" UID tag :" + content.substring(1));
  digitalWrite(LED_BUILTIN, LOW);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second


 Serial.println(urldecode(urlencode(content.substring(1))));
  
//******************************************* Send Request ************************************************

  String url ="http://webhook.site/f143e1f6-ffb4-4347-8a36-9369341c1716?id="+urlencode(content.substring(1));
  http_upload_get(url);

}
