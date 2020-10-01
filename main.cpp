#include "mbed.h"
#include "MQTTClient.h"
#include "MQTTEthernet.h"
#include "rtos.h"
#include "k64f.h"

// connect options for MQTT broker
#define BROKER "broker.mqttdashboard.com"   // MQTT broker URL
#define PORT 1883                           // MQTT broker port number
#define CLIENTID ""                         // use K64F MAC address without colons
#define USERNAME ""                         // not required for MQTT Dashboard public broker 
#define PASSWORD ""                         // not required for MQTT Dashboard public broker
#define TOPIC ""                            // MQTT topic

Queue<uint32_t, 6> messageQ;

// LED color control function
void controlLED(color_t led_color) {
    switch(led_color) {
        case red :
            greenLED = blueLED = 1;          
            redLED = 0.7;
            break;
        case green :
            redLED = blueLED = 1;
            greenLED = 0.7;
            break;
        case blue :
            redLED = greenLED = 1;
            blueLED = 0.7;
            break;
        case off :
            redLED = greenLED = blueLED = 1;
            break;
    }
}
    
// Switch 2 interrupt handler
void sw2_ISR(void) {
    messageQ.put((uint32_t*)22);
}

// Switch3 interrupt handler
void sw3_ISR(void) {
    messageQ.put((uint32_t*)33);
}
 
// MQTT message arrived callback function
void messageArrived(MQTT::MessageData& md) {
    MQTT::Message &message = md.message;
    pc.printf("Receiving MQTT message:  %.*s\r\n", message.payloadlen, (char*)message.payload);
    
    if (message.payloadlen == 3) {
        if (strncmp((char*)message.payload, "red", 3) == 0)
            controlLED(red);
        
        else if(strncmp((char*)message.payload, "grn", 3) == 0)
            controlLED(green);
        
        else if(strncmp((char*)message.payload, "blu", 3) == 0)
            controlLED(blue);
        
        else if(strncmp((char*)message.payload, "off", 3) == 0)
            controlLED(off);
    }        
}

int main() {
    // turn off LED  
    controlLED(off);
    
    // set SW2 and SW3 to generate interrupt on falling edge 
    switch2.fall(&sw2_ISR);
    switch3.fall(&sw3_ISR);
    
    pc.printf("\r\n\r\nWelcome to the K64F MQTT Demo!\r\n");
    pc.printf("\r\nAttempting connect to local network...\r\n");
        
    // initialize ethernet interface
    MQTTEthernet ipstack = MQTTEthernet();
    
    // get and display client network info
    EthernetInterface& eth = ipstack.getEth();
    pc.printf("IP address is %s\r\n", eth.getIPAddress());
    pc.printf("MAC address is %s\r\n", eth.getMACAddress());
    pc.printf("Gateway address is %s\r\n", eth.getGateway());
    
    // construct the MQTT client
    MQTT::Client<MQTTEthernet, Countdown> client = MQTT::Client<MQTTEthernet, Countdown>(ipstack);
    
    char* hostname = BROKER;
    int port = PORT;
    int rc;
    
    pc.printf("\r\nAttempting TCP connect to %s:%d:  ", hostname, port);
    
    // connect to TCP socket and check return code
    if ((rc = ipstack.connect(hostname, port)) != 0)
        pc.printf("failed: rc= %d\r\n", rc);
        
    else
        pc.printf("success\r\n");
    
    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;       
    data.MQTTVersion = 3;
    data.clientID.cstring = CLIENTID;
//    data.username.cstring = USERNAME;
//    data.password.cstring = PASSWORD;
    
    // send MQTT connect packet and check return code
    pc.printf("Attempting MQTT connect to %s:%d: ", hostname, port);
    if ((rc = client.connect(data)) != 0)
        pc.printf("failed: rc= %d\r\n", rc);
        
    else
        pc.printf("success\r\n");
        
    char* topic = TOPIC;
    
    // subscribe to MQTT topic
    pc.printf("Subscribing to MQTT topic %s: ", topic);
    if ((rc = client.subscribe(topic, MQTT::QOS0, messageArrived)) != 0)
        pc.printf("failed: rc= %d\r\n", rc);
        
    else
        pc.printf("success\r\n");
        
    MQTT::Message message;
    char buf[100];
    message.qos = MQTT::QOS0;
    message.retained = false;
    message.dup = false;
    message.payload = (void*)buf;
    message.payloadlen = strlen(buf)+1;
    
    while(true) {
        osEvent switchEvent = messageQ.get(100);
        
        if (switchEvent.value.v == 22 || switchEvent.value.v == 33) {
            switch(switchEvent.value.v) {
                case 22 :
                    sprintf(buf, "sw2");
                    break;
                case 33 :
                    sprintf(buf, "sw3");
                    break;
            }
            pc.printf("Publishing MQTT message: %.*s\r\n", message.payloadlen, (char*)message.payload);
            rc = client.publish(topic, message);
            client.yield(100);
        }
        
        else {
            client.yield(100);
        }           
    }
}
