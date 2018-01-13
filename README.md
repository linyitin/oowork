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



![](甘特圖.png)

 ***
負責人 ：蔡心敏<br>
對校外網路測試程式，因為學校的網路設定不能ping校外的網路，所以用對Dns傳封包的方式測試，在程式測試部分，rawsocket寫封包測試，下面是我們程式。目前使用兩個dns做測試，一個是googleDNS伺服器，另一個是hinet伺服器，原因是如果其中一台伺服器如果掛掉，另外一台伺服器還有資料上來，就代表學校網路沒有異常；如果兩台資料都傳不到就是學校網路有異常。

對校內網路測試程式對各棟閘道tracet route，是一種電腦網路工具。它可顯示封包在IP網路經過的路由器的IP位址，如果在傳送封包的過程中，在某一個節點中斷，就代表該節點出問題。校內網路如果電腦連線的是無線網路或是校外的網路是無法做tracet route，所以要直接用伺服器或是學校的有線網路做測試。

***
負責人 ：林薏庭<br>
一開始先在自己的電腦測試和Splunk測試，把資料丟到splunk前要先開PC的防火牆，然後再Splunk開port，利用socket程式把測試訊息丟到Splunk上面。通訊協定有分tcp、udp、http，先用udp方式傳。在PC測試成功後，便能在伺服器上面做測試，首先要先開伺服器的防火牆，在iptable上面開一個port做資料傳送的橋樑。
plunk部分主要是做資料分析，當發生異常會發出email通知管理者，所以splunk就要做alert設置。前面兩個程式測試裡面，如果發生錯誤的話資料會有erorr的訊息在裡面，在資料搜尋時對回傳的結果定如果有eroor便傳訊息給管理者的設定。此外也會監控每時段傳送訊息的數值有無發生異常，若發生異常也能立即告知server端測試的同學，檢查是否是程式出問題

