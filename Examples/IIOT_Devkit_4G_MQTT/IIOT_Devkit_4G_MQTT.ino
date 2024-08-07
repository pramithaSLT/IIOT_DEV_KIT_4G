#include <IIOTDEVKIT4G.h>

IIOTDEVKIT4G IIOT_Dev_kit;

Broker TB_Broker0;

String Response;
void setup() {
  Serial.begin(115200);
  while(Serial.available()){
    Serial.read();
  }
  while(!Serial.available());

  if(IIOT_Dev_kit.Init(115200)){
    Serial.println("initialized...");  
  }
  else{
    Serial.println("initiallization error"); 
  }

}

void loop() {
  if(Serial.available()){
    char x = Serial.read();
    switch (x){
      case '1':
        if(IIOT_Dev_kit.AT_TEST()){
          Serial.println("AT OK."); 
        }
        else{
          Serial.println("AT Error."); 
        }
        break;
      case '2':
        if(IIOT_Dev_kit.PWRDOWN()){
          Serial.println("POWER DOWN OK...");  
        }
        else{
          Serial.println("POWER DOWN Error."); 
        }
      case '3':
        if(IIOT_Dev_kit.SET_APN("1","IP","MOBITEL")){
          Serial.println("APN OK...");  
        }
        else{
          Serial.println("APN Error."); 
        }
        break;
      case '4':
        
        if(IIOT_Dev_kit.CSQ(&Response)){    // not working need to check 
          Serial.println(Response);  
        }
        else{
          Serial.println("CSQ Error"); 
        }
        break;
      case '5': // chech attched 
        if(IIOT_Dev_kit.IS_ATTACH()){    // not working need to check 
          Serial.println("attched.");  
        }
        else{
          Serial.println("attched Error"); 
        }
        break;
      case '6': // chech packet domain attched 
        if(IIOT_Dev_kit.IS_PACKET_DOMAIN_ATTACH()){    // not working need to check 
          Serial.println("packet domain attched.");  
        }
        else{
          Serial.println("attched Error"); 
        }
        break;
      case 'S':
        if(IIOT_Dev_kit.MQTT_SETUP(&TB_Broker0, "test.mosquitto.org","1883")){
           Serial.println("MQTT Start");  
        }
        else{
          Serial.println("MQTT Start Error"); 
        }
        break;
      case 'C':
        if(IIOT_Dev_kit.MQTT_CONNECT(&TB_Broker0, "iiiiijjjj")){
           Serial.println("Connect to server");  
        }
        else{
          Serial.println("fail to connect"); 
        }
        break;
      
      case 'D':
        if(IIOT_Dev_kit.MQTT_DISCONNECT(&TB_Broker0)){
           Serial.println("disconnected");  
        }
        else{
          Serial.println("fail to disconnect"); 
        }
        break;
      case 'K':
        if(IIOT_Dev_kit.MQTT_STOP()){
           Serial.println("MQTT STOP");  
        }
        else{
          Serial.println("fail to STOP MQTT"); 
        }
        break;

      case 'P':
        if(IIOT_Dev_kit.MQTT_PUB(&TB_Broker0, "IIOT_4g_test", "hello nfnfhe hhh ggg fff")){
           Serial.println("MSG sent");  
        }
        else{
          Serial.println("msg failed"); 
        }
        break;
      
      default:
        break;
    } 
  }
}
