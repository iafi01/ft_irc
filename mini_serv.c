#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>

#define ERR_ARG "Wrong number of arguments\n"
#define ERR_FAT "Fatal error\n"

typedef struct s_client
{
	int fd;
	int id;
	int new_msg;
	struct s_client *next;
}				t_client;

typedef struct s_server
{
	struct sockaddr_in serveraddr;
	uint16_t port;
	int sockfd;
	int max_id;
	fd_set curr_fds, read_fds, write_fds;
	char server_buffer[64];
	t_client *client;
} t_server;


void fatal()
{
	write(2, ERR_FAT, strlen(ERR_FAT));
	exit(1);
}

int get_max_fd(t_server *s)
{
	int max = s->sockfd;
	for (t_client *ite = s->client; ite; ite = ite->next)
	{
		max = max > ite->fd ? max : ite->fd;
	}
	return(max);
}

int new_client(int fd, t_server *s)
{
	t_client *to_add;
	t_client *tmp = s->client;

	to_add = calloc(sizeof(t_client), 1);
	to_add->fd = fd;
	to_add->id = s->max_id++;
	to_add->next = NULL;
	to_add->new_msg = 1;

	if(!tmp)
	{
		s->client = to_add;
		return s->client->id;
	}
	while(tmp)
	{
		if(!tmp->next)
		{
			tmp->next = to_add;
			return to_add->id;
		}
		tmp = tmp->next;
	}
	return -1;
}

void send_all(char *msg, int len, int fd, t_server *s)
{
	t_client *tmp = s->client;
	while(tmp)
	{
		if(tmp->fd != fd && FD_ISSET(tmp->fd, &s->write_fds))
		{
			if(send(tmp->fd, msg, len, 0) < 0)
				fatal();
		}
		tmp = tmp->next;
	}
}

void accept_client(t_server *s)
{
	int new_fd;
	if((new_fd = accept(s->sockfd, NULL, NULL)) < 0)
		fatal();
	sprintf(s->server_buffer, "server: client %d just arrived\n", new_client(new_fd, s));
	send_all(s->server_buffer, strlen(s->server_buffer), new_fd, s);
	FD_SET(new_fd, &s->curr_fds);
}

t_client *get_client(int fd, t_server *s)
{
	t_client *tmp = s->client;
	while(tmp)
	{
		if(fd == tmp->fd)
			return(tmp);
		tmp = tmp->next;
	}
	return(NULL);
}

int del_client(int fd, t_server *s)
{
	t_client *tmp = s->client;
	t_client *tmp2;
	int id_to_remove = get_client(fd, s)->id;

	if(tmp->id == id_to_remove)
	{
		tmp2 = s->client;
		s->client = s->client->next;
		free (tmp2);
		return(id_to_remove);
	}
	while(tmp)
	{
		if(tmp->next->fd == fd)
		{
			tmp2 = tmp->next->next;
			free(tmp->next);
			tmp->next = tmp2;
			return(id_to_remove);
		}
		tmp = tmp->next;
	}
	return (-1);
}

void start_server(t_server *s)
{
	FD_ZERO(&s->curr_fds);
	FD_SET(s->sockfd, &s->curr_fds);
	while(1)
	{
		s->write_fds = s->read_fds = s->curr_fds;
		if(select(get_max_fd(s) + 1, &s->read_fds, &s->write_fds, NULL, NULL) < 0)
			continue;
		for(int fd = 0; fd <= get_max_fd(s); fd++)
		{
			if(FD_ISSET(fd, &s->read_fds))
			{
				if(fd == s->sockfd)
				{
					accept_client(s);
					break;
				}
				else
				{
					char c;
					if(recv(fd, &c, 1, 0) <= 0)
					{
						sprintf(s->server_buffer, "server: client %d just left\n", del_client(fd, s));
						send_all(s->server_buffer, strlen(s->server_buffer), fd, s);
						FD_CLR(fd, &s->curr_fds);
						close(fd);
						break;
					}
					else
					{
						t_client *client = get_client(fd, s);
						if(client->new_msg)
						{
							sprintf(s->server_buffer, "client %d: ", client->id);
							send_all(s->server_buffer, strlen(s->server_buffer), fd, s);
						}
						client->new_msg = (c == '\n');
						send_all(&c, 1, fd, s);
					}
				}
			}
		}
	}
}

void setup_server(t_server *s, struct sockaddr_in serveraddr, int port)
{
	bzero(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(2130706433);
	serveraddr.sin_port = htons(port);

	s->sockfd = -1;
	if((s->sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		fatal();
	if(bind(s->sockfd, (const struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0)
		fatal();
	if(listen(s->sockfd, 0) < 0)
		fatal();
}

int main(int ac, char **av)
{
	t_server s;

	s.client = NULL;
	if(ac != 2)
	{
		write(2, ERR_ARG, strlen(ERR_ARG));
		exit(2);
	}

	s.port = atoi(av[1]);
	setup_server(&s, s.serveraddr, s.port);
	start_server(&s);
	
	return (0);
}