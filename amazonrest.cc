/* The MIT License (MIT)

Copyright (c) 2014 marcsokes

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE. */


#include "amazonrest.h"
#include "base64.h"
#include <time.h>
#include <sys/time.h>

int usage(){
	fprintf(stderr, "\nUsage: -R [Locale: CA DE US...]\n"
			" -S [Service: AWSECommerceService...]\n"
			" -O [Operation: ItemSearch...]\n"
			" -A [AWSAccessKeyId: SR51DMQFHZWY...]\n"
			" -K [SecretKey: YOUR1SECRET2KEY3HERE4...]\n"
			" -o [SearchIndex: SearchIndex=Apparel...]\n"
			" -o [Keywords: Keywords=Shirt...]\n"
			" -o etc...\n"
			"\nCapital letter flags are required and only require the value following them."
			"\nLowercase, or -o parameters are optional and require both param and value in param=val format."
			"\nIf you do not provide the appropriate options for your request, your request will fail."
			"\nRefer to Amazon's API guide for more information.\n\n");
	return 0;
}


AMAZON_REQUEST::amazon_api_rest_request(){ 
	service=operation=access_key_id=secret_key=0; 
}

AMAZON_REQUEST::amazon_api_rest_request(const char *service_arg, 
				const char *operation_arg,
				const char *access_key_id_arg, 
				const char *secret_key_arg,
				const char *locale,
				std::vector <std::string> opts){
	service = service_arg;
	operation = operation_arg;
	access_key_id = access_key_id_arg;
	secret_key = secret_key_arg;
	options = opts;
	std::string str = AMAZON_API_ENDPOINT_SUBDOM_AND_DOM;
	str += locale;
	api_endpoint = const_cast<char*>(str.c_str());
	build_amazon_rest_request();
}

std::string AMAZON_REQUEST::get_rest_request(){
	return rest_request;
}

const char *AMAZON_REQUEST::get_api_endpoint(){
	return api_endpoint;
}

int AMAZON_REQUEST::build_amazon_rest_request(){
	rest_request += "?";
	if (service){
		rest_request += "Service=";
		rest_request += service;
	}
	if (operation){
		rest_request += "&Operation=";
		rest_request += operation;
	}
	if (access_key_id){
		rest_request += "&AWSAccessKeyId=";
		rest_request += access_key_id;
	}
	
	for (std::vector<std::string>::iterator it = options.begin(); it != options.end(); it++){
		rest_request += "&";
		rest_request += *it;
	}
	rest_request += "&Timestamp=";
	rest_request += timestamp.get_utc_time_string();
	sign_api_rest_request();
	return 0;
}

int AMAZON_REQUEST::sign_api_rest_request(){
	std::string loader;
	std::vector <std::string> vec;

	char *tmp = escape_given_characters(const_cast<char*>(rest_request.c_str()), ",:+");
	rest_request = tmp;
	free(tmp);
	for (std::string::iterator it= rest_request.begin(); it != rest_request.end(); it++){
		if (*it != '&' && *it != '?')
			loader.push_back(*it);
		else if (*it == '?'){
			loader.clear();
		}
		else { 
			vec.push_back(loader);
			loader.clear();
		}
	}
	vec.push_back(loader);	
	loader.clear();
	for (std::vector<std::string>::iterator it = vec.begin(); it != vec.end(); loader += *it+"&", vec.erase(it), it = vec.begin())
		for (std::vector<std::string>::iterator it2 = it; it2 != vec.end(); it2++)
				if ((*it2).compare(*it) < 0){
					it = it2;
					it2 = vec.begin();
				}
	loader.erase(loader.end()-1);
	loader.insert(0, "\n");
	loader.insert(0, AMAZON_API_ENDPOINT_PATH);
	loader.insert(0, "\n");
	loader.insert(0, api_endpoint);
	loader.insert(0, "GET\n");

	std::cout << loader << "\n\n";
	generate_hmac256bit_hash(const_cast<char*>(loader.c_str()), secret_key);
	tmp = escape_given_characters(const_cast<char*>(signature.c_str()), "+=/");
	signature = tmp;
	free(tmp);
	rest_request += "&Signature=" + signature; //signed request
	return 0;
}

