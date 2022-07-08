#include "serverClass.hpp"

Server::Server(std::string host, std::string port, std::string password)
	: _host(host), _port(port), _password(password)
{
	setSockets();
};

Server::~Server()
{
	for (std::list<User *>::iterator it = _users.begin(); it != _users.end(); it++)
		delete *it;
	for (std::list<Channel *>::iterator it = _channels.begin(); it != _channels.end(); it++)
		delete *it;
	_users.clear();
	_channels.clear();
	for (std::vector<pollfd>::iterator it = _fdvec.begin(); it != _fdvec.end(); ++it)
	{
		close((*it).fd);
	}
};

/*
 *		Getaddrinfo: preenche a response (res) com uma lista linkada de addrinfo.
 *					 addrinfo tem as variaveis: flags, familia, tipo, protocolo,
 *					 tamanho do endereco, endereco, nome do host e proxima struct.
 *			int getaddrinfo(const char *node, const char *service,
 *					const struct addrinfo *hints, struct addrinfo **res);
 *			- Node: host
 *			- Service: port
 *			- Hints: criterios e restricoes para selecionar os sockets da response
 *			- Res: response com todos os sockets compativeis com os outros argumentos
 *		https://www.man7.org/linux/man-pages/man3/getaddrinfo.3.html
 *
 *		Socket: permitem comunicacao entre computadores, usam FD para isso.
 *		    int socket(int domain, int type, int protocol);
 *			- Domain: define a familia de protocolos
 *			- Type: especifica a semantica da comunicacao
 *			- Protocol: especifico para cada tipo de manipulacao de dados
 *			Essas variaveis serao definidas pela resposta do getaddrinfo
 *		https://www.linuxhowtos.org/C_C++/socket.htm
 *		https://www.man7.org/linux/man-pages/man2/socket.2.html
 *
 *		Setsockopt: define opcoes do socket
 *			int setsockopt(int sockfd, int level, int optname,
 *					const void *optval, socklen_t optlen);
 *			- Level: camada do protocolo. Para uso de opcoes que sao
 *					 independentes do protocolo, usar SOL_SOCKET
 *			- optname: opcao desejada
 *			- optval: valor da opcao (int?)
 *			- optlen: tamanho de optval
 *
 *		Listen: marca o socket como passivo, deixando apto a receber novas conexoes.
 *				AI_PASSIVE ja faz isso? sim, mas o listen fica no aguardo de uma conexao
 *			int listen(int sockfd, int backlog);
 */

void Server::setSockets()
{
	addrinfo hints;
	addrinfo *res;
	addrinfo *resaux;
	int socketfd;

	// AI_PASSIVE para aceitar conexoes e ser amarravel (bind)
	// AF_INET para IPv4
	// SOCK_STREAM para usar TCP, garantindo entrega dos pacotes
	// aceitar qualquer protocolo da familia definida acima
	// todos os outros hints devem ser 0 ou nulo (memset)
	memset(&hints, 0, sizeof(hints));
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = 0;

	// pegar informacoes do host para criar os sockets
	if (getaddrinfo(_host.c_str(), _port.c_str(), &hints, &res) != 0)
		throw std::runtime_error("error: could not get address info");

	// tentar amarrar(bind) ate ter sucesso. Se o socket falhar
	// fecha-lo e tentar o proximo endereco.
	// O bind tende a falhar quando a opcao de reutilizar o socketfd nao eh setada
	resaux = res;
	int enable = 1;
	while (resaux != NULL)
	{
		socketfd = socket(resaux->ai_family, resaux->ai_socktype, resaux->ai_protocol);
		if (socketfd == -1)
			continue;
		if (setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) == -1)
		{
			close(socketfd);
			freeaddrinfo(res);
			throw std::runtime_error("error: could not set socket options");
		}
		if (bind(socketfd, resaux->ai_addr, resaux->ai_addrlen) == 0)
			break;
		close(socketfd);
		resaux = resaux->ai_next;
	}
	freeaddrinfo(res);
	if (resaux == NULL)
		throw std::runtime_error("error: could not bind");
	if (listen(socketfd, MAX_CONNECTIONS) == -1)
		throw std::runtime_error("error: could not listen");
	_socket = socketfd;
};

/*
 *		Poll: espera um fd da lista passada ficar pronto para I/O
 *			  melhor que o select para manusear os fds
 *			int poll(struct pollfd *fds, nfds_t nfds, int timeout);
 *			timeout = -1 vai fazer com que espere um evento
 *		POLLIN: existe dados para serem lidos, mesmo uso do FD_SET
 *		POLLOUT:
 *
 */

