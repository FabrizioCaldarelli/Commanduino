#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include "Commander.h"
#include "DigitalReadCommand.h"
#include "DigitalWriteCommand.h"

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = { 0x90, 0xA2, 0xDA, 0x0D, 0x6D, 0xE4 };
byte ip[] = { 192, 168, 10, 91 };
byte dnsIP[] = { 0, 0, 0, 0 };
byte gateway[] = { 0, 0, 0, 0 };
byte subnet[] = { 0, 0, 0, 0 };

unsigned int localPort = 8888;      // local port to listen on

char packetBuffer[110]; //buffer to hold incoming packet,
char packetResponse[110];
char localMacaddressBuff[13];

EthernetUDP udp;
EthernetServer tcp(localPort);
Commander commander;
DigitalWriteCommand digitalWriteCommand;
DigitalReadCommand digitalReadCommand;

void (*fncDispatchActions)(char*, EthernetClient*);

void setup() {

  Serial.begin(9600);
  
  // To parse correctly udp packets
  pinMode(4,OUTPUT);
  digitalWrite(4,HIGH);
   
  Ethernet.begin(mac, ip, dnsIP, gateway, subnet);

  fncDispatchActions = &dispatchActions;

  commander.setMacAddress(mac);
  commander.setTcp(&tcp);
  commander.tcpStartServer();
  commander.setUdp(&udp);
  commander.udpStartServer(localPort);
  
  // Set commander for commands
   digitalWriteCommand.setCommander(&commander);
   digitalReadCommand.setCommander(&commander);
  
}

void dispatchActions(char *pktBuffer, EthernetClient *client)
{
    if( digitalWriteCommand.dispatch(pktBuffer, packetResponse, localMacaddressBuff) == 0) commander.sendResponse(packetResponse, client, localPort);
    if( digitalReadCommand.dispatch(pktBuffer, packetResponse, localMacaddressBuff) == 0) commander.sendResponse(packetResponse, client, localPort);
}


void loop() {
  
  commander.udpDispatch(packetBuffer, 100, fncDispatchActions);
  commander.tcpDispatch(packetBuffer, 100, fncDispatchActions);

  delay(100);
}
