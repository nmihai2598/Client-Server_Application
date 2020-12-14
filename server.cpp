#include <vector>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <map>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <math.h>
#include "helpers.h"


using namespace std;


class Topic{
public:
    char* name;
    int SF;


    Topic(char* name,int SF);

    ~Topic();

};

class Client{
public:
    char* id;
    int socket;
    int open;
    std::vector<class Topic> t;
    std::vector<char*> m;

    Client(char* id);
    ~Client();

};
Topic::Topic(char* name,int SF){
        this->name =strdup(name);
        this->SF = SF;
    }
Topic::~Topic(){}

Client::Client(char* id){
        this->id = strdup(id);
        this->open = 1;
}
Client::~Client(){}


char* mess(struct sockaddr_in cli_addr_udp,unsigned char* buffer){

char message[1600];
int n=0;

n+=sprintf(message,"%s:%d - %s",
                inet_ntoa(cli_addr_udp.sin_addr), ntohs(cli_addr_udp.sin_port), buffer);

    if(buffer[50] == 3){
        sprintf(message+n," - STRING - %s",buffer+51);
    } else{
        if(buffer[51] == 1){
            if(buffer[50] == 0){
                int aux = 0;
                int p=256*256*256;
                for(int i=52;i<52+4;i++){
                    aux-=(p*buffer[i]);
                    p/=256;
                }
                sprintf(message+n," - INT - %d",aux);
            } else if(buffer[50] == 2){
                double aux = 0;
                int p=256*256*256;
                for(int i=52;i<52+4;i++){
                    aux-=(p*buffer[i]);
                    p/=256;
                }
                aux/=pow(10,buffer[56]);
                aux = (10000*aux)/10000;
                sprintf(message+n," - FLOAT - %f",(10000*aux)/10000);

            }
        } else{
            if(buffer[50] == 0){
                int aux = 0;
                int p=256*256*256;
                for(int i=52;i<52+4;i++){
                    aux+=(p*buffer[i]);
                    p/=256;
                }
                sprintf(message+n," - INT - %d",aux);
            } else if(buffer[50] == 1){

                double aux=0;
                int p=256;
                for(int i=51;i<53;i++){
                    aux+=(p*buffer[i]);
                    p/=256;
                }
                aux/=100;
                sprintf(message+n," - SHORT_REAL - %f",aux);

            } else if(buffer[50] == 2){

                double aux = 0;
                int p=256*256*256;
                for(int i=52;i<52+4;i++){
                    aux+=(p*buffer[i]);
                    p/=256;
                }
                aux/=pow(10,buffer[56]);
                aux = (10000*aux)/10000;
                sprintf(message+n," - FLOAT - %f",(10000*aux)/10000);

            }
        }

    }
return strdup(message);
}

vector<class Client> sends(vector<class Client> clients, char* topic, char* buffer){
int n = clients.size();

   for(vector<class Client>::iterator x=clients.begin(); x!=clients.end(); ++x){
     std::vector<class Topic> t = (*x).t;

        for (std::vector<class Topic>::iterator it=t.begin(); it!=t.end(); ++it){

            if(strcmp((*it).name,topic) == 0){
                if((*x).open == 1){
                    n = send((*x).socket, buffer, strlen(buffer), 0);
                    DIE(n < 0, "send");
                    sleep(0.5);
                } else if((*it).SF == 1){
                    (*x).m.push_back(buffer);
                }
            break;
            }
        }


    }
    return clients;
}


vector<class Client> subscribe(vector<class Client> clients,int socket, int i,char* topic, int SF){

    for(vector<class Client>::iterator x=clients.begin(); x!=clients.end(); ++x){

        if((*x).socket == socket){
            if(i == 1){
                class Topic aux = Topic(topic, SF);
                (*x).t.push_back(aux);

            } else {
                std::vector<class Topic> t = (*x).t;
                for (int i = 0;i < t.size(); i++){
                    if(strncmp(t[i].name,topic, strlen(topic)) == 0){
                            (*x).t.erase((*x).t.begin()+i);
                    }
                }
            }
        }

    }
    return clients;
}
vector<class Client> parse(vector<class Client> clients,int socket,char* buffer){
int i,SF = 2;
char* topic = NULL;
char* aux= strtok (buffer," \n");
int ok = 0;
    while (aux != NULL){


        if(strcmp(aux,"subscribe") == 0){
            i = 1;
            ok = 1;
        } else if(strcmp(aux,"unsubscribe") == 0){
            i = 0;
            ok = 1;
        } else if(ok == 1){
            ok = 2;
            topic = strdup(aux);
        } else {
            SF = atoi(aux);

        }
        aux = strtok (NULL, " \n");

    }
    if((i == 1 && SF != 2) || i == 0){
        return subscribe(clients, socket, i, topic, SF);
    } else{
        return clients;
    }

}
vector<class Client> resend (vector<class Client> clients, int socket){
int n;
    for(vector<class Client>::iterator x=clients.begin(); x!=clients.end(); ++x){
        if((*x).socket == socket){
           for(int i = 0;i < (*x).m.size();i++){
                char *buffer = (*x).m[i];
                n = send(socket, buffer, strlen(buffer), 0);
                sleep(0.5);
                DIE(n < 0, "send");

            }
           (*x).m.clear();
           return clients;
        }

    }
}

