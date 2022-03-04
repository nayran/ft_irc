#ifndef UTILS_HPP
# define UTILS_HPP

#include <string>
#include <iostream>
#include <sys/socket.h>


/*
 *		FT_IRC
 *			Socket: permitem comunicacao entre computadores, usam FD para isso.
 *			        int socket(int domain, int type, int protocol);
 *				- Domain: define a familia de protocolos
 *				- Type: especifica a semantica da comunicacao
 *				- Protocol: especifico para cada tipo de manipulacao de dados
 *				Essas variaveis serao definidas pela resposta do getaddrinfo
 *
 *			Setsockopt: define opcoes do socket
 *					int setsockopt(int sockfd, int level, int optname,
 *							const void *optval, socklen_t optlen);
 *				- Level: camada do protocolo. Para uso de opcoes que sao
 *						 independentes do protocolo, usar SOL_SOCKET
 *				- optname: opcao desejada
 *				- optval: valor da opcao (int?)
 *				- optlen: tamanho de optval
 *
 *			Getaddrinfo: preenche a response (res) com flags, familia, tipo, protocolo,
 *						 tamanho do endereco, endereco, nome do host e proxima struct.
 *					int getaddrinfo(const char *node, const char *service,
 *							const struct addrinfo *hints, struct addrinfo **res);
 *				- Node: host
 *				- Service: port
 *				- Hints: criterios e restricoes para selecionar os sockets da response
 *				- Res: response com todos os sockets compativeis com os outros argumentos
 *
 *
 *		https://www.linuxhowtos.org/C_C++/socket.htm
 *		https://www.man7.org/linux/man-pages/man2/socket.2.html
 *		https://linux.die.net/man/3/getaddrinfo
 */

#endif
