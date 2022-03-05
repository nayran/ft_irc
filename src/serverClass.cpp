#include "serverClass.hpp"

Server::Server(std::string host, std::string port, std::string password)
	: _host(host), _port(port), _password(password)
{
	std::cout << "host: " << _host << std::endl;
	std::cout << "server port: " << _port << std::endl;
	std::cout << "password: " << _password << std::endl;
	std::cout << "socket: " << _socket << std::endl;
	setSockets();
	std::cout << "socket: " << _socket << std::endl;

};

Server::~Server()
{};


/* 
 *		https://www.man7.org/linux/man-pages/man3/getaddrinfo.3.html
 *		Getaddrinfo: preenche a response (res) com uma lista linkada de addrinfo.
 *					 addrinfo tem as variaveis: flags, familia, tipo, protocolo,
 *					 tamanho do endereco, endereco, nome do host e proxima struct.
 *			int getaddrinfo(const char *node, const char *service,
 *					const struct addrinfo *hints, struct addrinfo **res);
 *			- Node: host
 *			- Service: port
 *			- Hints: criterios e restricoes para selecionar os sockets da response
 *			- Res: response com todos os sockets compativeis com os outros argumentos
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
	while(resaux != NULL)
	{
		socketfd = socket(resaux->ai_family, resaux->ai_socktype, resaux->ai_protocol);
		if (socketfd == -1)
			continue ;
		if (bind(socketfd, resaux->ai_addr, resaux->ai_addrlen) == 0)
            break;
        close(socketfd);
		resaux = resaux->ai_next;
	}
	freeaddrinfo(res);
	if (resaux == NULL)
		throw std::runtime_error("error: could not bind");
	_socket = socketfd;
};
