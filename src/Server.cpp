#include <iostream>
#include <vector>
#include <map>
#include <poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>

int acceptClient(std::vector<pollfd>& pollfds)
{
	pollfd clientPollfd;
	struct sockaddr client_info;
	unsigned int client_info_size = sizeof(client_info);
	int new_client_fd = accept(serverSocket, &client_info, &client_info_size);
	if (new_client_fd == -1)
	{
		std::cerr << "Error accepting connection" << std::endl;
		return (-1);
	}
	clientPollfd.fd = new_client_fd;
	clientPollfd.events = POLLIN | POLLOUT | POLLERR;
	pollfds.push_back(clientPollfd);
	return new_client_fd;
}
int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

void serverLoop() {
	std::vector<pollfd> pollfds;
	pollfd serverPollfd;
	serverPollfd.fd = serverSocket;
    serverPollfd.events = POLLIN | POLLERR;
    pollfds.push_back(serverPollfd);
	std::cout << "Server started" << std::endl;
	while (true) 
	{
		if (poll(pollfds.data(), pollfds.size(), -1) == -1) 
		{
			std::cerr << "Error polling sockets" << std::endl;
			return;
		}
		std::vector<pollfd>::iterator it = pollfds.begin();
		while (it != pollfds.end())
		{
			if (it->revents & POLLIN)
			{
				if (it->fd == serverSocket)
				{
					sockaddr_in clientAddress;
					socklen_t clientAddressSize = sizeof(clientAddress);
					int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressSize);
					if (clientSocket == -1) {
						std::cerr << "Error accepting connection" << std::endl;
						return -5;
					} else {
						pollfd clientPollfd;
						clientPollfd.fd = clientSocket;
						clientPollfd.events = POLLIN | POLLOUT | POLLERR;
						pollfds.push_back(clientPollfd);
						clients[clientSocket] = clientAddress;
						std::cout << "Client connected" << std::endl;
					}
				} 
				else
				{
					char buffer[1024];
					int bytesRead = recv(it->fd, buffer, sizeof(buffer), 0);
					if (bytesRead <= 0) 
					{
						if (bytesRead == 0) 
							std::cout << "Client disconnected\n";
						else 
							std::cerr << "Error reading from socket" << std::endl;
						close(it->fd);
						it = pollfds.erase(it);
						clients.erase(it->fd);
						if (it == pollfds.end()) 
						{
							break;
						}
					} 
					else 
					{
						std::cout << "Message received: " << buffer;
						std::cout << buffer;
						int ret = handleMessages(buffer, password, nickname, it->fd, welcomeSent);
						if (ret == 1)
						{
							close(it->fd);
							clients.erase(it->fd);
							it = pollfds.erase(it);
						}
					}
				}
			}
			else if (it->events & POLLERR) 
			{
				std::cerr << "Error polling socket" << std::endl;
			}
			it++;
		}
	}
	std::cout << "Server stopped" << std::endl;
}