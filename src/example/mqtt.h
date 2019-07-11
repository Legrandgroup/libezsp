#ifndef SIMPLECLIENT_MQTT_H
#define SIMPLECLIENT_MQTT_H

#include <mosquittopp.h>
#include <cstring>
#include <cstdio>

#define MAX_PAYLOAD 50
#define DEFAULT_KEEP_ALIVE 60

class myMqtt : public mosqpp::mosquittopp
{
public:
    myMqtt();
    ~myMqtt();
    bool send_message(const char * _message);    

private:
    const char     *     host;
    const char    *     id;
    const char    *     topic;
    int                port;
    int                keepalive;

    void on_connect(int rc);
    void on_disconnect(int rc);
    void on_publish(int mid);
};

#endif //SIMPLECLIENT_MQTT_H
