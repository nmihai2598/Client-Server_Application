#include <string>
#include <vector>
 
using std::vector;
using std::string;
#ifndef _REQUESTS_
#define _REQUESTS_

char *compute_get_request(char *host, char *url, char *url_params, char * cookie,
                     char *Authorization);
char *compute_post_request(char *host, char *url, char *form_data, char * cookie,
                      char *Authorization,char *content_type );
vector<string> dns_lookup(const string &host_name, int ipv=4);

#endif