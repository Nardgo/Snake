//NAME              ID
//Jake Terpening    21945519
//Whitney Tran      77187709
//Daniel Williams   77402284
//Tristan Bock      50665011

//code modified from chatroom example
#include <stdlib.h>
#include <iostream>
#include <string>
#include <sstream>
#include <time.h>
#include "websocket.h"
#include "gamestate.h"
#include <chrono>

using namespace std::chrono;
	
webSocket server;
gamestate State;
map<int, string> players; //keeps track of which client is which snake
std::string p1Name = "P1", p2Name = "P2";
bool gameStarted = false;
std::string * inputs = new string[2];

int update_delay = 1000;
long long last_update = 0;
long long * offset  = new long long[2];


void init()	// fresh game
{
	State = gamestate();
	inputs[0] = "00";
	inputs[1] = "00";
	offset[0] = 0;
	offset[1] = 0;
	if (players.size() < 2)
	{
		gameStarted = false;
	}

}

/* called when a client connects */
/* tracks clients to players*/
void openHandler(int clientID) {

	std::cout << clientID << " joined." << std::endl;

	if (players.empty())
	{
		players[clientID] = "1";
	}
	else if (players.size() == 1)
	{
		players[clientID] = "2";
	}
	else
	{
		server.wsClose(clientID);
	}
}

/* called when a client disconnects */
void closeHandler(int clientID) {
	if (players[clientID] == "1")
		p1Name = "P1";
	else if (players[clientID] == "2")
		p2Name = "P2";
	std::cout << clientID << " disconnected." << std::endl;
	players.erase(clientID);

	// Player DC, game init
	if (players.size() < 2) {
		init();
	}
}

/* Return a vector of strings split over the delimiter for message*/
vector<string> split(string message, char delimiter) {
	vector<string> result = vector<string>();

	stringstream stream(message);
	string token;

	while (getline(stream, token, delimiter))
	{
		result.push_back(token);
	}

	return result;
}

/* called when a client sends a message to the server */
void messageHandler(int clientID, string message)
{
	vector<string> messageArr = split(message, ':');
}

void moveHandler(int clientID, string direction)
{
	//given direction of client, apply game logic
	std::string playerMove = players[clientID] + direction; // (ex 1a2w means player 1 pressed a and player 2 pressed w)
	if (players[clientID] == "1")
		inputs[0] = playerMove;
	else
		inputs[1] = playerMove;

	ostringstream os; // UPDATE:GameBoard:score1:score2
		os << "UPDATE:" << State.state_str() << ":" << State.playerSco(1) << ":" << State.playerSco(2) << ":" << to_string(offset[clientID]);

}


void moveResults(int clientID, string message)
{
	vector<string> messageArr = split(message, ':');
	if (messageArr[0] == "START") 
		{
			if (players[clientID] == "1")
			{
				if (messageArr[1] != "")
				{
					p1Name = messageArr[1];
				}
			}

			if (players[clientID] == "2")
			{
				if (messageArr[1] != "")
				{
					p2Name = messageArr[1];
				}
			}

			if (players.size() == 2)
			{
				vector<int> clientIDs = server.getClientIDs();
				for (int i = 0; i < clientIDs.size(); i++) // START:col:row:clientSnake:p1Name:p2Name
				{
					server.wsSend(clientIDs[i], "START:" + State.state_str() + ":" + players[i] + ":" + p1Name + ":" + p2Name);
				}
				gameStarted = true;
			}
		}
	

	
	else if (messageArr[0] == "MOVE") // MOVE:direction
	{
		string direction = messageArr[1];

		moveHandler(clientID, direction);

		std::cout << "message sent: " << messageArr[2] << std::endl;

		milliseconds ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
		long long num_ms = ms.count();
		
		std::cout << "message recieved: " << num_ms << std::endl;
		offset[clientID] = num_ms - std::stoll(messageArr[2]);
		std::cout << clientID << "offset: " << offset[clientID] << std::endl;
		if (num_ms < last_update)
		{
			State.revert();
			State.update();
		}
	}
}

/* called once per select() loop */
void periodicHandler()
{
	if (gameStarted) 
	{
		if (State.check_collisions())
		{
			init();
		}
		std::string update_str(inputs[0] + inputs[1]);
		State.set_dir_by_str(update_str);

		int random_delay1 = rand() % 390 + 10;
		int random_delay2 = rand() % 390 + 10;
		random_delay2 = abs(random_delay1 - random_delay2);
		int first_to_recieve = rand() % 2;
		int second_to_recieve = 1 - first_to_recieve;
		State.update();
		milliseconds ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
		last_update = ms.count();

		Sleep(random_delay1);
		server.wsSend(first_to_recieve, "START:" + State.state_str() + ":" + players[first_to_recieve] + ":" + p1Name + ":" + p2Name + ":" + to_string(offset[first_to_recieve]));
		Sleep(random_delay2);
		server.wsSend(second_to_recieve, "START:" + State.state_str() + ":" + players[second_to_recieve] + ":" + p1Name + ":" + p2Name + ":" + to_string(offset[second_to_recieve]));
		Sleep(update_delay);
	}
	
	
}

int main(int argc, char *argv[]) {
	int port;

	cout << "Please set server port: ";
	cin >> port;

	/* set event handler */
	server.setOpenHandler(openHandler);
	server.setCloseHandler(closeHandler);
	server.setMessageHandler(moveResults);
	server.setPeriodicHandler(periodicHandler); // to periodically check state

	/* start the chatroom server, listen to ip '127.0.0.1' and port '8000' */
	server.startServer(port);

	return 1;
}
