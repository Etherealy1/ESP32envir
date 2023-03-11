#include "painlessMesh.h"
 
#define   MESH_PREFIX     "whateverYouLike"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555
 
painlessMesh  mesh;
 
#include <DHT.h>

#define DHTPIN     33
#define DHTTYPE    DHT11
 
DHT dht(DHTPIN, DHTTYPE);

Scheduler userScheduler;
 
void sendMessage();
 
Task taskSendMessage( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage );
 
void sendMessage() {
  String msg = "Message from node one.";    
  msg = msg + " Temperature: " + String(dht.readTemperature());
  msg = msg + " Humidity: " + String(dht.readHumidity());
  mesh.sendBroadcast( msg );
}


void receivedCallback( uint32_t from, String &msg ) {
  Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
}
 
void newConnectionCallback(uint32_t nodeId) {
    Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
}
 
void changedConnectionCallback() {
  Serial.printf("Changed connections\n");
}
 
void nodeTimeAdjustedCallback(int32_t offset) {
    Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(),offset);
}
 
 //...........................................MYSQL..............................
#include <WiFi.h>          // Use this for WiFi instead of Ethernet.h
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>
#include <stdio.h>

HardwareSerial mySerial1(1); //软串口，用来与传感器进行通信
unsigned char item[8] = {0x02, 0x03, 0x00, 0x00, 0x00, 0x02, 0xC4, 0x38}; 

IPAddress server_addr(192,168,43,97);  //mysql数据库的ip地址e
char user[] = "root";              // 数据库的用户名
char password[] = "123456";        // 数据库的登陆密码

// Sample query
char INSERT_SQL[] = "INSERT INTO esp.esp( tem,hum,name) VALUES ('%s','%s','%s')";
//grtrace.arduino_test( tem, hem)  建立的数据库名称.表名（值，值）

 String tem="";
 String hum =""; 
 String name ="";
 int flag=0;

// WiFi card example
char ssid[] = "EZ";         // ESP32连接的无线名称
char pass[] = "12345678";     // ESP32连接的无线密码

WiFiClient client;            // Use this for WiFi instead EthernetClient
MySQL_Connection conn(&client);
MySQL_Cursor* cursor;
//下面定义了一个函数，用来与传感器通信和发送温湿度的值到数据库
double *readAndRecordData(){
 static double linshi_d[2];
 String tem="";
 String hum_t ="";
 char tem1[5];
 char hem[4]; 
 String data = ""; 
 char buff[128];// 定义存储传感器数据的数组
 String info[11];
 for (int i = 0 ; i < 8; i++) {  // 发送测温命令
   mySerial1.write(item[i]);   // write输出
  }
  delay(100);  // 等待测温数据返回
  data = "";
  while (mySerial1.available()) {//从串口中读取数据
    unsigned char in = (unsigned char)mySerial1.read();  // read读取
    Serial.print(in, HEX);
    Serial.print(',');
    data += in;
    data += ',';
  }
  if (data.length() > 0) { //先输出一下接收到的数据
    Serial.print(data.length());
    Serial.println();
    Serial.println(data);
    int commaPosition = -1;
    // 用字符串数组存储
    for (int i = 0; i < 11; i++) {
    commaPosition = data.indexOf(',');
    if (commaPosition != -1)
    {
      info[i] = data.substring(0, commaPosition);
      data = data.substring(commaPosition + 1, data.length());
    }
    else {
      if (data.length() > 0) {  
        info[i] = data.substring(0, commaPosition);
      }
    }
  }
  }
 tem = dtostrf((info[3].toInt() * 256 + info[4].toInt())/10.0,2,1,tem1);
  Serial.print("tem:");
  Serial.println(tem);
 hum = dtostrf((info[5].toInt() * 256 + info[6].toInt())/10.0,2,1,hem);
  dtostrf((info[5].toInt() * 256 + info[6].toInt())/10.0,2,1,hem);
  Serial.print("hum:");
  Serial.println(hum);


name="TURANG";

  sprintf(buff,INSERT_SQL, tem ,hum,name); 
  Serial.println(buff);
  MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn); // 创建一个Mysql实例
 cur_mem->execute(buff);         // 将采集到的温湿度值插入数据库中
 Serial.println("读取传感器数据，并写入数据库");
 delete cur_mem;        // 删除mysql实例为下次采集作准备
 //这里我在项目的应用中，需要把温湿度的数据提取出来，在其他的地方使用，如果实现数据上传功能，下面三行程序可不需要。
 linshi_d[0] =(tem.toInt()  ) ;
 linshi_d[1] =(hum.toInt()  ) ;

if(flag==0)
{
  flag=1;
}
else
{
  flag=0;
}

 return linshi_d;
}

void setup()
{
 
  Serial.begin(4800);
  
  mySerial1.begin(4800,SERIAL_8N1,35,12);
  while (!Serial); // wait for serial port to connect. Needed for Leonardo only
  
  // Begin WiFi section
  Serial.printf("\nConnecting to %s", ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // print out info about the connection:
  Serial.println("\nConnected to network");
  Serial.print("My IP address is: ");
  Serial.println(WiFi.localIP());

  Serial.print("Connecting to SQL...  ");
  if (conn.connect(server_addr, 3306, user, password))
    Serial.println("OK.");
  else
    Serial.println("FAILED.");
  
  // create MySQL cursor object
  cursor = new MySQL_Cursor(&conn);

  //..................MESH组网。。。。。。。。。。
  /*
  mesh.setDebugMsgTypes( ERROR | STARTUP );
 
  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT );
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
 
  userScheduler.addTask( taskSendMessage );
  taskSendMessage.enable();*/
}


void loop()
{
  

  if (conn.connected()){
   
      double *linshi_tem;
    
      double *linshi_temp;
 
      linshi_temp = readAndRecordData();
      *linshi_tem = *(readAndRecordData()+1); 
      Serial.println(*linshi_temp);
      Serial.println(*linshi_tem);   
      delay(5000);
      
  } 
  // mesh.update();
}