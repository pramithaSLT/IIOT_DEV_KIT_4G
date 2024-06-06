/*
Author - Pramitha Muthukudaarachchi, navindu Madanayaka
Copyrights - Sri Lanka Telecom PLC
*/

#ifndef SriLinkDevBoard_h
#define IIOTDEVKIT4G_H

#include "Arduino.h"

struct Broker{
  String addr;
  String port = "1883";
  uint8_t version;
  uint8_t clientId;
  String username = "0";
  String password = "0";
  uint8_t mqttId=0;   //range : 0-1
  uint keepalive_time = 3000;
  bool clean_session =0;  //
  bool Server_type=0; // 0-> MQTT server with TCP , 1 -> MQTT server with SSL/TLS
};

struct NetStat{
  String IP;
  String APN;
  String oper_long;
  String oper_short;
  String oper_num;
  String AcT;
};

#define PWR_PIN 12
#define RST_PIN 4

class IIOTDEVKIT4G{
    public:

        bool Init(unsigned long buad_rate);
        //void enableNBIOT();
        bool AT_TEST();
        bool PWRDOWN();
        bool SET_APN(String CID,String PDP_type,String APNNAME);

        bool CSQ(String *response);
        //
        bool IS_ATTACH();
        bool IS_PACKET_DOMAIN_ATTACH();
        bool GET_IP();

        //MQTT Functions 
        bool MQTT_SETUP(Broker *broker, String server,String port);
        bool MQTT_STOP();

        bool MQTT_CONNECT(Broker *broker, String clientid);
        bool MQTT_CONNECT(Broker *broker, String clientid, String Username);
        bool MQTT_CONNECT(Broker *broker, String clientid, String Username, String password);

        bool MQTT_DISCONNECT(Broker *broker);
        bool MQTT_DISCONNECT(Broker *broker, uint timeout);



        bool MQTT_SETTOPIC(Broker *broker, String topic);
        bool MQTT_PAYLOAD(Broker *broker, String msg);

        bool MQTT_PUB(Broker *broker,uint8_t qos, uint pub_timeout, bool retained, bool dup);
        bool MQTT_PUB(Broker *broker, String topic, String msg);
        bool MQTT_PUB(Broker *broker, String topic, String msg, uint8_t qos, uint pub_timeout, bool retained, bool dup);
        
        uint8_t MQTTSUB(Broker *broker, String topic, uint8_t qos);
        uint8_t MQTTUNSUB(Broker *broker, String topic);
      


        bool SEND_AT_CMD_RAW(char *at_command, unsigned int timeout, String* response);
        
    private:
        uint8_t SENDATCMD(const char* at_command, unsigned int timeout, const char* expected_answer1, const char* expected_answer2);
        NetStat _NetStat;
        
        //int _pin;    
};
#endif