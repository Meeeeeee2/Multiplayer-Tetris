#include <iostream>
#include <vector>
#include <chrono>
#include <thread>


#include <enet/enet.h>

enum command
{
	Move,
	Rotate,
	Frop,
	ROOM,
	Clear,
	Start
};

struct ConnectedUser
{
	ENetPeer* peer;
	int PlayerID;
	UINT32 host;
	UINT16 port;

	ConnectedUser() {
		peer = nullptr;
		PlayerID = -1;
		host = 0;
		port = 0;
	}
};

const int c_maxGames = 6;


ENetAddress address;
ENetEvent event;
ENetHost* server;

ConnectedUser players[c_maxGames * 2];



struct PackedData
{
	command type;
	int playerSlot;
	int data;
};

void SendString(ENetPeer* peer, std::string data) {
	ENetPacket* packet = enet_packet_create(data.c_str(), sizeof(data) + 1, ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(peer, 0, packet);// where, what channel, packet
}

void SendData(int data,int playerID, command type, ENetPeer* peer, ENetPacketFlag flag) {
	PackedData pData;
	pData.data = data;
	pData.type = type;
	pData.playerSlot = playerID;

	ENetPacket* packet = enet_packet_create(&pData, sizeof(PackedData), flag);
	enet_peer_send(peer, 0, packet);
}

void StartRoom(int room ,long long delay)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(delay));

	std::cout << "starting Room " << room << "\n";
	

	for (int i = 0; i < 2; i++)
	{
		SendData(0,
			room * 2 + i,
			Start,
			players[room * 2 + i].peer,
			ENET_PACKET_FLAG_RELIABLE);
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
			rooms[(int)floor(i / 2)] += 1;
		}
	}
	for (int i = 0; i < c_maxGames; i++)
	{
		if ((rooms[i] > highestPlayer && rooms[i] < 2 )|| (rooms[i] < 2 && currentRoom == -1))
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

		static std::thread t(StartRoom, currentRoom, 1000);
		t.detach();

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

UINT16 GetAvailableGameSlots(ConnectedUser players[c_maxGames * 2])
{
	UINT16 result = 0;
	for (int i = 0; i < c_maxGames * 2; i++) 
	{
		if (players[i].PlayerID != -1)
		{
			result |= 1 << i;
		}
	}
	return result;
}

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
		auto currentTime = clock::now();

		std::chrono::duration<float> delta = currentTime - lastTime;
		float dt = delta.count();

		lastTime = currentTime;
		
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
				printf("Packet of length %u containing %u recieved from %x:%u on channel %u\n ",
					event.packet->dataLength,
					*(int*)event.packet->data,
					event.peer->address.host,
					event.peer->address.port,
					event.channelID);
				break;

			case ENET_EVENT_TYPE_DISCONNECT:
				printf(" % x: % u disconnected \n",
					event.peer->address.host,
					event.peer->address.port);
				if (event.data > 0)
				{
					std::cout << "Disconnecting Player: " << event.data - 1 << "\n";
					players[event.data - 1].PlayerID = -1;
				}
				else if (event.data == 0)
				{
					for (int i = 0; i < c_maxGames * 2; i++) 
					{
						if (players[i].host == event.peer->address.host &&
							players[i].port == event.peer->address.port)
						{
							players[i] = ConnectedUser();
						}
					}
				}
				event.peer->data = NULL;
				break;
			}
		}
		//-------------------
		// Game Logic
		//-------------------






	}
	//GAME LOOP END

	enet_host_destroy(server);


	return EXIT_SUCCESS;
}