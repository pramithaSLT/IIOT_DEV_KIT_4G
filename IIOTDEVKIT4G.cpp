#ifndef IIOTDEVKIT4G_H
#include "IIOTDEVKIT4G.h"
#endif

bool IIOTDEVKIT4G::Init(unsigned long buad_rate)
{
  // port config and buad rate set 
  pinMode(PWR_PIN, OUTPUT);
  Serial2.begin(buad_rate);
  // PWR on pin signal
  digitalWrite(PWR_PIN, HIGH);
  delay(600);
  digitalWrite(PWR_PIN, LOW);
  delay(2500);
  // disable echo mode.
  if(SENDATCMD("ATE0\r\n", 2000, "OK", "ERROR")!=1){
    return false;
  }
  delay(100);
  return AT_TEST();
}

bool IIOTDEVKIT4G::PWRDOWN()
{
  
  uint8_t answer = SENDATCMD("AT+CPOF\r\n", 2000, "OK", "error");

  if (answer == 1)
  {
    digitalWrite(PWR_PIN, LOW);   
    return true;
  }
  return false;
  
}
bool IIOTDEVKIT4G::CSQ(String *response)
{
  
  uint8_t answer=SEND_AT_CMD_RAW("AT+CSQ\r\n",2000,response);
  return answer;

}


bool IIOTDEVKIT4G::SET_APN(String CID,String PDP_type,String APNNAME)
{
  uint8_t answer = SENDATCMD("AT+CFUN=0\r\n", 2000, "OK", "ERROR");
  if (answer == 1)
  {
    String atCommand = "AT+CGDCONT="+CID+",\""+PDP_type+"\",\""+ APNNAME + "\",\"\",0,0,,,,\r\n";
    //Serial.print(atCommand);
    char charArray[atCommand.length()];
    atCommand.toCharArray(charArray, atCommand.length());
    answer = SENDATCMD(charArray, 4000, "OK", "ERROR");
    if (answer == 1)
    {
      uint8_t answer = SENDATCMD("AT+CFUN=1\r\n", 2000, "OK", "ERROR");
      if (answer == 1)
      {
        return true;
      }
      else
      {
        return false;
      }
    }
    else
    {
      return false;
    }
  }
  else
  {
    // Serial.println("AP  set  Error");
    return false;
  }
}

bool IIOTDEVKIT4G ::IS_ATTACH()
{
  String response;
  uint8_t answer=SEND_AT_CMD_RAW("AT+CREG?\r\n",2000,&response);
  if (answer)
  {
    //Serial.println(response[8]);
    if((response[7]=='0') && (response[9]=='1')) return true;
  }
  return false;
}

bool IIOTDEVKIT4G ::IS_PACKET_DOMAIN_ATTACH()
{
  String response;
  uint8_t answer=SEND_AT_CMD_RAW("AT+CGATT?\r\n",2000,&response);
  if (answer)
  {
    //Serial.println(response);
    if(response[8]=='1') return true;
  }
  return false;
}

bool IIOTDEVKIT4G ::GET_IP()
{
  String response;
  uint8_t answer = SEND_AT_CMD_RAW("AT+CGCONTRDP\r\n",2000,&response);

  //if (response =="ERROR")
  //response = +CGCONTRDP: 1,5,"nbiot","10.106.221.48.255.255.255.0"
  //_NetStat.IP = 
  // uint8_t answer = SENDATCMD("AT+CGCONTRDP?\r\n", 2000, "OK", "ERROR");
  Serial.println(response);
  if (answer == 1)
  {
    return true;
  }
  else 
  {
    return false;
  }
}

bool IIOTDEVKIT4G ::MQTT_SETUP(Broker *broker, String server, String port)
{


  broker->addr = server;
  broker->port = port;

  String response;
  uint8_t answer = SEND_AT_CMD_RAW("AT+CMQTTSTART\r\n", 30000,  &response);
  if (answer){
    if((response[0]=='O') && (response[1]=='K')){
      //Serial.println("success");
      return true;
    }
    else{
      return false;
    }
    
  }
  else{
     return false;
  }
}

bool IIOTDEVKIT4G ::MQTT_STOP()
{
  uint8_t answer = SENDATCMD("AT+CMQTTSTOP\r\n", 30000, "OK", "ERROR");


  if (answer == 1){
    //Serial.println("MQTT STOP");
    return true;
  }
  else{
     return false;
  }
}

