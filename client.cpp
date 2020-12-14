#include <bits/stdc++.h>
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>
#include "json.hpp"
#include "helpers.h"
#include "requests.h"
 
 
using std::vector;
using std::string;

using json = nlohmann::json;

using namespace std;

void getcookie(char *response,char* cookie, char *js){

int ok = 0;
int nr = 0;
char *tip = strtok(response , "\r\n");    
   // work
	
   while(tip != NULL){

        if(ok == 0 && strncmp(tip,"Set-Cookie",5) == 0){
            
            
            nr = sprintf(cookie,"%s",tip + 11);
            ok++;
        } else if(strncmp(tip,"{",1) == 0){
            
            sprintf(js ,"%s" ,tip);
        	return;
        } else if(ok > 0 && strncmp(tip,"Set-Cookie",5) == 0){
        	
            nr += sprintf(cookie + nr,"; %s",tip + 11);
            
        } 
        tip = strtok(NULL,"\r\n");
   }


}

void getip(char *response,char *ip, char *url){
    int n = strlen(response);
    int i;
    char u[100];
    for(i = 0;i < n;i++){
        if(response[i] == '/'){
            break;
        } else {
            u[i] = response[i]; 
        }
    }
    sprintf(url,"%s",response+i);
    
    vector<string> domains;
 	domains = dns_lookup(u, 4);
 	sprintf(ip, "%s",(char*)(domains[0].c_str()));
}

int main(int argc, char *argv[])
{
    char *message;
    char response[5000];
    char cookie[2000];
    char js[4000];
    
    int sockfd;

    int port = 8081;
    char host[100];
    sprintf(host, "185.118.200.35");

    /*---------TASK1-------------*/
    char url[100];
    sprintf(url,"/task1/start");
    message = compute_get_request(host, url, NULL, NULL, NULL);

    sockfd = open_connection(host, port, AF_INET, SOCK_STREAM, 0);
    send_to_server(sockfd, message);
    memset(response, 0 ,sizeof(response));
    receive_from_server(sockfd, response);
    printf("%s", message);
    printf("%s\n\n", response);
    close_connection(sockfd);
    
    
    /*---------TASK2-------------*/


    free(message);
    getcookie(response, cookie, js);

    json task2 = json::parse(js);
    std::string u = task2["url"];
    sprintf(url,"%s",&u[0]);
    std::string username = (task2["data"])["username"];
    std::string password = (task2["data"])["password"];

    char line[1000];
    sprintf(line,"username=%s&password=%s", &username[0], &password[0]);

    sockfd = open_connection(host, port, AF_INET, SOCK_STREAM, 0);
    message = compute_post_request(host, url, line, cookie, NULL, NULL);
    
    send_to_server(sockfd, message);
    memset(response, 0 ,sizeof(response));
    receive_from_server(sockfd, response);
    printf("%s\n", message);
    printf("%s\n\n", response);
    close_connection(sockfd);
    
    
    /*---------TASK3-------------*/
    free(message);
    memset(cookie, 0 ,sizeof(cookie));
    memset(js, 0 ,sizeof(js));


    getcookie(response, cookie, js);
    json task3 = json::parse(js);
   	u = task3["url"];
    sprintf(url,"%s",&u[0]);
    
    std::string id = ((task3["data"])["queryParams"])["id"];
    std::string token = (task3["data"])["token"];
    char Authorization[2000];
    
    sprintf(Authorization, "Basic %s",&token[0]);
    
    memset(line, 0 ,sizeof(line));

    sprintf(line,"raspuns1=omul&raspuns2=numele&id=%s", &id[0]);
    sockfd = open_connection(host, port, AF_INET, SOCK_STREAM, 0);
    message = compute_get_request(host, url, line, cookie, Authorization);
    
    
    send_to_server(sockfd, message);
    memset(response, 0 ,sizeof(response));
    receive_from_server(sockfd, response);
    
    printf("%s\n", message);
    printf("%s\n\n", response);
    close_connection(sockfd);
    


    /*---------Task4----------------*/
    free(message);
    memset(cookie, 0 ,sizeof(cookie));
    memset(js, 0 ,sizeof(js));    

    getcookie(response, cookie, js);
    json task4 = json::parse(js);
    u = task4["url"];
    sprintf(url,"%s",&u[0]);
    bzero(line, strlen(line));

    sockfd = open_connection(host, port, AF_INET, SOCK_STREAM, 0);
    message = compute_get_request(host, url, line, cookie, Authorization);
    

    send_to_server(sockfd, message);
    memset(response, 0 ,sizeof(response));
    receive_from_server(sockfd, response);
    
    printf("%s\n", message);
    printf("%s\n\n", response);
    close_connection(sockfd);

    /*---------Task5----------------*/


    free(message);
    memset(cookie, 0 ,sizeof(cookie));
    memset(js, 0 ,sizeof(js));
    getcookie(response, cookie, js);
    json task5 = json::parse(js);
    
    u = (task5["data"])["url"];
    sprintf(url,"%s",&u[0]);
    char ip[100];
    char urll[100];
    
    getip(url,ip,urll);
    
    
    bzero(line, strlen(line));
    std::string q = ((task5["data"])["queryParams"])["q"];
    std::string APPID = ((task5["data"])["queryParams"])["APPID"];
    sprintf(line,"q=%s&APPID=%s", &q[0], &APPID[0]);

    sockfd = open_connection(ip, 80, AF_INET, SOCK_STREAM, 0);
    message = compute_get_request(ip, urll, line, NULL, NULL);
        
    
    send_to_server(sockfd, message);
    memset(response, 0 ,sizeof(response));
    receive_from_server(sockfd, response);
    
    printf("%s\n", message);
    printf("%s\n\n", response);
    close_connection(sockfd);
	



    free(message);
    memset(js, 0 ,sizeof(js));
    getcookie(response, cookie, js);
    
    
    u = task5["url"];
    sprintf(url,"%s",&u[0]);
    std::string cont = task5["type"];
    char content_type[100];
    sprintf(content_type, "%s",&cont[0]);

    
	sockfd = open_connection(host, port, AF_INET, SOCK_STREAM, 0);
    memset(response, 0 ,sizeof(response));
    message = compute_post_request(host, url, js, cookie, Authorization, content_type);
    
    send_to_server(sockfd, message);
    receive_from_server(sockfd, response);
    printf("%s\n", message);
    printf("%s\n\n", response);
    close_connection(sockfd);
    




    free(message);
    return 0;
}
