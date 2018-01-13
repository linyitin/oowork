#include<SoftwareSerial.h>
#define LEDG 11
#define LEDB 10
#define LEDR 9

int raw_data;
String  raw_hex_data[255];
SoftwareSerial loraSerial(2, 3);// RX,TX
unsigned long time;
int status;
int  s0,s4,s5;


char A[24]={0x14,0x02,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x01,0xE8};
//強制校正
char B[4]={0xAA,0x00,0x01,0x01};
//查詢
char C[4]={0xAA,0x00,0x04,0x04};


void setup() {
  Serial.begin(9600);  //Serial Monitor.
  Serial1.begin(9600);  //USB-to-UART converter
  loraSerial.begin(9600); //LoRa Serial
 //燈號
  pinMode(LEDR, OUTPUT);
  pinMode(LEDG, OUTPUT);
  pinMode(LEDB, OUTPUT);
  digitalWrite(LEDR, LOW);
  digitalWrite(LEDG, LOW);
  digitalWrite(LEDB, HIGH);
  //SLPEN input
pinMode(0, OUTPUT);
pinMode(4, INPUT);
pinMode(5, INPUT);
//10秒
  Serial.print("RAW DATA : ");
  Serial.write(&A[0],24); 
  Serial.write(&B[0],4);
} 

void loop() {
//digitalWrite(0, LOW);
s0=digitalRead(0);
s4=digitalRead(4);
s5=digitalRead(5);
delay(10);
//Serial.print("SLPEN");
//Serial.println(0);
//Serial.print("read");
//Serial.println(4);
//Serial.print("stat");
//Serial.println(s5);
   // Serial.write(&B[0],4); 
   // Serial.write(&C[0],4); 
  if (Serial1.available() > 0) {
    Serial.print("RAW DATA : ");
    for (int i = 0 ; i < 12 ; i++) {
      //Get raw data
      raw_data = Serial1.read();
      //Decimal to hex and store the data to array
      printHex(raw_data, 2);
      raw_hex_data[i] = String(returnHex(raw_data, 2));
      Serial.print(" ");
      if (i == 11) {
        //print status
        Serial.println(" ");
        Serial.print("Payload Header幀頭 : ");
        Serial.println(raw_hex_data[0]);
        Serial.print("Payload length 數據長度: ");
        Serial.println(raw_hex_data[1]);
        Serial.print("Signaling coding 信令編碼: ");
        Serial.println(raw_hex_data[2]);
        Serial.print("Device ID : ");
        Serial.println(raw_hex_data[3] + " " + raw_hex_data[4] + " " + raw_hex_data[5] + " " + raw_hex_data[6]);
        Serial.print("Status 狀態: ");
        Serial.println(raw_hex_data[7]);
       
        Serial.print("Voltage 電壓: ");
        Serial.println(raw_hex_data[8]);
        Serial.print("Magnetic strength磁擾強度: ");
        Serial.println(raw_hex_data[9] + " " + raw_hex_data[10]);
        Serial.print("Checksum 效驗碼: ");
        Serial.println(raw_hex_data[11]);
        
        if (raw_hex_data[7] == "00") {
          Serial.print("Is there a Car?: ");
          Serial.println("Haven't car");
          loraSerial.println("AT+DTX=2,00");
          String txtMsg = loraSerial.readString();
          Serial.println(txtMsg);
          digitalWrite(LEDR, LOW);
          digitalWrite(LEDG, HIGH);
          digitalWrite(LEDB, LOW);
        }
        else if (raw_hex_data[7] == "01") {
          Serial.print("Is there a Car?: ");
          Serial.println("Have car");
          loraSerial.println("AT+DTX=2,01");
          String txtMsg = loraSerial.readString();
          Serial.println(txtMsg);
          digitalWrite(LEDR, HIGH);
          digitalWrite(LEDG, LOW);
          digitalWrite(LEDB, LOW);
        }
        else if (raw_hex_data[7] == "02") {
          Serial.println("Test");
          loraSerial.println("AT+DTX=2,02");
          String txtMsg = loraSerial.readString();
          Serial.println(txtMsg);
          digitalWrite(LEDR, LOW);
          digitalWrite(LEDG, LOW);
          digitalWrite(LEDB, HIGH);

        }
        Serial.println("------------------------------------------------------");
      }
    }
  }
}




//Decimal to Hex function
void printHex(int num, int precision) {
  char tmp[16];
  char format[128];
  sprintf(format, "%%.%dX", precision);
  sprintf(tmp, format, num);

  Serial.print(tmp);
}

//Decimal to Hex function
String returnHex(int num, int precision) {
  char tmp[16];
  char format[128];

  sprintf(format, "%%.%dX", precision);
  sprintf(tmp, format, num);

  return tmp;
}