bool IIOTDEVKIT4G ::MQTT_DISCONNECT(Broker *broker)
{
  return MQTT_DISCONNECT(broker, 0);
}

bool IIOTDEVKIT4G ::MQTT_DISCONNECT(Broker *broker, uint timeout)
{
  String atCommand1;
  if(timeout==0){
    atCommand1 ="AT+CMQTTDISC="+String(broker->mqttId)+"\r\n";
  }
  else{
    atCommand1 ="AT+CMQTTDISC="+String(broker->mqttId)+","+timeout+"\r\n";
  }
  String response1 = "+CMQTTDISC: "+String(broker->mqttId)+ ",0";
  char charArray1[atCommand1.length()];
  atCommand1.toCharArray(charArray1, atCommand1.length());
  uint8_t answer = SENDATCMD(charArray1, 30000, response1.c_str(), "ERROR");
  
  if (answer == 1){
    Serial.println("CMQTTDISC ok");
    answer = MQTT_RELESECLIENT(broker);
    if(answer==1){
      return true;
    }
    else{
      return false;
    }
  }
  else{
    return false;
  }
}


bool IIOTDEVKIT4G::MQTT_CONNECT(Broker *broker, String clientid, String Username, String password)
{

  broker->username = Username;
  broker->password = password;

  /******Acquire a client******/
  String atCommand = "AT+CMQTTACCQ="+String(broker->mqttId)+",\"" + clientid + "\","+broker->Server_type+"\r\n";
  char charArray[atCommand.length()];
  atCommand.toCharArray(charArray, atCommand.length());
 
  //String response1;
  //bool answer1 = SEND_AT_CMD_RAW(charArray, 60000, &response1);
  uint8_t answer1 = SENDATCMD(charArray, 40000,"OK", "+ERROR");
  //Serial.println(response1);
  if(answer1==1){
    
    /***Connect to MQTT Server***/
    String atCommand2;
    if(Username==NULL){
      //Serial.println("no username");
      atCommand2 = "AT+CMQTTCONNECT="+String(broker->mqttId)+",\"tcp://"+broker->addr+":"+broker->port+"\","+broker->keepalive_time+","+broker->clean_session+"\r\n";
    }
    else if(password==NULL){
      //Serial.println("no password");
      atCommand2 = "AT+CMQTTCONNECT="+String(broker->mqttId)+",\"tcp://"+broker->addr+":"+broker->port+"\","+broker->keepalive_time+","+broker->clean_session+",\""+Username+"\"\r\n";
    }
    else{
      //Serial.println("have both");
      atCommand2 = "AT+CMQTTCONNECT="+String(broker->mqttId)+",\"tcp://"+broker->addr+":"+broker->port+"\","+broker->keepalive_time+","+broker->clean_session+",\""+Username+"\",\""+password+"\"\r\n";
    }
    
    char charArray2[atCommand2.length()];
    atCommand2.toCharArray(charArray2, atCommand2.length());
    

    String ext_response2 = "+CMQTTCONNECT: "+String(broker->mqttId)+",0";
    
    //bool answer2 = SEND_AT_CMD_RAW(charArray2, 60000, &response);
    //Serial.println("AT+CMQTTCONNECT: -");
    //Serial.println(response);

    uint8_t answer2 = SENDATCMD(charArray2, 40000, ext_response2.c_str() , "+ERROR");
    if (answer2==1)
    {
        Serial.println("Acquire a client");
        return true;
    }
    else
    {
      MQTT_RELESECLIENT(broker);
      return false;
    }

  }else{
    Serial.println("Acquire a client Error");
    //MQTT_RELESECLIENT(broker);
    return false;
  }
 
}

bool  IIOTDEVKIT4G::MQTT_RELESECLIENT(Broker *broker){
 
  String atCommandrel ="AT+CMQTTREL="+String(broker->mqttId)+"\r\n";
  char charArrayrel[atCommandrel.length()];
  atCommandrel.toCharArray(charArrayrel, atCommandrel.length());
  uint8_t answer = SENDATCMD(charArrayrel, 20000, "OK", "ERROR");
  if(answer==1){
    return true;
  }
  return false;
}

bool IIOTDEVKIT4G::MQTT_CONNECT(Broker *broker, String clientid){
  return MQTT_CONNECT(broker, clientid, "", "");
}

bool IIOTDEVKIT4G::MQTT_CONNECT(Broker *broker, String clientid, String Username){
  return MQTT_CONNECT(broker, clientid, Username, "");
}

