#include "mqtt.h"

static char MQTT_ID[]="libezsp_id";

static char PUBLISH_TOPIC []="zb_repport";
#define DEFAULT_KEEP_ALIVE 60
static char BROKER_ADDRESS[]="localhost";
#define MQTT_PORT 1883

#include <iostream>

myMqtt::myMqtt() : 
    host(BROKER_ADDRESS),
    topic(PUBLISH_TOPIC),
    port(MQTT_PORT),
    keepalive(DEFAULT_KEEP_ALIVE),
    mosquittopp(MQTT_ID)
{
    mosqpp::lib_init();        // Mandatory initialization for mosquitto library
    connect_async(host,     // non blocking connection to broker request
                    port,
                    keepalive);
    loop_start();            // Start thread managing connection / publish / subscribe
};

myMqtt::~myMqtt() 
{
    loop_stop();            // Kill the thread
    mosqpp::lib_cleanup();    // Mosquitto library cleanup
}

bool myMqtt::send_message(const  char * _message)
{
    // Send message - depending on QoS, mosquitto lib managed re-submission this the thread
    //
    // * NULL : Message Id (int *) this allow to latter get status of each message
    // * topic : topic to be used
    // * lenght of the message
    // * message
    // * qos (0,1,2)
    // * retain (boolean) - indicates if message is retained on broker or not
    // Should return MOSQ_ERR_SUCCESS
    int ret = publish(NULL,this->topic,strlen(_message),_message,1,false);
    return ( ret == MOSQ_ERR_SUCCESS );
}

void myMqtt::on_disconnect(int rc) {
    std::cout << ">> myMosq - disconnection(" << rc << ")" << std::endl;
}

void myMqtt::on_connect(int rc)
{
    if ( rc == 0 ) {
        std::cout << ">> myMosq - connected with server" << std::endl;
    } else {
        std::cout << ">> myMosq - Impossible to connect with server(" << rc << ")" << std::endl;
    }
}

void myMqtt::on_publish(int mid)
{
    std::cout << ">> myMosq - Message (" << mid << ") succeed to be published " << std::endl;
}