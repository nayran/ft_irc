#ifndef UTILS_HPP
# define UTILS_HPP

#include <string>
#include <iostream>
#include <sys/socket.h>


/*
 *		FT_IRC
 *			Socket: permitem comunicacao entre computadores, usam FD para isso.
 *			         int socket(int domain, int type, int protocol);
 *				- Domain: define a familia de protocolos
 *				- Type: especifica a semantica da comunicacao
 *				- Protocol: especifico para cada tipo de manipulacao de dados
 *				Essas variaveis serao definidas pela resposta do getaddrinfo
 *			Setsockopt: 
 *					int setsockopt(int sockfd, int level, int optname,
 *							const void *optval, socklen_t optlen);
 *			Getaddrinfo:
 *					int getaddrinfo(const char *node, const char *service,
 *							const struct addrinfo *hints, struct addrinfo **res);	
 *
 *
 *		https://www.linuxhowtos.org/C_C++/socket.htm
 *		https://www.man7.org/linux/man-pages/man2/socket.2.html
 */

#endif
