#include "serverClass.hpp"

Server::Server(std::string host, std::string port, std::string password)
	: _host(host), _port(port), _password(password)
{ setSockets(); };

Server::~Server()
{
	// delete users
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
	addrinfo	hints;
	addrinfo	*res;
	addrinfo	*resaux;
	int			socketfd;

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
	while(resaux != NULL)
	{
		socketfd = socket(resaux->ai_family, resaux->ai_socktype, resaux->ai_protocol);
		if (socketfd == -1)
			continue ;
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
	if (fcntl(socketfd, F_SETFL, O_NONBLOCK) == -1)
	 	throw std::runtime_error("error: could not set fcntl flags");
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

void	Server::init()
{
	int cli;
	sockaddr_storage		cli_addr;
	socklen_t				sock_size;
	
	pollfd fd = {_socket, POLLIN, 0};
	_fdvec.push_back(fd);
	std::vector<pollfd>::iterator	it;
	std::cout << "Server: " << _host << ":" << _port << std::endl;
				
	char *buff = new char [512];
	std::string buffaux;
	
	while (true)
	{
		it = _fdvec.begin();
		unsigned long cont = 0;
		if (poll(&(*it), _fdvec.size(), -1) == -1)
			throw std::runtime_error("error: could not poll");
		for	(; cont < _fdvec.size() ; cont++)
		{
			// if: aceita o client
			// else: recebe a mensagem
			if (it->revents == POLLIN && it->fd == _socket)
			{
				cli = accept(_socket, (sockaddr *)&cli_addr, &sock_size);
				if (cli == -1)
					throw std::runtime_error("error: could not accept client");
				if (fcntl(cli, F_SETFL, O_NONBLOCK) == -1)
				 	throw std::runtime_error("error: could not set fcntl flags");
				_users.push_back(new User(cli));
				pollfd cliaux = {cli, POLLIN, 0};
				_fdvec.push_back(cliaux);
				break;
			}
			else if (it->revents == POLLIN)
			{
				memset(buff, '\0', 512);
				size_t nbytes;
				if ((nbytes = recv(it->fd, buff, sizeof(buff), 0)) <= 0)
					throw std::runtime_error("error");
				else
				{
					if (buff[0] == 10)
						buffaux += &buff[1];
					else
						buffaux += buff;
					if (buffaux.find("\r") != std::string::npos ||
						buffaux.find("\n") != std::string::npos)
					{
						Command cmd(buffaux, it->fd, *this);
						buffaux.clear();
					}
				}
				break;
			}
			else
			{
				//std::cout << "POLLOUT";
			}
			it++;
		}
	}
	delete[] buff;
}

User*		Server::getUserBySocket(int socket)
{
	std::list<User *>::iterator it = _users.begin();
	while (it != _users.end())
	{
		if ((*it)->getSocket() == socket)
			return (*it);
	}
	return (nullptr);
}
	
/*int			Server::getSocket()
{
	return(_socket);
}*/
	
std::string		Server::getPassword()
{
	return (_password);
}
