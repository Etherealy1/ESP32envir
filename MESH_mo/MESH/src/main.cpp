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
 /*
#include <WiFi.h>          // Use this for WiFi instead of Ethernet.h
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>
#include <stdio.h>

HardwareSerial mySerial1(1); //软串口，用来与传感器进行通信


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
 String data = ""; 
 char buff[256];// 定义存储传感器数据的数组

 tem=String(dht.readTemperature());

  Serial.print("tem:");
  Serial.println(tem);

hum=String(dht.readHumidity());

  Serial.print("hum:");
  Serial.println(hum);

Serial.print("flag:");
  Serial.println(flag);
  
if(flag==0)
{
name="FIRST";
}
if(flag==1){
  name="SEC";
}
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
}*/

void setup()
{
 
  Serial.begin(4800);
  /*
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
*/
  //..................MESH组网。。。。。。。。。。
  
  mesh.setDebugMsgTypes( ERROR | STARTUP );
 
  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT );
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
 
  userScheduler.addTask( taskSendMessage );
  taskSendMessage.enable();
}


void loop()
{
  /*

  if (conn.connected()){
   
      double *linshi_tem;
    
      double *linshi_temp;
 
      linshi_temp = readAndRecordData();
      *linshi_tem = *(readAndRecordData()+1); 
      Serial.println(*linshi_temp);
      Serial.println(*linshi_tem);   
      delay(5000);
      
  } */
   mesh.update();
}