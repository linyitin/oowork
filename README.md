#學校網管及智慧停車系統<br>

#指導老師：朱彥銘<br>

#組員：莊博馨、曾瓊瑩、蔡心敏、林薏庭、陳沛穎<br>

內容:從Client端Ping各段閘道網路，校外網路用DNS測試，定時將ping的值回傳到Splunk,再由Splunk分析，若回傳時間超過訂時間，便發出Alert提醒網管人員．
而智慧停車系統則是，透過感測器偵測停車格是否有停車，將資料回傳至Splunk監看，再進行分析．<br>

效益:讓網管人員即時知道哪段網路出問題，且可以即時知道校園內停車狀況．<br>


HOW:用PYTHON寫好的程式，使用MQTT傳輸協定傳送至SPLUNK，在使用SPLUNK告警功能發出錯誤的LOG檔<br>
功能性需求<br>
1.偵測是否斷線<br>

2.監控網段連線狀態<br>

3.有異常的網段發出告警<br>
4.分析各時段的連線品質<br>

5.偵測校園那各停車格<br>
6.監控停車狀態<br>
7.分析各感測器狀態<br>



<br>非功能性需求
<br>1.有異常即時給使用者知道
<br>2.當地磁感測器沒電讓管理者知道

***






甘特圖設計https://www.smartapp.com/gantterforgoogledrive/index.html?fileID=0B2NixI49LfkmSXF6SVlWZFc2Rms#



![](01.PNG)

 ***
負責人 ：蔡心敏
  程式碼
***
負責人 ：林薏庭
  程式碼
***
負責人 ：曾瓊瑩
  程式碼
***
  負責人 ：陳沛穎<br>
  程式碼<br>
  GMS1001+7697+6509<br>
     
#include <RHSoftwareSPI.h><br>
#include <RHGenericSPI.h><br>
#include <SPI.h><br>

#include <SPI.h><br>
#include <Ethernet.h><br>


//Sensor Setup<br>
//Ethernet Shield Setup<br>
// Enter a MAC address for your controller below.<br>
// Newer Ethernet shields have a MAC address printed on a sticker on the shield<br>
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };<br>
// if you don't want to use DNS (and reduce your sketch size)<br>
// use the numeric IP instead of the name for the server:<br>
IPAddress server(10,14,0,85);  // numeric IP for Google (no DNS)<br>
//char server[] = "www.google.com";    // name address for Google (using DNS)<br>
//If you want to use DNS for your server<br>
// Set the static IP address for the ethernet shield<br>
// to use if the DHCP fails to assign<br>
IPAddress ip(168,95,1,1);<br>

// Initialize the Ethernet client library<br>
// with the IP address and port of the server<br>
// that you want to connect to (port 80 is default for HTTP):<br>
EthernetClient client;<br>
DHT dht(DHTPIN, DHTTYPE);<br>

void setup() {<br>
  // Open serial communications and wait for port to open:<br>
  Serial.begin(9600);<br>
  //just a startup delay, to let the little guy wake up<br>
  delay(1000);<br>
  // start the Ethernet connection:<br>
  if (Ethernet.begin(mac) == 0) {<br>
    Serial.println("Failed to configure Ethernet using DHCP");<br>
    // try to congifure using IP address instead of DHCP:<br>
    Ethernet.begin(mac, ip);<br>
  }<br>
  // give the Ethernet shield a second to initialize:<br>
  Serial.print("My IP address: ");<br>
  // Print out the IP address, in case you need to wireshark the transaction<br>
  Serial.println(Ethernet.localIP());<br>
  
  //init our DHT11 temp sensor<br>
  dht.begin();<br>
}<br>