char *AMAZON_REQUEST::escape_given_characters(const char *http, const char *given_chars){
	const char *p;
	char *q = 0;
	char *result = 0;
	const char *tmp = given_chars;
	int unacceptable = 0;
	if (!http) return NULL;
	for(p=http; *p; p++, tmp=given_chars)
		while (*tmp)
			if (*tmp++ == *p)
				unacceptable++;

	if (!(result = (char*)calloc(p-http + unacceptable+unacceptable + 1, sizeof(char)))){
	    fprintf(stderr, "Failed memory allocation, terminating program\n");
	    exit(1);
	}
	for(q=result, p=http; *p; p++, tmp=given_chars) {
		unsigned char a = *p;
		while (*tmp)
			if (*tmp++ == a) {
			    *q++ = '%';
			    *q++ = hex[a >> 4];
			    *q++ = hex[a & 15];
			     break;
			}
			else if (!(*tmp))*q++ = *p;
	    }
	*q++ = 0;

	return result;
}

void AMAZON_REQUEST::generate_hmac256bit_hash(const char *message, const char *key_buf){

	/* integrate the return of your hmacsha256 through here */ 

}

char *escape_http_for_rfc(const char *http, int mask){
	const char *p;
	char *q = 0;
	char *result = 0;
	int unacceptable = 0;
	if (!http) return NULL;
	for(p=http; *p; p++)
	    if (!ACCEPTABLE((unsigned char)*p))
		unacceptable++;

	if (!(result = (char *)calloc(p-http + unacceptable+unacceptable + 1, sizeof(char)))){
	    fprintf(stderr, "Failed memory allocation, terminating program\n");
	    exit(1);
	}
	for(q=result, p=http; *p; p++) {
		unsigned char a = *p;
		if (!ACCEPTABLE(a)) {
			*q++ = '%';
			*q++ = hex[a >> 4];
			*q++ = hex[a & 15];
		}
		else *q++ = *p;
	}
	*q++ = 0;

	return result;
}

std::string make_full_http_request(const char *fq_domain, const char *path, const char *query){
	const char *useragent = "\r\nUser-Agent: Mozilla/5.0 (compatible; amazonAPIC++;)";

	std::string str = "GET ";
	str += path;
	str += query;
	str += " HTTP/1.1\r\nHOST: ";
	str += fq_domain;
	str += useragent;
	str += "  \r\n\r\n";
	
	return str;
}

int send_api_request(int socket_fd, const char *full_rq){
	struct pollfd pol_fds = {0};
	pol_fds.fd = socket_fd;
	int x = 0;
	int len = strlen(full_rq) + 1;

	while (send(socket_fd, full_rq, len, MSG_NOSIGNAL) != len){
		if (errno != EAGAIN && errno != EWOULDBLOCK){	
			return -1;
		}
		pol_fds.events = POLLOUT;
		x = poll(&pol_fds, 1, 4000);
		if (x < 1){
			return -1;					
		}
	}
	return 0;
}

char *receive_api_response(int socket_fd){
	int nread = 0;
	int mrr_nread = 0;
	struct pollfd pol_fds = {0};
	pol_fds.fd = socket_fd;
	pol_fds.events = POLLIN;
	char *page = (char *)calloc(SOCKET_READ_BUFFER_SIZE, sizeof(char));
	char *page_tmp = (char *)calloc(SOCKET_TEMP_READ_BUFFER_SIZE, sizeof(char));

	int x = poll(&pol_fds, 1, 6000);
	if (x < 1){
		free(page);
		free(page_tmp);
		return NULL;
	}
	while ((mrr_nread = recv(socket_fd, page_tmp, SOCKET_TEMP_READ_BUFFER_SIZE - 1, 0)) > 0 
		&& nread + mrr_nread < SOCKET_READ_BUFFER_SIZE - 1){
		memcpy(page + nread, page_tmp, mrr_nread);
		nread += mrr_nread;
		bzero(page_tmp, mrr_nread);
		x = poll(&pol_fds, 1, 2500);
		if (x == -1){
			free(page);
			free(page_tmp);
			return NULL;
		}
		else if (x == 0){
			break;
		}
	}	
	if (nread < 1) {
		free(page);
		free(page_tmp);
		return NULL;		
	}
	page[nread] = 0;
	free(page_tmp);
	return page;
}

