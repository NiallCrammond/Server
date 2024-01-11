#include "SFML/Network.hpp"
#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"
#include <thread>
#include <iostream>
#include <Windows.h>
#include "Player.h"



struct foodInfo
{
	
	int positionX;
	int positionY;

};

sf::Packet& operator <<(sf::Packet& packet, const Player& player)
{
	return packet << player.positionX << player.positionY << player.size << player.clientID << player.numOfClients << player.serverTime;
}


sf::Packet& operator >>(sf::Packet& packet, Player& player)
{
	return packet >> player.positionX >> player.positionY >> player.size >> player.clientID >> player.numOfClients >> player.serverTime;
}
int numOfClients;

int main()
{
	float receivedValue = 0;


	sf::IpAddress serverAddress = sf::IpAddress::getPublicAddress();
	sf::TcpListener listener;
	sf::Clock foodSpawnClock;
	sf::Time totalTime;

	std::cout << "Listening for connection in IP :" << serverAddress << std::endl;
	if (listener.listen(3000) != sf::Socket::Done)
	{
		printf("There was an error Listening\n");
	}


	sf::SocketSelector selector;
	selector.add(listener);

	std::vector<sf::TcpSocket*> clients;


	int playerID = 1;

	int x = 0;

	while (true)
	{
		totalTime += foodSpawnClock.getElapsedTime();
	srand(10);
		numOfClients = clients.size();



		if (selector.wait())
		{
			if (clients.size() < 2)
			{



				if (selector.isReady(listener))
				{
					sf::TcpSocket* newClient;
					newClient = new sf::TcpSocket;
					newClient->setBlocking(false);
					if (listener.accept(*newClient) == sf::Socket::Done)
					{
						printf("New Client Connected\n");
						Player player;

						clients.push_back(newClient);
						selector.add(*newClient);
						sf::Packet idPacket;
						idPacket << playerID;
						newClient->send(idPacket);
						playerID++;
					}

				}
			}

			for (int i = 0; i < clients.size();i++)
			{
				
				if (selector.isReady(*clients[i]))
				{

					std::cout << "Entered selector loop" << std::endl;
					sf::Packet receiver;


					sf::Socket::Status status = clients[i]->receive(receiver);

					if (status == sf::Socket::Done)
					{
						Player player;
	
						receiver >> player;
						sf::Packet sendPacket;
						sendPacket << player;

						clients[i]->send(receiver);
						player.numOfClients = clients.size();

						std::cout << "Received The following information for Player: " << player.clientID << std::endl
							<< "PlayerPosition X: " << player.positionX << std::endl
							<< "PlayerPosition Y: " << player.positionY << std::endl
							<< "Player size: " << player.size << std::endl
						<< "Number of Clients: " << player.numOfClients << std::endl;

						for (int j = 0; j < clients.size(); j++)
						{
							if (j != i)
							{


								clients[j]->send(sendPacket);
							}
						}

					
					}

					else if (status == sf::Socket::Disconnected)
					{

						std::cout << "Client " << i << " disconnected";
						selector.remove(*clients[i]);
						delete clients[i];
						clients.erase(clients.begin() + i);

						i--;

					}
					else if (status == sf::Socket::Error)
					{
						printf("Error handling Client data");
					}
				}
			}

		}

		
		

		
	}
}



	


void handleWindow()
{
	sf::RenderWindow window(sf::VideoMode(800, 600), "SFML Window");


	
	while (true)
	{


		while (window.isOpen())
		{
			
			sf::Event event;
			while (window.pollEvent(event))
			{
				if (event.type == sf::Event::Closed)
				{
					window.close();

				}
			}
			window.clear();
			window.display();

		}
	}
}
