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

***
負責人 ：林薏庭

***
負責人 ：曾瓊瑩

***
  負責人 ：陳沛穎<br>
  使用的工具有GMS1001/7697/6509(lora)，由於需要感應車以磁場來判斷，所以使用GMS1001來判斷附近磁場，而選用lora是因為他能傳輸的距離可以15至20公，而且又低耗電，將GMS1001(地磁感測模組)焊接，接上7697以及6509，透過GMS1001感測，將資料讀取出來並且切割成容易閱讀的格式，接下來將ＬＥＤ裝上，寫上燈號控制，一開始環境學習為藍燈，為停放車為綠燈，以停放車為紅燈，由於各個地方的磁場不同後續加上校正，讓感測更正確．<br>
  程式碼<br>
  GMS1001+7697+6509<br>
     
#include<SoftwareSerial.h><br>
#define LEDG 11<br>
#define LEDB 10<br>
#define LEDR 9<br>

int raw_data;<br>
String  raw_hex_data[255];<br>
SoftwareSerial loraSerial(2, 3);// RX,TX<br>
unsigned long time;<br>
int status;<br>
int  s0,s4,s5;<br>


char A[24]=<br>{0x14,0x02,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x01,0xE8};<br>
//強制校正<br>
char B[4]={0xAA,0x00,0x01,0x01};<br>
//查詢<br>
char C[4]={0xAA,0x00,0x04,0x04};<br>
<br>
<br>
void setup() {<br>
  Serial.begin(9600);  //Serial Monitor.<br>
  Serial1.begin(9600);  //USB-to-UART converter<br>
  loraSerial.begin(9600); //LoRa Serial<br>
 //燈號<br>
  pinMode(LEDR, OUTPUT);<br>
  pinMode(LEDG, OUTPUT);<br>
  pinMode(LEDB, OUTPUT);<br>
  digitalWrite(LEDR, LOW);<br>
  digitalWrite(LEDG, LOW);<br>
  digitalWrite(LEDB, HIGH);<br>
  //SLPEN input<br>
pinMode(0, OUTPUT);<br>
pinMode(4, INPUT);<br>
pinMode(5, INPUT);<br>
//10秒<br>
  Serial.print("RAW DATA : ");<br>
  Serial.write(&A[0],24); <br>
  Serial.write(&B[0],4);<br>
} <br>

void loop() {<br>
//digitalWrite(0, LOW);<br>
s0=digitalRead(0);<br>
s4=digitalRead(4);<br>
s5=digitalRead(5);<br>
delay(10);<br>
//Serial.print("SLPEN");<br>
//Serial.println(0);<br>
//Serial.print("read");<br>
//Serial.println(4);<br>
//Serial.print("stat");<br>
//Serial.println(s5);<br>
  
  if (Serial1.available() > 0) {<br>
    Serial.print("RAW DATA : ");<br>
    for (int i = 0 ; i < 12 ; i++) {<br>
      //Get raw data<br>
      raw_data = Serial1.read();<br>
      //Decimal to hex and store the data to array<br>
      printHex(raw_data, 2);<br>
      raw_hex_data[i] = String(returnHex(raw_data, 2));<br>
      Serial.print(" ");<br>
      if (i == 11) {<br>
        //print status<br>
        Serial.println(" ");<br>
        Serial.print("Payload Header幀頭 : ");<br>
        Serial.println(raw_hex_data[0]);<br>
        Serial.print("Payload length 數據長度: ");<br>
        Serial.println(raw_hex_data[1]);<br>
        Serial.print("Signaling coding 信令編碼: ");<br>
        Serial.println(raw_hex_data[2]);<br>
        Serial.print("Device ID : ");<br>
        Serial.println(raw_hex_data[3] + " " + raw_hex_data[4] + " " + raw_hex_data[5] + " " + raw_hex_data[6]);<br>
        Serial.print("Status 狀態: ");<br>
        Serial.println(raw_hex_data[7]);<br>
       <br>
        Serial.print("Voltage 電壓: ");<br>
        Serial.println(raw_hex_data[8]);<br>
        Serial.print("Magnetic strength磁擾強度: ");<br>
        Serial.println(raw_hex_data[9] + " " + raw_hex_data[10]);<br>
        Serial.print("Checksum 效驗碼: ");<br>
        Serial.println(raw_hex_data[11]);<br>
        <br>
        if (raw_hex_data[7] == "00") {<br>
          Serial.print("Is there a Car?: ");<br>
          Serial.println("Haven't car");<br>
          loraSerial.println("AT+DTX=2,00");<br>
          String txtMsg = loraSerial.readString();<br>
          Serial.println(txtMsg);<br>
          digitalWrite(LEDR, LOW);<br>
          digitalWrite(LEDG, HIGH);<br>
          digitalWrite(LEDB, LOW);<br>
        }<br>
        else if (raw_hex_data[7] == "01") {<br>
          Serial.print("Is there a Car?: ");<br>
          Serial.println("Have car");<br>
          loraSerial.println("AT+DTX=2,01");<br>
          String txtMsg = loraSerial.readString();<br>
          Serial.println(txtMsg);<br>
          digitalWrite(LEDR, HIGH);<br>
          digitalWrite(LEDG, LOW);<br>
          digitalWrite(LEDB, LOW);<br>
        }<br>
        else if (raw_hex_data[7] == "02") {<br>
          Serial.println("Test");<br>
          loraSerial.println("AT+DTX=2,02");<br>
          String txtMsg = loraSerial.readString();<br>
          Serial.println(txtMsg);<br>
          digitalWrite(LEDR, LOW);<br>
          digitalWrite(LEDG, LOW);<br>
          digitalWrite(LEDB, HIGH);<br>
<br>
        }
        Serial.println("------------------------------------------------------");<br>
      }<br>
    }<br>
  }<br>
}<br>

<br>


//Decimal to Hex function<br>
void printHex(int num, int precision) {<br>
  char tmp[16];<br>
  char format[128];<br>
  sprintf(format, "%%.%dX", precision);<br>
  sprintf(tmp, format, num);<br>
<br>
  Serial.print(tmp);<br>
}<br>

//Decimal to Hex function<br>
String returnHex(int num, int precision) {<br>
  char tmp[16];<br>
  char format[128];<br>
<br>
  sprintf(format, "%%.%dX", precision);<br>
  sprintf(tmp, format, num);<br>
<br>
  return tmp;<br>
}<br>
<br>

 ***
#負責人 ：莊博馨<br>

***
＃期末簡報<br>

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



