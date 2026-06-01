#pragma once 
#include <string>




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
	Stick,
	OpponentConnect,
	Death
};
struct PackedData
{
	command type;
	int playerID;
	int data;
};

bool InitEnet();

void HandleEnetEvents();

void SendData(int data, int playerID, command type);

void SendString( std::string data);

void SendNumber(int data);

void DisconnectENet(int data);