int establ(char *dmn_next, const char *port, struct addrinfo *templ, struct addrinfo **holder){
	int x = 0;
	int socket_fd = 0;
	struct pollfd pol_fds = {0};
	pol_fds.events = POLLOUT;
	struct addrinfo *helper = NULL;
	
	socket_fd = getaddrinfo(dmn_next, port, templ, holder);
	helper = *holder;
	if (socket_fd != 0){
		fprintf(stderr,"%s", gai_strerror(socket_fd));
		helper = NULL;
		return -1;
	}
	for ( ; helper != NULL; helper = helper -> ai_next){
		socket_fd = socket(helper -> ai_family, helper -> ai_socktype, helper -> ai_protocol);
		if (socket_fd == -1)
			continue;
		fcntl(socket_fd, F_SETFL, O_NONBLOCK);
		pol_fds.fd = socket_fd;
		if (connect(socket_fd, helper -> ai_addr, helper -> ai_addrlen) != -1){
			helper -> ai_next = NULL;
			freeaddrinfo(*holder);
			break; //found it!
		}
		else if (errno != EINPROGRESS) {
			close(socket_fd);
		}
		else {
			x = poll(&pol_fds, 1, 4000);
			if (x == -1){
				close(socket_fd);
				continue;
			}
			else if (x == 0){
				close(socket_fd);
				continue;
			}							
			int result = 0;
			socklen_t result_len = sizeof(result);
			if (getsockopt(socket_fd, SOL_SOCKET, SO_ERROR, &result, &result_len) < 0){
				close(socket_fd);
				continue;
			} 
			if (result != 0){
				close(socket_fd);
			}
			else {
				helper -> ai_next = NULL;
				freeaddrinfo(*holder);
				break;
			}
		}
	}
	if (helper == NULL){ /* No address succeeded */
		freeaddrinfo(*holder);
		helper = NULL;
		return -1;
	}	
	return socket_fd;	
}

int main(int argc, char *argv[]){
	int ac = 1;
	const char *service, *access_key_id, *operation, *secret_key, *locale;
	char *av;
	std::vector <std::string> options;
	while (ac < argc - 1) {
		if (*argv[ac] == '-') {
			av = argv[ac] + 1;
			if (!strcmp(av, "R")) {
				switch (argv[ac+1][1]){
					case 'A': locale = "ca"; break;
					case 'N': locale = "cn"; break;
					case 'E': locale = "de"; break;
					case 'R': locale = "fr"; break;
					case 'T': locale = "it"; break;
					case 'P': locale = "co.jp"; break;
					case 'K': locale = "co.uk"; break;
					case 'S': if (argv[ac+1][0] == 'U')
							locale = "com"; 
						  else locale = "es";
						  break;	
					default: usage(); return 1;
				}
			} else if (!strcmp(av, "S")) {
				service = argv[ac+1];
			} else if (!strcmp(av, "O")) {
				operation = argv[ac+1];
			} else if (!strcmp(av, "A")) {
				access_key_id = argv[ac+1];
			} else if (!strcmp(av, "K")) {
				secret_key = argv[ac+1];
			} else if (!strcmp(av, "o")) {
				options.push_back(argv[ac+1]);
			} else {
				usage();
				return 1;
			}
			ac+=2;
		} else {
			usage();
			return 1;
		}
	} if (ac == 1){
		usage();
		return 1;
	}

	AMAZON_REQUEST request( service,
				operation, 
				access_key_id, 
				secret_key, 
				locale,
				options );
	
	std::string str = request.get_rest_request();
	str = make_full_http_request(request.get_api_endpoint(), AMAZON_API_ENDPOINT_PATH, const_cast<char*>(str.c_str()));

	struct addrinfo *holder = 0;
	struct addrinfo hints = {0};
	hints.ai_family = AF_UNSPEC;    
	hints.ai_socktype = SOCK_STREAM; 
	hints.ai_flags = 0;
	hints.ai_protocol = 0;

	char endpoint[50] = {0};
	memcpy(endpoint, const_cast<char*>(request.get_api_endpoint()), strlen(const_cast<char*>(request.get_api_endpoint())));
	
	int fd = establ(endpoint, "80", &hints, &holder);
	if (fd == -1){
		fprintf(stderr, "Error establishing connection with Amazon\n");
		return 1;
	}
	if (send_api_request(fd, const_cast<char*>(str.c_str()))){
		fprintf(stderr, "Error sending API request through socket\n");
		shutdown(fd, SHUT_RDWR);
		close(fd);
		return 1;
	}
	char *response = receive_api_response(fd);
	shutdown(fd, SHUT_RDWR);
	close(fd);	
	if (!response){
		fprintf(stderr, "Error receiving API response through socket\n");
		return 1;
	}

	std::cout << response;
	free(response);
	return 0;
}