void loop()<br>
{<br>
  delay(5000);<br>
  // Reading temperature or humidity takes about 250 milliseconds!<br>
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)<br>
  float h = dht.readHumidity();<br>
  // Read temperature as Fahrenheit (isFahrenheit = true)<br>
  float f = dht.readTemperature(true);<br>
  // Check if any reads failed and exit early (to try again).<br>
  if (isnan(h) || isnan(f)) {<br>
    Serial.println("Failed to read from DHT sensor!");<br>
  }<br>
<br>
  //quick read of the incoing traffic on the shield...if any<br>
  if (client.available()) {<br>
    char c = client.read();<br>
  }<br>

  //wait a bit more 10000 millis before sending the request to Splunk<br>
  delay(10000);<br>
  sendHttpRequest(h, f);<br>
  
<br>

}<br>

void sendHttpRequest(float h, float f){<br>
  //close any current connections<br>
  client.stop();<br>
  //set up our payload as a string<br>
  String stringF = String(f, 3);<br>
  String stringH = String(h, 3);<br>
  String payload = "{ \"host\" : \"arduino\", \"sourcetype\" : \"arduino\", \"index\" : \"arduino\", \"event\" :  {\"temp\" : \"" + stringF + "\" , \"humidity\": \"" + stringH + "\" }}";<br>

  // if you get a connection, report back via serial<br>
  //also set client port to 8088 default for HTTP event collector<br>
  if (client.connect(server, 8088)) {<br>
    Serial.println("connected");<br>
    // Make an HTTP POST request to our event collector endpoint<br>
    client.println("POST /services/collector HTTP/1.1");<br>
    // add our authorization header<br>
    // add your key below after "Splunk"<br>
    client.println("Authorization: Splunk 2B6461AC-FE4E-4EE6-80B0-11E38DF58C2D");<br>
    //send our JSON payload<br>
    //uncomment below if you care about the content-type headers...I do not<br>
    //client.println("Content-Type: application/x-www-form-urlencoded;");<br>
    //Content-Length header is ABSOLUTELY required, otherwise Splunk doesnt know<br>
    //how long to keep the connection open<br>
    client.print("Content-Length: ");<br>
    client.println(payload.length());<br>
    //required to add a space to delineate our payload from the header info<br>
    client.println();<br>
    client.println(payload);<br>
    client.println();<br>
  }<br>
  else {<br>
    // if you didn't get a connection to the server:<br>
    Serial.println("connection failed");<br>
  }<br>
<br>
}<br>
 ***
#負責人 ：莊博馨
##程式碼

***
＃期末簡報

![](投影片01.jpg)
![](投影片02.jpg)
![](投影片03.jpg)
![](投影片04.jpg)
![](投影片05.jpg)
![](投影片06.jpg)
![](投影片07.jpg)
![](投影片08.jpg)
![](投影片09.jpg)
![](投影片10.jpg)
![](投影片11.jpg)
![](投影片12.jpg)
![](投影片13.jpg)
![](投影片14.jpg)
![](投影片15.jpg)
![](投影片16.jpg)
![](投影片17.jpg)
![](投影片18.jpg)
![](投影片19.jpg)
![](投影片20.jpg)
![](投影片21.jpg)
![](投影片22.jpg)
![](投影片23.jpg)
![](投影片24.jpg)
![](投影片25.jpg)
![](投影片26.jpg)
![](投影片27.jpg)
![](投影片28.jpg)
![](投影片29.jpg)
![](投影片30.jpg)
![](投影片31.jpg)
![](投影片32.jpg)
![](投影片33.jpg)
![](投影片34.jpg)
![](投影片35.jpg)
![](投影片36.jpg)
![](投影片37.jpg)
![](投影片38.jpg)
![](投影片39.jpg)
![](投影片40.jpg)
![](投影片41.jpg)
![](投影片42.jpg)
![](投影片43.jpg)
![](投影片44.jpg)
![](投影片45.jpg)
![](投影片46.jpg)
![](投影片47.jpg)
![](投影片48.jpg)
![](投影片49.jpg)
![](投影片50.jpg)
![](投影片51.jpg)