***
負責人 ：曾瓊瑩<br>
測試的工作就是看server端這邊有沒有發生異常，splunk那邊的筆數異常，就會從伺服端這邊做檢查程式有沒有正常運作，在server的crontab會有做設定程式執行，所以如果筆數異常的話有可能就是這邊的設定出問題，必須將之前執行的程式做關閉，然後crontab做好設定，程式才能正常運作
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
將Splunk所接收到的Lora資料與預先上傳的位置資料結合，製作成停車場監控儀表板。<br>
內容含有：當下停車資訊與時間、Lora剩餘電量與溫度，停車場地圖與停車狀態，每日停車百分比，每小時停車次數，原始LOG資料。<br>
程式碼：<br>
<.form><br>
  <.label>校園停車場即時監控系統04<./label><br>
  <.fieldset submitButton="false" autoRun="true"><./fieldset><br>
  <.row><br>
    <.panel><br>
      <title>各停車格資訊</title><br>
      <.table><br>
        <title>Alarm：綠色－無停車　橘色－有停車　紅色－停車超過一小時，power：藍色－電量正常　黃色－電量過低</title><br>
        <.search><br>
          <.query>|inputlookup NKFUST_DevicePixel_test  <br>
            |append [search source="udp:5567"  | regex macAddr="^000000000d0100[0-9a-z][0-9a-z]$"<br>
            |rex field=data "fc000105(?&lt;ParkData&gt;\d{2})" <br>
            |rex field=data "fc000105\S{8}(?&lt;TempData&gt;\S{2})" |eval Temperature=tostring(tonumber(TempData,16))+"℃"<br>
            |rex field=data "fc000105\S{2}(?&lt;Voltage_hex&gt;\S{4})" |eval Voltage=round(tonumber(Voltage_hex,16)/1000,3)<br>
            |rex field=data "fc000105\S{6}(?&lt;Magnetic&gt;\S{2})" |eval Magnetic_disturbance=tonumber(Magnetic,16)<br>
            |eval ParkStatus=case(ParkData=="02","Not yet learn",ParkData=="22","Had Learn",ParkData=="21","Parking",ParkData=="23","Keep Parking",ParkData=="60","No Car Parking",ParkData=="61","Parking",ParkData=="62","Keep Correcttion",ParkData=="63","Had Correcttion")<br>
            | transaction data maxspan=30s<br>
            |eval secondsAgoStr=tonumber(now() - _time) <br>
            |eval Alarm = if(ParkData=21  OR ParkData=23 OR ParkData=61 OR ParkData=63, if (secondsAgoStr&gt;3600 ,"ParkOver1hr","Parking" ) , "NoCarParking" )<br>
            |eval power = if (Voltage&gt;2.6, "normal" , "low")<br>
            |eval ParkingTime = if(ParkData=21  OR ParkData=23 OR ParkData=61 OR ParkData=63,tostring(secondsAgoStr,"duration"),"-")<br>
            |dedup macAddr ]|table macAddr data ParkData ParkStatus ParkingTime Alarm location  Voltage power Magnetic_disturbance Temperature _time|selfjoin macAddr|dedup macAddr<./query><br>
          <.earliest>0<./earliest><br>
          <.latest><./latest><br>
          <.refresh>1m<./refresh><br>
          <.refreshType>delay</refreshType><br>
        <./search><br>
        <.format type="color" field="Alarm"><br>
          <.colorPalette type="map">{"ParkOver1hr":#FF8E7F,"Parking":#FFE480,"NoCarParking":#A3FF80}</colorPalette><br>
        <./format><br>
        <.format type="number" field="Voltage"><br>
          <.option name="precision">3<./option><br>
          <.option name="unit">v<./option><br>
        <./format><br>
        <.format type="color" field="power"><br>
          <.colorPalette type="map">{"low":#FEFF80,"normal":#80FFDA}<./colorPalette><br>
        <./format><br>
      <./table><br>
    <./panel><br>
  <./row><br>
  <.row><br>
    <.panel><br>
      <title>停車格位置圖</title><br>
      <.viz type="custom-image-map-viz.map_viz"><br>
        <title>綠色－無停車　橘色－有停車　紅色－停車超過一小時　灰色：預設建置點</title><br>
        <.search><br>
          <.query>|inputlookup NKFUST_DevicePixel_test  <br>
            |appendcols[search source="udp:5567" | regex macAddr="^000000000d0100[0-9a-z][0-9a-z]$" <br>
            |rex field=data "fc000105(?&lt;ParkData&gt;\d{2})" <br>
            |eval ParkStatus=case(ParkData=="02","Not yet learn",ParkData=="22","Had Learn",ParkData=="21","Parking",ParkData=="23","Keep Parking",ParkData=="60","No Car Parking",ParkData=="61","Parking",ParkData=="62","Keep Correcttion",ParkData=="63","Had Correcttion")<br>
            |eval secondsAgoStr=tonumber(now() - _time) |dedup macAddr |sort macAddr<br>
            |table macAddr data ParkData ParkStatus  _time time secondsAgoStr rssi snr ]<br>
            |rex field=macAddr "000000000d0100(?&lt;ID&gt;\w{2})"<br>
            |eval newy=Pixel_Y, newx=Pixel_X, coordinates=newy.",".newx ,description=ID,markerColor=if(ParkData=21 OR ParkData=23 OR ParkData=61 OR ParkData=63,if(secondsAgoStr&gt;3600,"lightred","beige"),if(ParkData=21 OR ParkData=22 OR ParkData=60 OR ParkData=62,"lightgreen","lightgray")),iconColor="#fbff7a"<br>
            |eval status=if(ParkData=21 OR ParkData=23 OR ParkData=61 OR ParkData=63,"Parking","NoCarParking")<br>
            | table description,coordinates,newx,newy,ParkStatus,markerColor,iconColor<./query><br>
          <.earliest>0<./earliest><br>
          <.latest><./latest><br>
          <.refresh>1m<./refresh><br>
          <.refreshType>delay<./refreshType><br>
        <./search><br>
        <.option name="custom-image-map-viz.map_viz.allPopups">0<./option><br>
        <.option name="custom-image-map-viz.map_viz.backgroundColor">#FFF<./option><br>
        <.option name="custom-image-map-viz.map_viz.defaultHeight">460<./option><br>
        <.option name="custom-image-map-viz.map_viz.drilldown">0<./option><br>
        <.option name="custom-image-map-viz.map_viz.focusClicked">1<./option><br>
        <.option name="custom-image-map-viz.map_viz.fullScreen">1<./option><br>
        <.option name="custom-image-map-viz.map_viz.mapCenterX">565<./option><br>
        <.option name="custom-image-map-viz.map_viz.mapCenterY">525<./option><br>
        <.option name="custom-image-map-viz.map_viz.mapCenterZoom">0<./option><br>
        <.option name="custom-image-map-viz.map_viz.mapHeight">840<./option><br>
        <.option name="custom-image-map-viz.map_viz.mapImage">east_schoolmap.PNG<./option><br>
        <.option name="custom-image-map-viz.map_viz.mapWidth">1166<./option><br>
        <.option name="custom-image-map-viz.map_viz.maxZoom">0<./option><br>
        <.option name="custom-image-map-viz.map_viz.minZoom">0<./option><br>
        <.option name="custom-image-map-viz.map_viz.multiplePopups">0<./option><br>
        <.option name="custom-image-map-viz.map_viz.scrollWheelZoom">1<./option><br>
        <.option name="custom-image-map-viz.map_viz.showPath">0<./option><br>
        <.option name="custom-image-map-viz.map_viz.showPointerCoordinates">1<./option><br>
        <.option name="custom-image-map-viz.map_viz.unfocusedOpacity">0.1<./option><br>
        <.option name="refresh.display">preview<./option><br>
      <./viz><br>
    <./panel><br>
  <./row><br>
  <.row><br>
    <.panel><br>
      <title>每日停車百分比</title><br>
      <.chart><br>
        <.search><br>
          <.query>source="udp:5567"  earliest=-1d@d | regex macAddr="^000000000d0100[0-9a-z][0-9a-z]$"<br>
            |rex field=data "fc000105(?&lt;ParkData&gt;\d{2})" <br>
            |eval ParkStatus=case(ParkData=="02","Not yet learn",ParkData=="22","Had Learn",ParkData=="21","Parking",ParkData=="23","Keep Parking",ParkData=="60","No Car Parking",ParkData=="61","Parking",ParkData=="62","Keep Correcttion",ParkData=="63","Had Correcttion")<br>
            |eval secondsAgoStr=tonumber(now() - _time) <br>
            |table macAddr data ParkData ParkStatus  _time time secondsAgoStr   rssi snr <br>
            |transaction data maxspan=30s //將資料聚集成交易模式，時間是30s內的資料同個DATA的 <br>
            |transaction macAddr  startswith="ParkData=61" endswith="ParkData=60" |eval TotalTime=toString(duration,"duration") |rename TotalTime as "此次停車總時間" <br>
            |eventstats sum(duration) AS Today_Parking_TotalTime <br>
            |eval percentage=round(duration/Today_Parking_TotalTime*100 ,2) |eval percentage=tostring(percentage+"%")<br>
            | addcoltotals labelfield=Today_Parking_TotalTime label=Today_Parking_TotalTime <br>
            |fields - _raw ,- closed_txn ,- field_match_sum , - linecount ,- Today_Parking_TotalTime <br>
            |chart values(duration) over _time |rename values(duration) AS "每日停車時間與百分比"<./query><br>
          <.earliest>0<./earliest><br>
          <.latest><./latest><br>
          <.refresh>1m<./refresh><br>
          <.refreshType>delay<./refreshType><br>
        <./search><br>
        <.option name="charting.chart">pie<./option><br>
      <./chart><br>
    <./panel><br>
    <.panel><br>
      <title>每日最低電量</title><br>
      <.input type="radio" token="macAddr04"><br>
        <.label>MAC<./label><br>
        <.choice value="000000000d010053">53(圖資館卸貨區)<./choice><br>
        <.choice value="000000000d010071">71(圖資館卸貨區)<./choice><br>
        <.choice value="000000000d0100b4">B4(電資學院卸貨場)<./choice><br>
        <.choice value="000000000d0100b6">B6(電資學院卸貨場)<./choice><br>
        <.choice value="000000000d0100c1">C1(工學院卸貨場)<./choice><br>
        <.choice value="000000000d0100c3">C3(工學院卸貨場)<./choice><br>
        <.choice value="000000000d0100c8">C8(管理學院卸貨場)<./choice><br>
        <.choice value="000000000d0100ce">CE(外語學院卸貨區)<./choice><br>
        <.choice value="000000000d0100d0">D0(外語學院卸貨區)<./choice><br>
        <.default>000000000d0100b4<./default><br>
      <./input><br>
      <.chart><br>
        <title>電量</title><br>
        <.search><br>
          <.query>source="udp:5567" $macAddr04$<br>
            |rex field=data "fc000105\S{2}(?&lt;Voltage_hex&gt;\S{4})" |eval Voltage=round(tonumber(Voltage_hex,16)/1000,3)<br>
            |transaction data maxspan=30s<br>
            |timechart min(Voltage) by macAddr span=1d<./query><br>
          <.earliest>0<./earliest><br>
          <.latest><./latest><br>
          <.refresh>1m<./refresh><br>
          <.refreshType>delay<./refreshType><br>
        <./search><br>
        <.option name="charting.chart">line<./option><br>
      <./chart><br>
    <./panel><br>
  <./row><br>
  <.row><br>
    <.panel><br>
      <title>每小時停車次數</title><br>
      <.input type="radio" token="macAddr01"><br>
        <.label>MAC<./label><br>
        <.choice value="000000000d010053">53(圖資館卸貨區)<./choice><br>
        <.choice value="000000000d010071">71(圖資館卸貨區)<./choice><br>
        <.choice value="000000000d0100b4">B4(電資學院卸貨場)<./choice><br>
        <.choice value="000000000d0100b6">B6(電資學院卸貨場)<./choice><br>
        <.choice value="000000000d0100c1">C1(工學院卸貨場)<./choice><br>
        <.choice value="000000000d0100c3">C3(工學院卸貨場)<./choice><br>
        <.choice value="000000000d0100c8">C8(管理學院卸貨場)<./choice><br>
        <.choice value="000000000d0100ce">CE(外語學院卸貨區)<./choice><br>
        <.choice value="000000000d0100d0">D0(外語學院卸貨區)<./choice><br>
        <.default>000000000d0100b4<./default><br>
      <./input><br>
      <.chart><br>
        <.search><br>
          <.query>source="udp:5567" $macAddr01$<br>
            |rex field=data "fc000105(?&lt;ParkData&gt;\d{2})" <br>
            |eval ParkStatus=case(ParkData=="02","Not yet learn",ParkData=="22","Had Learn",ParkData=="21","Parking",ParkData=="23","Keep Parking",ParkData=="60","No Car Parking",ParkData=="61","Parking",ParkData=="62","Keep Correcttion",ParkData=="63","Had Correcttion") <br>
            |eval ParkStatus2 = if(ParkData=="21", if (secondsAgoStr &gt;3600 ,"more than 1hr","Had parking less 1hr" ) , "No car parking" )<br>
            |eval secondsAgoStr=tonumber(now() - _time)<br>
            |transaction data maxspan=30s<br>
            |where ParkData=21 OR ParkData=23 OR ParkData=61 OR ParkData=63<br>
            |timechart span=1h count(_time) AS "事件發生次數"<./query><br>
          <.earliest>0<./earliest><br>
          <.latest><./latest><br>
          <.refresh>1m<./refresh><br>
          <.refreshType>delay<./refreshType><br>
        <./search><br>
        <.option name="charting.chart">column<./option><br>
      <./chart><br>
    <./panel><br>
    <.panel><br>
      <title>每日停車次數</title><br>
      <.input type="radio" token="macAddr02"><br>
        <.label>MAC<./label><br>
        <.choice value="000000000d010053">53(圖資館卸貨區)<./choice><br>
        <.choice value="000000000d010071">71(圖資館卸貨區)<./choice><br>
        <.choice value="000000000d0100b4">B4(電資學院卸貨場)<./choice><br>
        <.choice value="000000000d0100b6">B6(電資學院卸貨場)<./choice><br>
        <.choice value="000000000d0100c1">C1(工學院卸貨場)<./choice><br>
        <.choice value="000000000d0100c3">C3(工學院卸貨場)<./choice><br>
        <.choice value="000000000d0100c8">C8(管理學院卸貨場)<./choice><br>
        <.choice value="000000000d0100ce">CE(外語學院卸貨區)<./choice><br>
        <.choice value="000000000d0100d0">D0(外語學院卸貨區)<./choice><br>
        <.default>000000000d0100b4<./default><br>
      <./input><br>
      <.chart><br>
        <.search><br>
          <.query>source="udp:5567" $macAddr02$<br>
            |rex field=data "fc000105(?&lt;ParkData&gt;\d{2})" <br>
            |eval ParkStatus=case(ParkData=="02","Not yet learn",ParkData=="22","Had Learn",ParkData=="21","Parking",ParkData=="23","Keep Parking",ParkData=="60","No Car Parking",ParkData=="61","Parking",ParkData=="62","Keep Correcttion",ParkData=="63","Had Correcttion") <br>
            |eval ParkStatus2 = if(ParkData=="21", if (secondsAgoStr &gt;3600 ,"more than 1hr","Had parking less 1hr" ) , "No car parking" )<br>
            |eval secondsAgoStr=tonumber(now() - _time)<br>
            |transaction data maxspan=30s<br>
            |table data Temperature ParkData ParkStatus ParkStatus2 _time secondsAgoStr  rssi snr <br>
            |where ParkData=21  OR ParkData=23 OR ParkData=61 OR ParkData=63<br>
            |timechart span=1d count(_time) AS "停車次數"<./query><br>
          <.earliest>0<./earliest><br>
          <.latest><./latest><br>
          <.refresh>1m<./refresh><br>
          <.refreshType>delay<./refreshType><br>
        <./search><br>
        <.option name="charting.chart">line<./option><br>
      <./chart><br>
    <./panel><br>
  <./row><br>
  <.row><br>
    <.panel><br>
      <title>停車格原始資料</title><br>
      <.input type="dropdown" token="macAddr"><br>
        <.label>MAC<./label><br>
        <.choice value="000000000d010053">53(圖資館卸貨區)<./choice><br>
        <.choice value="000000000d010071">71(圖資館卸貨區)<./choice><br>
        <.choice value="000000000d0100b4">B4(電資學院卸貨場)<./choice><br>
        <.choice value="000000000d0100b6">B6(電資學院卸貨場)<./choice><br>
        <.choice value="000000000d0100c1">C1(工學院卸貨場)<./choice><br>
        <.choice value="000000000d0100c3">C3(工學院卸貨場)<./choice><br>
        <.choice value="000000000d0100c8">C8(管理學院卸貨場)<./choice><br>
        <.choice value="000000000d0100ce">CE(外語學院卸貨區)<./choice><br>
        <.choice value="000000000d0100d0">D0(外語學院卸貨區)<./choice><br>
        <.default>000000000d0100b4<./default><br>
      <./input><br>
      <.table><br>
        <.search><br>
          <.query>source="udp:5567"  $macAddr$<br>
            |rex field=data "fc000105(?&lt;ParkData&gt;\d{2})" <br>
            |rex field=data "fc000105\S{8}(?&lt;TempData&gt;\S{2})" |eval Temperature=tostring(tonumber(TempData,16))+"℃"<br>
            |rex field=data "fc000105\S{2}(?&lt;Voltage_hex&gt;\S{4})" <br>
            |eval Voltage=tostring(round(tonumber(Voltage_hex,16)/1000,3))+"v"<br>
            |rex field=data "fc000105\S{6}(?&lt;Magnetic&gt;\S{2})" |eval Magnetic_disturbance=tonumber(Magnetic,16)<br>
            |eval ParkStatus=case(ParkData=="02","Not yet learn",ParkData=="22","Had Learn",ParkData=="21","Parking",ParkData=="23","Keep Parking",ParkData=="60","No Car Parking",ParkData=="61","Parking",ParkData=="62","Keep Correcttion",ParkData=="63","Had Correcttion")<br>
            |table data ParkData ParkStatus Voltage Magnetic_disturbance Temperature _time rssi snr<./query><br>
          <.earliest>0<./earliest><br>
          <.latest><./latest><br>
          <.refresh>1m<./refresh><br>
          <.refreshType>delay<./refreshType><br>
        <./search><br>
        <.option name="count">15<./option><br>
        <.option name="rowNumbers">true<./option><br>
      <./table><br>
    <./panel><br>
  <./row><br>
<./form><br>
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