bool IIOTDEVKIT4G::MQTT_SETTOPIC(Broker *broker, String topic)
{
  uint topic_len = topic.length();
 // Serial.print("topic length - ");
 // Serial.println(topic_len);

  if((topic_len<1)||(topic_len>1023)) return false;

  
  String atCommand = "AT+CMQTTTOPIC="+String(broker->mqttId)+","+String(topic_len)+"\r\n";
  char charArray[atCommand.length()];
  atCommand.toCharArray(charArray, atCommand.length());

  while(Serial2.available()){
    Serial2.read();
  }

  Serial2.write(charArray); //send array to module

  bool OK=false;
  uint long previous = millis();
  do{
    if(Serial2.available()){
      char val = Serial2.read();
      if(val == '>'){
          OK = true;
      }
    }
  }while((OK == false) && ((millis()-previous)<3000));
  if(OK==false){
    return false;
  }

  //Serial.println("topic responce c");
  char topicarray[topic.length()];
  topic.toCharArray(topicarray, topic.length()+1);

  String response2;
  bool answer = SEND_AT_CMD_RAW(topicarray, 30000, &response2);

  if(answer && (response2[0]=='O') && (response2[1]=='K')){
    return true;
  }
  else{
    return false;
  }
}

bool IIOTDEVKIT4G::MQTT_PAYLOAD(Broker *broker, String msg){

  uint msg_len = msg.length();
  //Serial.print("msg length - ");
  //Serial.println(msg_len);

  if((msg_len<1)||(msg_len>10240)) return false;

  String atCommand = "AT+CMQTTPAYLOAD="+String(broker->mqttId)+","+String(msg_len)+"\r\n";
  char charArray[atCommand.length()];
  atCommand.toCharArray(charArray, atCommand.length());

  while(Serial2.available()){
    Serial2.read();
  }

  Serial2.write(charArray); //send array to module

  bool OK=false;
  uint long previous = millis();
  do{
    if(Serial2.available()){
      if(Serial2.read() == '>'){
          OK = true;
      }
    }
  }while((OK == false) && ((millis()-previous)<3000));
  if(OK==false){
    return false;
  }
 
  char msgarray[msg_len];
  msg.toCharArray(msgarray, msg_len+1);

  String response2;
  bool answer = SEND_AT_CMD_RAW(msgarray, 30000, &response2);
  
  if(answer && (response2[0]=='O') && (response2[1]=='K')){
    return true;
  }
  else{
    return false;
  }
}

bool IIOTDEVKIT4G::MQTT_PUB(Broker *broker,uint8_t qos, uint pub_timeout, bool retained, bool dup){

  String atCommand = "AT+CMQTTPUB="+String(broker->mqttId)+"," + String(qos) +","+ String(pub_timeout)+","+String(retained)+","+String(dup)+"\r\n";
  char charArray[atCommand.length()];
  atCommand.toCharArray(charArray, atCommand.length());
 
  String response;
  bool answer = SEND_AT_CMD_RAW(charArray, 30000, &response);

  if(answer && (response[0]=='O') && (response[1]=='K')){
    return true;
  }
  else{
    return false;
  }

}

bool IIOTDEVKIT4G::MQTT_PUB(Broker *broker, String topic, String msg)
{
  return MQTT_PUB(broker, topic, msg, 0, 5, 0, 0);
}

bool IIOTDEVKIT4G::MQTT_PUB(Broker *broker, String topic, String msg, uint8_t qos, uint pub_timeout, bool retained, bool dup)
{
  /*InPut the topic of publish message*/
  
  if(MQTT_SETTOPIC(broker, topic)==false){
    Serial.println("topic fail");
    return false;
  }

  /*Input the publish message*/
  if(MQTT_PAYLOAD(broker, msg)==false){
    Serial.println("msg fail");
    return false;
  }

  if(MQTT_PUB(broker,qos,pub_timeout,retained,dup)==false)
  {
    Serial.println("pub fail");
     return false;
  }

  return true;
}

uint8_t IIOTDEVKIT4G::MQTTSUB(Broker *broker, String topic, uint8_t qos)
{
  /* Input validation */
  if (qos < 0 || qos > 2)
    return 0xe0; /* QoS must be 0, 1, 2 */

  String atCommand = "AT+CMQSUB=" + String(broker->mqttId) + ",\"" + topic + "\"," + qos + "\r\n";
  char charArray[atCommand.length()];
  atCommand.toCharArray(charArray, atCommand.length());
  uint8_t answer = SENDATCMD(charArray, 4000, "OK", "ERROR");
  return answer == 1 ? 0x01 : answer;
}

