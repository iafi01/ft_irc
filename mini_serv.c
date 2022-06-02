#include <stdio.h>
#include <sys/socket.h> //For Sockets
#include <stdlib.h>
#include <netinet/in.h> //For the AF_INET (Address Family)
#include <sys/select.h>

#define localhost            2130706433 

typedef struct s_server {
    int server_socket;
    struct sockaddr_in server_address;
    fd_set sockets;
    fd_set write_set;
    fd_set read_set;
    int max_fds;
    int max_id;
} t_server;

typedef struct s_client {
    int fd;
    int id;
    char *message_buffer;
    struct s_client *next;
} t_client;

void fatal_error(void)
{
    write(2, "Fatal error\n", strlen("Fatal error\n"));
    exit(1);
}

void send_to_all_client(t_server *s, t_client *c, char *text, int fd)
{
    while (c != 0)
    {
        if (c->fd != fd && FD_ISSET(c->fd, &s->write_set))
            send(c->fd, text, strlen(text), MSG_DONTWAIT);
        c = c->next;
    }
}

t_client *client_connection(t_server *s, t_client *c)
{
    int new_connection;
    char buffer[100];
    t_client *new;
    t_client *it;
    socklen_t len;

    it = c;
    len = sizeof(s->server_address);
    if ((new_connection = accept(s->server_socket, (struct sockaddr *) &s->server_address, &len) == -1))
        return (c);
    if (new_connection > s->max_fds)
        s->max_fds = new_connection;
    FD_SET(new_connection, &s->sockets);
    if ((new = malloc(sizeof(t_client))) == 0)
        fatal();
    new->fd = new_connection;
    new->id = s->max_id + 1;
    new->message_buffer = 0;
    new->next = 0;
    if (c == 0)
        c = new;
    else
    {
        while (it->next != 0)
            it = it->next;
        it->next = new;
    }
    sprintf(buffer, "server: client %d just arrived\n", new->id);
    send_to_all_client(s, c, buffer, new_connection);
    return (c);
}

void launch_server(t_server s)
{
    t_client *c;
    t_client *it;

    c = 0;
    s.max_id = 0;
    s.max_fds = s.server_socket;
    FD_ZERO(&s.sockets);
    FD_SET(s.server_socket, &s.sockets);
    while (1)
    {
        s.read_set = s.write_set = s.sockets;
        it = c;
        if (select(s.max_fds + 1, &s.read_set, &s.write_set, 0, 0))
            fatal();
        if (FD_ISSET(s.server_socket, &s.read_set))
        {
            c = client_connection(&s, c);
            continue;
        }
        while (it)
        {
            if (FD_ISSET(it->fd, &s.read_set))
                c = client_message(&s, c, it);
            it = it->next;
        }
    }
}

void setup_server(t_server *s, int port)
{
    s->server_address.sin_family = AF_INET;
    s->server_address.sin_addr.s_addr = htonl(localhost);
    s->server_address.sin_port = htons(port);
    if ((s->server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        fatal_error();
    if (bind(s->server_socket, (const struct sockaddr *)&s->server_address, sizeof(s->server_address)) != 0)
        fatal_error();
    if (listen(s->server_socket, 0) != 0)
        fatal_error();
}

int main(int argc, char **argv)
{
    t_server s;

    if (argc != 2)
    {
        write(2, "Wrong number of arguments\n", strlen("Wrong number of arguments\n"));
        exit(1);
    }
    setup_server(&s, atoi(argv[1]));
    launch_server(s);
    return 0;
}