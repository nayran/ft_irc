#ifndef FT_IRC_HPP
# define FT_IRC_HPP

#include <string>
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <fcntl.h>

#define HOST "127.0.0.1"
#define MAX_CONNECTIONS 1024

/*
 *		FT_IRC
 *			1 - Criar sockets que usem TCP (setSockets)
 *				- cria socket: fd do socket
 *				- bind num addr: escolhe porta
 *				- listen: ouve porta
 *				- fcntl: seta socket para nonblocking (read e write simultaneo)
 *			2 - Fazer loop para deixar o server rodando
 *				- poll: prepara fd, aguarda conexao
 *					- POLLIN & FD=0
 *					- POLLIN
 *					- POLLOUT
 *
 *			- accept: estabelece conexao com client, cria socket
 *					  e aceita connect
 *
 *			Client:
 *				Comando nc (netcat), abre conexoes TCP e as ouve
 *					nc 127.0.0.1 8080
 *		
 *		Server			Client
 *		socket()
 *		bind()
 *		listen()
 *		accept()		socket()
 *		  |-------------connect()	// estabelece conexao
 *		read()----------write()		// requisita dados
 *		write()---------read()		// responde dados
 *		close()			close()
 *
 *
 *		http://chi.cs.uchicago.edu/chirc/irc_examples.html
 *		(1)
 *		https://www.youtube.com/watch?app=desktop&v=dquxuXeZXgo
 *		https://www.youtube.com/watch?app=desktop&v=esXw4bdaZkc
 *		https://www.bogotobogo.com/cplusplus/sockets_server_client.php
 *		https://www.youtube.com/watch?app=desktop&v=eVYsIolL2gE
 *
 *		(2)
 *		https://www.ibm.com/docs/en/i/7.4?topic=designs-using-poll-instead-select
 */

#endif