uint8_t IIOTDEVKIT4G::MQTTUNSUB(Broker *broker, String topic)
{
  String atCommand = "AT+CMQUNSUB=" + String(broker->mqttId) + "," + topic + "\r\n";
  char charArray[atCommand.length()];
  atCommand.toCharArray(charArray, atCommand.length());
  uint8_t answer = SENDATCMD(charArray, 4000, "OK", "ERROR");
  return answer == 1 ? 0x01 : answer;
}

bool IIOTDEVKIT4G::SEND_AT_CMD_RAW(char *at_command, unsigned int timeout, String *resp)
{
  uint8_t x = 0;
  char response[100] = {0}; // uint8 responce[100] -
  unsigned long previous;
  bool buffer_start = false;
  bool buffer_end = false;

  while (Serial2.available() > 0)
    Serial2.read(); // Clean the input buffer

  // delay(100);
  //Serial.write(at_command); // for dubug
  Serial2.write(at_command); // Send the AT command

  // delay(00);
  bool IS_CGEV=false;
  do{
    previous = millis();
    x=0;
    IS_CGEV=false;

    do
    {
      if (Serial2.available())
      {
        char tem1 = Serial2.read();
        if (tem1 = '\r')
        {
          char tem2 = Serial2.read();
          if (tem1 = '\n')
          {
            buffer_start = true;
          }
        }
      }
      delay(10);
    } while (!buffer_start && ((millis() - previous) < timeout));
    if (!buffer_start)
    {
      return false;
    }

    do
    {
      if (Serial2.available())
      {
        response[x] = Serial2.read();
        x++;
      }
    
      if (x > 1)
      {
        char tempArray[2] = {0};
        tempArray[0] = response[x-2];
        tempArray[1] = response[x-1];
        if (strstr(tempArray, "\r\n") != NULL)
        {
          buffer_end = true;
        }
      }
    } while (!buffer_end && ((millis() - previous) < timeout));
    if (!buffer_end)
    {
      return false;
    }

    /*+CGEV check*/
    if((response[0]=='+')&&(response[1]=='C')&&(response[2]=='G')&&(response[1]=='E')){
      Serial.println("detect +CGEV CMD");
      IS_CGEV = true;
    }
  }while(IS_CGEV);

  char tempStr[100] = {0};
  uint pointer=0;
  for(uint8_t i = 0; i < x - 2; i++) {
    if(response[i]==NULL || response[i]=='\r' || response[i]=='\n'){
      Serial.println("null, newline detected");
    }else{
      tempStr[pointer]=response[i];
      pointer++;
    }
  }
  *resp = String(tempStr);
  return true;
}


uint8_t IIOTDEVKIT4G::SENDATCMD(const char* at_command, unsigned int timeout, const char *expected_answer1, const char *expected_answer2)
{

  uint8_t x = 0, answer = 0;
  char response[100] = {0};

  unsigned long previous;

  while (Serial2.available() > 0)
    Serial2.read(); // Clean the input buffer

  delay(100);

  //Serial.write(at_command); // for debug
  Serial2.write(at_command); // Send the AT command

  delay(1000);

  previous = millis();
  // this loop waits for the answer
  do
  {
    // if there are data in the UART input buffer, reads it and checks for the asnwer
    if (Serial2.available() != 0)
    {
      response[x] = Serial2.read();
      //Serial.print(response[x]);
      if (expected_answer1 != "")
      {
        // check if the desired answer   is in the response of the module
        if (strstr(response, expected_answer1) != NULL)
        {
          answer = 1;
          delay(100);
        }
        else if (expected_answer2 != "")
        {
          if (strstr(response, expected_answer2) != NULL)
          {
            answer = 2;
            delay(100);
          }
        }
      }
      else
      {
        // Serial.print(response);
      }
      x++;
      delay(10);
    }

  } while ((answer == 0) && ((millis() - previous) < timeout));

  // if (answer == 0) {
  //   Serial.println("AT response Time Out");
  // }
  return answer;
}

bool IIOTDEVKIT4G::AT_TEST()
{
  uint8_t answer = SENDATCMD("AT\r\n", 2000, "OK", "ERROR");
  if (answer == 1)
  {
    return true;
  }
  else
  {
    return false;
  }
}

