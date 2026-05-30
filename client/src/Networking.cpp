#include "Networking.hpp"
#include "main.hpp"
#include <enet/enet.h>
#include <iostream>
#include <thread>
#include <chrono>

ENetAddress address; // IP address and port of server 
ENetEvent event; // holds all incoming events
ENetPeer* peer; // server connecting to 
ENetHost* client;

void SendData(int data, int playerID, command type) {
	PackedData pData;
	pData.data = data;
	pData.type = type;
	pData.playerID = playerID;

	ENetPacket* packet = enet_packet_create(&pData, sizeof(PackedData), ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(peer, 0, packet);
}

void SendString( std::string data) {
	ENetPacket* packet = enet_packet_create(data.c_str(), sizeof(data) + 1, ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(peer, 0, packet);// where, what channel, packet
}

bool InitEnet()
{
	if (enet_initialize() != 0)
	{
		fprintf(stderr, " ENet Failed to Initialize.\n");
		return EXIT_FAILURE;
	}
	atexit(enet_deinitialize);
	
	 // same type for server and host 
	client = enet_host_create(NULL, 1, 1, 0, 0); // adreess, outgoing connections, channels, incoming bandwidth (0 = unlimited), outgoing bandwidth (0 = unlimited)

	if (client == NULL) {
		fprintf(stderr, "Client creation Failed\n");
		return EXIT_FAILURE;
	}
	
	

	enet_address_set_host(&address, "192.168.1.195"); // IP of whatever connecting to (127.0.0.1 is local) 
	// My Lan IP (192.168.1.195)
	// server IP (138.68.183.224)
	address.port = 7777; // any port (make sure it is unused)

	peer = enet_host_connect(client, &address, 1, 0); // client, address, channels, data to send right away 
	if (peer == NULL) {
		fprintf(stderr, "No available peers for Connection\n");
		return EXIT_FAILURE;
	}
	// client , event pointer, milliseconds to wait, return how many things recieved 
	if (enet_host_service(client, &event, 5000) > 0 &&
		event.type == ENET_EVENT_TYPE_CONNECT)
	{
		puts("Connection to server successful");
	}
	else {
		enet_peer_reset(peer);
		puts("Connection to server failed");
		return EXIT_FAILURE; // returns success because nothing crashed but we cant continue from here 
	}
}

void HandleEnetEvents()
{
	//loops through all incoming data 
	while (enet_host_service(client, &event, 0) > 0)
	{
		switch (event.type)
		{
		case ENET_EVENT_TYPE_RECEIVE:
			PackedData pData;
			memcpy(&pData, event.packet->data, sizeof(PackedData));

			UnPackData(pData);



			break;

			// Incase the server disconnects 
		case ENET_EVENT_TYPE_DISCONNECT:
			printf(" % x: % u disconnected \n",
				event.peer->address.host,
				event.peer->address.port);
			event.peer->data = NULL;
			break;

		}
	}
}

void SendNumber(int data)
{
	ENetPacket* packet = enet_packet_create(&data, sizeof(data), ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(peer, 0, packet);
}

void DisconnectENet(int data)
{
	enet_peer_disconnect(peer, data); // peer, data to send with 
	std::cout << data << "\n";

	while (enet_host_service(client, &event, 5000) > 0)
	{

		switch (event.type)
		{
		case ENET_EVENT_TYPE_RECEIVE:
			enet_packet_destroy(event.packet);
			break;
		case ENET_EVENT_TYPE_DISCONNECT:
			puts("Dissconnect success");
			return;
			break;
		}
	}
}