void usage(char *file)
{
	fprintf(stderr, "Usage: %s server_port\n", file);
	exit(0);
}

int main(int argc, char *argv[])
{
	int sockettcp, newsocket, portno;
	int socketudp;


	struct sockaddr_in serv_addr, cli_addr_tcp, cli_addr_udp;
    int n, i, ret;

	socklen_t clilen;

	vector<class Client> clients;

	fd_set read_fds;	// multimea de citire folosita in select()
	fd_set tmp_fds;		// multime folosita temporar
	int fdmax;			// valoare maxima fd din multimea read_fds

	if (argc < 2) {
		usage(argv[0]);
	}

	// se goleste multimea de descriptori de citire (read_fds) si multimea temporara (tmp_fds)
	FD_ZERO(&read_fds);
	FD_ZERO(&tmp_fds);
	sockettcp = socket(AF_INET, SOCK_STREAM, 0);
	DIE(sockettcp < 0, "socket");
    portno = atoi(argv[1]);
	DIE(portno == 0, "atoi");
	memset((char *) &serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(portno);
	serv_addr.sin_addr.s_addr = INADDR_ANY;
    ret = bind(sockettcp, (struct sockaddr *) &serv_addr, sizeof(struct sockaddr));
	DIE(ret < 0, "bind");

	ret = listen(sockettcp, MAX_CLIENTS);
	DIE(ret < 0, "listen");

	socketudp = socket(PF_INET, SOCK_DGRAM, 0);
	ret = bind(socketudp, (struct sockaddr *) &serv_addr, sizeof(struct sockaddr));
	DIE(ret < 0, "bind");




	// se adauga noul file descriptor (socketul pe care se asculta conexiuni) in multimea read_fds
	FD_SET(0, &read_fds);
	FD_SET(sockettcp, &read_fds);
	FD_SET(socketudp, &read_fds);

	fdmax = socketudp;

	unsigned int size = sizeof(struct sockaddr_in);

	while (1) {
		tmp_fds = read_fds;

		ret = select(fdmax + 1, &tmp_fds, NULL, NULL, NULL);
		DIE(ret < 0, "select");
		int var = 0;
		for (i = 0; i <= fdmax; i++) {
		unsigned char buffer[BUFLEN];
			if (FD_ISSET(i, &tmp_fds)) {
                if (i == 0){
                    memset(buffer, 0, BUFLEN);
                    fgets((char*)buffer, BUFLEN - 1, stdin);
                    if (strncmp((char*)buffer, "exit", 4) == 0) {
                            close(socketudp);
                            close(sockettcp);
                            return 0;
                    }

				} else if (i == sockettcp) {

					// a venit o cerere de conexiune pe socketul inactiv (cel cu listen),
					// pe care serverul o accepta
					clilen = sizeof(cli_addr_tcp);
					newsocket = accept(sockettcp, (struct sockaddr *) &cli_addr_tcp, &clilen);
					DIE(newsocket  < 0, "accept");
                    // se adauga noul socket intors de accept() la multimea descriptorilor de citire
					FD_SET(newsocket , &read_fds);
					bzero(buffer,sizeof(buffer));
					n = recv(newsocket, buffer, sizeof(buffer), 0);
					DIE(n < 0, "recv");
					int ok = 0;
                    	for(int j=0;j<clients.size();j++){
                            if(strcmp(clients[j].id,(char*)buffer) == 0){
                                ok = 1;
                                if(clients[j].open == 0){
                                    if (newsocket > fdmax) {
                                        fdmax = newsocket;
                                    }
                                    clients[j].socket = newsocket;
                                    clients[j].open = 1;
                                    clients = resend(clients,clients[j].socket);
                                } else {
                                    close(newsocket);
                                    FD_CLR(newsocket, &read_fds);
                                }
                            }
                        }

                    if(ok == 0){
                        if (newsocket > fdmax) {
                            fdmax = newsocket;
                        }
                        class Client aux = Client((char*)(buffer));
                        aux.socket = newsocket;
                        clients.push_back(aux);
                        printf("New client %s connected from %s:%d\n",
                            buffer,inet_ntoa(cli_addr_tcp.sin_addr), ntohs(cli_addr_tcp.sin_port));

                    }





				} else if(i == socketudp){
					recvfrom(socketudp, buffer, BUFLEN, 0, (struct sockaddr *) &cli_addr_udp, &size);
                    char * aux = mess(cli_addr_udp,buffer);
                    char * topic = strdup((char*)buffer);
                    clients = sends(clients, topic, aux);

				} else {
					// s-au primit date pe unul din socketii de client,
					// asa ca serverul trebuie sa le receptioneze
					memset(buffer, 0, BUFLEN);
					n = recv(i, buffer, sizeof(buffer), 0);
					DIE(n < 0, "recv");


					if (n == 0) {
						// conexiunea s-a inchis
						for(int j=0;j<clients.size();j++){
                            if(clients[j].socket == i){
                                clients[j].open = 0;

                                printf("Client %s disconnected.\n", clients[j].id);
                            }

                        }
						close(i);

						// se scoate din multimea de citire socketul inchis
						FD_CLR(i, &read_fds);
					} else {
                        clients = parse(clients,i,(char*)buffer);

					}


				}
			}
		}

	}
    close(socketudp);
	close(sockettcp);

	return 0;
}
