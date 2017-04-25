//
//  main.cpp
//  LoRa_Gateway
//
//  Created by 耿楷寗 on 3/12/17.
//  Copyright © 2017 Keng. All rights reserved.
//

#include <iostream>
#include <thread>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string>
#include "arduPiLoRa.h"
#include "Lora_Setup.h"
using namespace std;

const int PORT=5000;
char *IP="10.1.1.2";
int sockfd;
int e,NodeAddress=0;//用來表示LoRaGateway的偵錯碼,LoRaGateway的Address
bool is_Send=false;
char buffer[256];
string recvMessage() {
	// Receive message
	char lastPacketMessage[256];
	string msg;
	cout<<"Wait for sent..."<<endl;
	e = sx1272.receivePacketTimeoutACK(10000);
	if (e == 0)
	{
		printf("Receive packet, state %d\n", e);
		for (unsigned int i = 0; i < sx1272.packet_received.length; i++)
		{
			lastPacketMessage[i] = (char)sx1272.packet_received.data[i];
		}
		msg.assign(lastPacketMessage);
		return msg;
	}
	else {
			printf("TimeOut, state %d\n", e);
	}
}
void send(string s,int e){
	char send_str[256];
	sprintf(send_str,"%s",s.c_str());
	e = sx1272.sendPacketTimeoutACK(0,send_str);
	printf("Packet sent, state %d\n",e);
	
}
void LoRaListen(){
	
	string msg;
	int res;
	while(true){
		if(is_Send){
			//Send LoRa
			printf("Message from Server: %s\n",buffer);
			send(buffer,e);
			is_Send=false;
		}else{
			msg=recvMessage();
			if(msg!=NULL){
				cout<<"Message from LoRa : "<<msg<<endl;
				res=write(sockfd,&msg,strlen(msg.c_str()));
			}
		}
	}
}
void error(char *msg)
{
	perror(msg);
	exit(0);
}

void TcpListen(){
	int res;
	
	struct timeval timeout={1,0};
	setsockopt(sockfd,SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
	sleep(1);
	cout<<"Tcp Start Listening"<<endl;
	while(true){
		bzero(buffer,256);
		res = read(sockfd,buffer,255);
		if (res < 0) error("ERROR reading from socket");
		is_Send=true;
		sleep(10);
	}
}
void TcpSetUp(){
	
	struct sockaddr_in server_addr;
	struct hostent *server;
	//create socket
	sockfd=socket(AF_INET,SOCK_STREAM,0);
	//check whether socket create sucessfully
	if(sockfd<0){
		cout<<"socket builds fail !!"<<endl;
	}
	//basic setup for server info
	server=gethostbyname(IP);
	bzero((char *)&server_addr, sizeof(server_addr));
	bcopy((char *)server->h_addr, (char *)&server_addr.sin_addr.s_addr, server->h_length);
	server_addr.sin_family=AF_INET;
	server_addr.sin_port=htons(PORT);
	//start connect
	if(connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr))<0){
		cout<<"connection fails"<<endl;
	}
	cout<<"Tcp Connection set up Successfully !!"<<endl;
}
void GatewaySetup(){
	cout<<"Initial setup start"<<endl;
	TcpSetUp();
	//Set up socket connection
	//TcpSetUp();
	//setup LoRa module
	cout<<"Please input your NodeAddress"<<endl;
	cin>>NodeAddress;
	e=setup(NodeAddress);
}
int main(int argc, const char * argv[]) {
	GatewaySetup();
	//create listener
	std::thread TcpListener(TcpListen);
	std::thread LoRaListener(LoRaListen);
	//cout<<TcpListener.get_id()<<" ";
	//cout<<LoRaGateway.get_id()<<" ";
	TcpListener.join();
	LoRaListener.join();
	return 0;
}
