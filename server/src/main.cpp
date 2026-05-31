#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <cstring>
#include <cmath>

#include <enet/enet.h>

enum command
{
	Move,
	Rotate,
	Drop,
	ROOM,
	Clear,
	Start,
	NewPiece,
	End,
	Stick
};

struct ConnectedUser
{
	ENetPeer* peer;
	int PlayerID;
	uint32_t host;
	uint16_t port;

	ConnectedUser() {
		peer = nullptr;
		PlayerID = -1;
		host = 0;
		port = 0;
	}
};

struct Room
{
	bool pendingStart = false;
	bool roomActive = false;
	std::chrono::steady_clock::time_point startTime;
};



const int c_maxGames = 6;


ENetAddress address;
ENetEvent event;
ENetHost* server;

ConnectedUser players[c_maxGames * 2];
Room roomList[c_maxGames];


struct PackedData
{
	command type;
	int playerSlot;
	int data;
};

void HandleIncomingTraffic(PackedData pData);

void SendString(ENetPeer* peer, std::string data) {
	ENetPacket* packet = enet_packet_create(data.c_str(), sizeof(data) + 1, ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(peer, 0, packet);// where, what channel, packet
}

void SendData(int data, int playerID, command type, ENetPeer* peer, ENetPacketFlag flag);

void StartRoom(int room);

void EndRoom(int room);

int GetNextAvailableGameSlot(ConnectedUser players[c_maxGames * 2]);



int main() {

	std::cout << "Starting Server\n";


	if (enet_initialize() != 0) {
		fprintf(stderr, "Enet Failed to Initialize\n");
		return EXIT_FAILURE;
	}
	atexit(enet_deinitialize);

	


	address.host = ENET_HOST_ANY; // anywhere / whereever sever is running / can connect from anywhere 
	address.port = 7777; // must match client

	

	server = enet_host_create(&address, c_maxGames * 2, 1, 0,0);// adreess, outgoing connections, channels, incoming bandwidth (0 = unlimited), outgoing bandwidth (0 = unlimited)
	if (server == NULL)
	{
		fprintf(stderr, "Host creation Failed\n");
		return EXIT_FAILURE;
	}

	
	char boards[c_maxGames][2][200]; // num of games, boards per game, tiles per board  
	

	


	//GAME LOOP START

	using clock = std::chrono::high_resolution_clock;
	auto lastTime = clock::now();

	

	int playerID;

	float dropTimer = 0;

	while (true)
	{
		//-------------------
		// Game Logic
		//-------------------
		auto currentTime = clock::now();

		std::chrono::duration<float> delta = currentTime - lastTime;
		float dt = delta.count();

		lastTime = currentTime;
		
		for (int roomID = 0; roomID < c_maxGames; ++roomID)
		{
			if (roomList[roomID].pendingStart &&
				std::chrono::steady_clock::now() >= roomList[roomID].startTime)
			{
				StartRoom(roomID);
				roomList[roomID].pendingStart = false;
				roomList[roomID].roomActive = true;
			}
		}
		//-------------------
		// Networking Loop
		//-------------------
		while (enet_host_service(server, &event, 0) > 0)
		{
			
			switch (event.type)
			{
			case ENET_EVENT_TYPE_CONNECT:
				printf("New Client Connected from %x:%u.\n",
					event.peer->address.host,
					event.peer->address.port);

				playerID = GetNextAvailableGameSlot(players) + 1;

				if (playerID > 0) {

					players[playerID - 1].host = event.peer->address.host;
					players[playerID - 1].port = event.peer->address.port;
					players[playerID - 1].PlayerID = playerID;
					players[playerID - 1].peer = event.peer;
					SendData(0, playerID, ROOM, event.peer, ENET_PACKET_FLAG_RELIABLE);
				}
				else {
					enet_peer_disconnect(event.peer, 0);
				}
				break;

			case ENET_EVENT_TYPE_RECEIVE:
				PackedData pData;
				memcpy(&pData, event.packet->data, sizeof(PackedData));
				HandleIncomingTraffic(pData);
				break;

			case ENET_EVENT_TYPE_DISCONNECT:
				printf(" % x: % u disconnected \n",
					event.peer->address.host,
					event.peer->address.port);
				int room;
				if (event.data > 0)
				{
					std::cout << "Disconnecting Player: " << event.data - 1 << "\n";
					players[event.data - 1].PlayerID = -1;

					room = (int)((event.data - 1) / 2);
				}
				else if (event.data == 0)
				{
					for (int i = 0; i < c_maxGames * 2; i++) 
					{
						if (players[i].host == event.peer->address.host &&
							players[i].port == event.peer->address.port)
						{
							players[i] = ConnectedUser();

							room = (int)(i  / 2);													
						}
					}
				}
				if (roomList[room].roomActive)
					EndRoom(room);
				roomList[room].pendingStart = false;
				event.peer->data = NULL;
				break;
			}
		}
	}
	//GAME LOOP END

	enet_host_destroy(server);


	return EXIT_SUCCESS;
}


void SendData(int data, int playerID, command type, ENetPeer* peer, ENetPacketFlag flag) {
	PackedData pData;
	pData.data = data;
	pData.type = type;
	pData.playerSlot = playerID;

	ENetPacket* packet = enet_packet_create(&pData, sizeof(PackedData), flag);
	enet_peer_send(peer, 0, packet);
}

void HandleIncomingTraffic(PackedData pData)
{
	std::cout << pData.playerSlot << "\n";
	int room = (int)((pData.playerSlot - 1) / 2);
	if (roomList[room].roomActive) 
	{
		int slotA = room * 2;
		int slotB = room * 2 + 1;
		int targetSlot;
		if ((pData.playerSlot - 1) == slotA)
		{
			targetSlot = slotB;
		}
		else
		{
			targetSlot = slotA;
		}
		if (pData.type == End) {
			roomList[room].roomActive = false;
		}
		if (players[targetSlot].peer != nullptr)
			SendData(pData.data, targetSlot + 1, pData.type, players[targetSlot].peer, ENET_PACKET_FLAG_RELIABLE);
		
		

		
	}
}


void StartRoom(int room)
{
	std::cout << "starting Room " << room << "\n";

	int seed = static_cast<int>(time(nullptr));
	for (int i = 0; i < 2; i++)
	{
		SendData(seed,
			room * 2 + i,
			Start,
			players[room * 2 + i].peer,
			ENET_PACKET_FLAG_RELIABLE);
	}
}

void EndRoom(int room)
{
	roomList[room].roomActive = false;
	int slotA = room * 2;
	int slotB = room * 2 + 1;
	for (int i = 0; i < 2; ++i)
	{
		int slot = room * 2 + i;
		if (players[slot].peer != nullptr)		
			SendData(0, slot + 1, End, players[slot].peer, ENET_PACKET_FLAG_RELIABLE);
		
	}
	
}

int GetNextAvailableGameSlot(ConnectedUser players[c_maxGames * 2])
{
	int rooms[c_maxGames];
	for (int i = 0; i < c_maxGames; i++) rooms[i] = 0;

	int highestPlayer = 0;
	int currentRoom = -1;

	for (int i = 0; i < c_maxGames * 2; i++)
	{
		if (players[i].PlayerID != -1)
		{
			rooms[(int)(i / 2)] += 1;
		}
	}
	for (int i = 0; i < c_maxGames; i++)
	{
		if ((rooms[i] > highestPlayer && rooms[i] < 2) || (rooms[i] < 2 && currentRoom == -1))
		{
			currentRoom = i;
			highestPlayer = rooms[i];
		}

	}
	if (currentRoom == -1) return currentRoom;
	if (rooms[currentRoom] == 1)
	{
		int slotA = currentRoom * 2;
		int slotB = slotA + 1;


		roomList[currentRoom].pendingStart = true;
		roomList[currentRoom].startTime = std::chrono::steady_clock::now() + std::chrono::seconds(1);

		if (players[slotA].PlayerID == -1)
		{
			return slotA;
		}
		else
			if (players[slotB].PlayerID == -1)
			{
				return slotB;
			}


	}
	else {
		currentRoom *= 2;
	}
	return currentRoom;
}