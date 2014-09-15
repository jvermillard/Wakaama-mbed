/*
 * Author: Edoardo De Marchi
 * Date: 15/02/14
 * Notes: Checks the Ethernet cable connection
*/

// I think it's the key :)

extern "C" {
    #include "wakaama/liblwm2m.h"
}


#include "mbed.h"
#include "EthernetInterface.h"
#include "UDPSocket.h"

DigitalOut led1(LED1);
DigitalOut led2(LED2);
DigitalOut led3(LED3);
DigitalOut led4(LED4);

//ETHERNET
#define ECHO_SERVER_PORT   2000

//char* ip = "192.168.153.153";               // ip
//char* mask = "255.255.255.0";               // mask
//char* gateway = "192.168.153.100";          // gateway
EthernetInterface eth;

UDPSocket udp;

extern "C" {
    extern lwm2m_object_t * get_object_device();
}

int Init()
{
    led1 = 0;
    led2 = 0;
    led3 = 0;
    led4 = 0;


    // ETHERNET
    // use this for static IP
    //eth.init(ip, mask, gateway);

    // DHCP
    eth.init();


    eth.connect();

    udp.init();
    udp.bind(5683);

    udp.set_blocking(false, 60000);

    printf("IP Address is %s\r\n", eth.getIPAddress());

    return 0;
}

static uint8_t prv_buffer_send(void * sessionH,
                               uint8_t * buffer,
                               size_t length,
                               void * userdata)
{
    Endpoint * connP = (Endpoint*) sessionH;

    if (-1 == udp.sendTo(*connP, (char*)buffer, length))
    {
        return COAP_500_INTERNAL_SERVER_ERROR;
    }
    return COAP_NO_ERROR;
}



int main() 
{

    Init();

    printf("Hello\n");
    Endpoint client;
    char buffer[1024];
    lwm2m_context_t * lwm2mH = NULL;
    lwm2m_object_t * objArray[3];
    lwm2m_security_t security;
    int result;

    objArray[0] = get_object_device();
    if (NULL == objArray[0])
    {
        fprintf(stderr, "Failed to create Device object\r\n");
        return -1;
    }


    lwm2mH = lwm2m_init("myfreakingmbed", 1, objArray, prv_buffer_send, NULL);
    if (NULL == lwm2mH)
    {
        fprintf(stderr, "lwm2m_init() failed\r\n");
        return -1;
    }
    
    Endpoint server;
    server.set_address("54.228.25.31",5683);

    memset(&security, 0, sizeof(lwm2m_security_t));

    result = lwm2m_add_server(lwm2mH, 123, 0, NULL, BINDING_U, (void *)&server, &security);

    if (result != 0)
    {
        fprintf(stderr, "lwm2m_add_server() failed: 0x%X\r\n", result);
        return -1;
    }

    result = lwm2m_register(lwm2mH);
    if (result != 0)
    {
        fprintf(stderr, "lwm2m_register() failed: 0x%X\r\n", result);
        return -1;
    }


    while (true) {
        printf("loop...\n");
        struct timeval tv;
        result = lwm2m_step(lwm2mH, &tv);

        int n = udp.receiveFrom(client, buffer, sizeof(buffer));
        printf("Received packet from: %s of size %d\n", client.get_address(), n);
        if (n>0) {
            lwm2m_handle_packet(lwm2mH, (uint8_t*)buffer, n, (void*) &server);
        }
    }
}
