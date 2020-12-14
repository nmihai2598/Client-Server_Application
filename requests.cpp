#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <stdio.h>
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include <cstring> //memset
#include <string>
#include <vector>
#include <cstdlib>
#include "helpers.h"
#include "requests.h"

 
using std::vector;
using std::string;


using namespace std;

char *compute_get_request(char *host, char *url, char *url_params, char * cookie,
                     char *Authorization)
{
    char *message = (char *)calloc(BUFLEN, sizeof(char));
    char line[10000];

    if (url_params != NULL)
    {
        sprintf(line, "GET %s?%s HTTP/1.1", url, url_params);
    }
    else
    {
        sprintf(line, "GET %s HTTP/1.1", url);
    }
    compute_message(message, line);

    sprintf(line, "Host: %s", host) ;
    compute_message(message, line) ;

    if(Authorization != NULL){
        sprintf(line, "Authorization: %s", Authorization);
        compute_message(message, line) ;   
    }
    
    if(cookie != NULL){

        sprintf(line, "Cookie:%s", cookie);
        compute_message(message, line);   
        
    }
    
    
    compute_message(message, "") ;

    return message;
}
char *compute_post_request(char *host, char *url, char *form_data, char * cookie,
                      char *Authorization,char *content_type )
{
    char *message;
    message = (char *)calloc(BUFLEN, sizeof(char));
    char line[10000];
       
    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line) ;
    sprintf(line, "Host: %s", host) ;
    compute_message(message, line) ;

    if(content_type == NULL){
        compute_message(message, "Content-Type: application/x-www-form-urlencoded") ;
    } else {
        sprintf(line,"Content-Type: %s",content_type);
        compute_message(message, line);
    }
    if(Authorization != NULL){
        sprintf(line, "Authorization: %s", Authorization);
        compute_message(message, line);   
    }
    
    if(cookie != NULL){

        sprintf(line, "Cookie:%s",  cookie);
        compute_message(message, line);   
        
    }

    int n =  strlen(form_data);
    sprintf(line, "Content-Length: %d", n);
    compute_message(message, line);
    

    compute_message(message, "");


    compute_message(message, form_data);
    return message;
}

vector<string> dns_lookup(const string &host_name, int ipv)
{
    vector<string> output;
 
    struct addrinfo hints, *res, *p;
    int status, ai_family;
    char ip_address[INET6_ADDRSTRLEN];
    ai_family = ipv==6 ? AF_INET6 : AF_INET; //v4 vs v6?
    ai_family = ipv==0 ? AF_UNSPEC : ai_family; // AF_UNSPEC (any), or chosen
    memset(&hints, 0, sizeof hints);
    hints.ai_family = ai_family; 
    hints.ai_socktype = SOCK_STREAM;
    if ((status = getaddrinfo(host_name.c_str(), NULL, &hints, &res)) != 0) {
        //cerr << "getaddrinfo: "<< gai_strerror(status) << endl;
        return output;
    }
    
    for(p = res;p != NULL; p = p->ai_next) {
        void *addr;
        if (p->ai_family == AF_INET) { // IPv4
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
            addr = &(ipv4->sin_addr);
        } else { // IPv6
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
            addr = &(ipv6->sin6_addr);
        }
 
        // convert the IP to a string
        inet_ntop(p->ai_family, addr, ip_address, sizeof ip_address);
        output.push_back(ip_address);
    }
 
    freeaddrinfo(res); // free the linked list
 
    return output;
}