void Server::init()
{
	pollfd fd = {_socket, POLLIN, 0};
	if (fcntl(_socket, F_SETFL, O_NONBLOCK) == -1)
		throw std::runtime_error("error: could not set fcntl flags");
	_fdvec.push_back(fd);
	std::vector<pollfd>::iterator itfd;
	std::cout << "Server: " << _host << ":" << _port << std::endl;
	while (running)
	{
		itfd = _fdvec.begin();
		if (poll(&(*itfd), _fdvec.size(), -1) == -1)
			throw std::runtime_error("error: could not poll");
		this->init2();
	}
}

void Server::init2()
{
	pollfd thisPollFd;

	for (std::vector<pollfd>::iterator it = _fdvec.begin(); it != _fdvec.end(); it++)
	{
		thisPollFd = *it;
		if (thisPollFd.revents == POLLHUP)
		{
			std::cout << "pollhup" << std::endl << std::flush;
			break;
		}
		if ((thisPollFd.revents & POLLIN) == POLLIN)
		{
			if (thisPollFd.fd == _socket)
				this->acceptUser();
			else
				receiveMessage(thisPollFd.fd);
			break;
		}
	}
}

int Server::receiveMessage(int clifd)
{
	char buff = '\0';
	std::string buffaux;
	int a = 0;
	while (buffaux.find("\r\n"))
	{
		int nbytes;
		nbytes = recv(clifd, &buff, 1, 0);
		if (nbytes < 0)
			throw std::runtime_error("RECV ERROR");
		else
		{
			buffaux += buff;
			if (a > 500)
				buffaux = "/QUIT you can't flood this server\r\n";
			if (buffaux.find("\r\n") != std::string::npos)
			{
				Command cmd(buffaux, clifd, *this);
				break;
			}
		}
		a++;
	}
	buffaux.clear();
	return 0;
}

int Server::acceptUser()
{
	int client_d;
	sockaddr_in client_addr;
	socklen_t s_size;

	s_size = sizeof(client_addr);
	client_d = accept(_socket, (sockaddr *)&client_addr, &s_size);
	if (client_d == -1)
		throw std::runtime_error("clientd");
	pollfd newPollFd = {client_d, POLLIN, 0};
	_fdvec.push_back(newPollFd);
	if (fcntl(client_d, F_SETFL, O_NONBLOCK) == -1)
		throw std::runtime_error("fcntl");
	User *newUser = new User(client_d);
	_users.push_back(newUser);
	std::cout << "New client: " << newUser->getSocket() << std::endl;
	return client_d;
}

User *Server::getUserBySocket(int socket)
{
	std::list<User *>::iterator it = _users.begin();
	while (it != _users.end())
	{
		if ((*it)->getSocket() == socket)
			return (*it);
		it++;
	}
	return (NULL);
}

User *Server::getUserByNick(std::string nick)
{
	std::list<User *>::iterator it = _users.begin();
	while (it != _users.end())
	{
		if ((*it)->getNick() == nick)
			return (*it);
		it++;
	}
	return (NULL);
}

int Server::getSocket()
{
	return (_socket);
}

std::string Server::getPassword()
{
	return (_password);
}

std::list<User *> Server::getUsers()
{
	return (_users);
}

void Server::deleteUser(int socket)
{
	for (std::vector<pollfd>::iterator it = _fdvec.begin(); it != _fdvec.end(); ++it)
	{
		if ((*it).fd == socket)
		{
			close(socket);
			_fdvec.erase(it);
			break;
		}
	}
	for (std::list<User *>::iterator it = _users.begin(); it != _users.end(); ++it)
	{
		if ((*it)->getSocket() == socket)
		{
			delete *it;
			_users.erase(it);
			break;
		}
	}
}

Channel *Server::getChannelByName(std::string name)
{
	if (name[0] != '#')
		name = '#' + name;
	std::list<Channel *>::iterator it = _channels.begin();
	while (it != _channels.end())
	{
		if ((*it)->getName() == name)
			return (*it);
		it++;
	}
	return (NULL);
}

std::list<Channel *> Server::getChannels()
{
	return _channels;
}

void Server::addChannel(Channel *channel)
{
	_channels.push_back(channel);
}

void Server::messageAll(std::string message)
{
	if (message.find("\r\n"))
		message += "\r\n";
	for (std::list<User *>::iterator it = _users.begin(); it != _users.end(); ++it)
	{
		if (send((*it)->getSocket(), message.c_str(), strlen(message.c_str()), 0) == -1)
			throw std::runtime_error("Couldn't SEND messageAll");
	}
	// std::cout << "SERVER MSG: " << ack << std::endl;
}

void Server::messageAllBut(std::string message, int socket)
{
	if (message.find("\r\n"))
		message += "\r\n";
	for (std::list<User *>::iterator it = _users.begin(); it != _users.end(); ++it)
	{
		int thisSock = (*it)->getSocket();
		if (thisSock != socket)
			if (send(thisSock, message.c_str(), strlen(message.c_str()), 0) == -1)
				throw std::runtime_error("Couldn't SEND messageAllBut");
	}
}
