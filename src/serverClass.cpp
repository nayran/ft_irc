#include "serverClass.hpp"

Server::Server(std::string host, std::string port, std::string password)
	: _host(host), _port(port), _password(password)
{
	setSockets();
};

Server::~Server()
{};

/*
 *		Listen: marca o socket como passivo, deixando apto a receber novas conexoes.
 *				AI_PASSIVE ja faz isso?
 *			int listen(int sockfd, int backlog);
 *		Poll: 
 *			int poll(struct pollfd *fds, nfds_t nfds, int timeout);
 *		
 */
void	Server::start()
{
	//if (listen(_socket, MAX_CONNECTIONS) == -1)
	//	throw std::runtime_error("error: could not listen");
	std::cout << "host: " << _host << std::endl;
	std::cout << "port: " << _port << std::endl;
	std::cout << "pass: " << _password << std::endl;
	std::cout << "sock: " << _socket << std::endl;
	// POLLIN: existe dados para serem lidos, mesm uso do FD_SET
	pollfd fd = {_socket, POLLIN, 0};
	_fdvec.push_back(fd);
	std::vector<pollfd>::iterator	it;
	while (true)
	{
		it = _fdvec.begin();
		if (poll(&(*it), _fdvec.size(), -1) == -1)
			throw std::runtime_error("error: could not poll");
		if (!_fdvec.empty())
		{
			std::cout << "greeting" << std::endl;
			break;
		}
	}
}

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
 */

void Server::setSockets()
{
	addrinfo	hints;
	addrinfo	*res;
	addrinfo	*resaux;
	int			socketfd;

    memset(&hints, 0, sizeof(hints));
	// AI_PASSIVE para aceitar conexoes e ser amarravel (bind)
	hints.ai_flags = AI_PASSIVE;
	// AF_INET para IPv4
	hints.ai_family = AF_INET;
	// SOCK_STREAM para usar TCP, garantindo entrega dos pacotes
	hints.ai_socktype = SOCK_STREAM;
	// aceitar qualquer protocolo da familia definida acima
	hints.ai_protocol = 0;
	// todos os outros hints devem ser 0 ou nulo
	hints.ai_addrlen = 0;
	hints.ai_addr = NULL;
	hints.ai_canonname = NULL;
	hints.ai_next = NULL;
	
	// pegar informacoes do host para criar os sockets
	if (getaddrinfo(_host.c_str(), _port.c_str(), &hints, &res) != 0)
		throw std::runtime_error("error: could not get address info");
	
	// tentar amarrar(bind) ate ter sucesso. Se o socket falhar
	// fecha-lo e tentar o proximo endereco
	resaux = res;
	//int enable = 1;
	while(resaux != NULL)
	{
		socketfd = socket(resaux->ai_family, resaux->ai_socktype, resaux->ai_protocol);
		if (socketfd == -1)
			continue ;
		//if (setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &enable, sizeof(enable)) == -1)
			//throw std::runtime_error("error: could not set sock options");
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
