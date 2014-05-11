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


#ifndef _AMAZON_API_H_
#define _AMAZON_API_H_

#include <iostream>
#include <vector>
#include <string>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <netdb.h>
#include "utctimestamp.h"

unsigned char isAcceptable[96] =
{/* 0x0 0x1 0x2 0x3 0x4 0x5 0x6 0x7 0x8 0x9 0xA 0xB 0xC 0xD 0xE 0xF */
    0x0,0xC,0x0,0xC,0xC,0xC,0xC,0xC,0xC,0xC,0xF,0xE,0xC,0xF,0xF,0xC, /* 2x  !"#$%&'()*+,-./   */
    0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF,0x8,0xC,0x0,0xC,0x0,0xC, /* 3x 0123456789:;<=>?   */
    0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF, /* 4x @ABCDEFGHIJKLMNO   */
    0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xC,0x0,0xC,0x0,0xF, /* 5X PQRSTUVWXYZ[\]^_   */
    0x0,0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF, /* 6x `abcdefghijklmno   */
    0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF,0x0,0x0,0x0,0xF,0x0  /* 7X pqrstuvwxyz{\}~DEL */
};
const char *hex = "0123456789ABCDEF";

#define ACCEPTABLE(a) ( a>=32 && a<128 && ((isAcceptable[a-32]) & mask))
#define SOCKET_READ_BUFFER_SIZE 200000
#define SOCKET_TEMP_READ_BUFFER_SIZE 100000
#define AMAZON_API_ENDPOINT_SUBDOM_AND_DOM "webservices.amazon."
#define AMAZON_API_ENDPOINT_PATH "/onca/xml"

const char *amazon_api_region;
const char *amazon_api_endpoint;
std::string make_full_http_request(const char * fq_domain, const char *path, const char *query);
int establ(char *dmn_next, const char *port, struct addrinfo *templ, struct addrinfo **holder);
int send_api_request(int socket_fd, const char *full_rq);
char *receive_api_response(int socket_fd);

class amazon_api_rest_request {
public:
	std::vector <std::string> options;
	amazon_api_rest_request();
	amazon_api_rest_request(const char *service_arg, 
					const char *operation_arg,
					const char *access_key_id_arg, 
					const char *secret_key_arg,
					const char *locale,
					std::vector <std::string> options);
	int build_amazon_rest_request();
	std::string get_rest_request();
	const char *get_api_endpoint();
private:
	const char *service;
	const char *access_key_id;
	const char *operation;
	const char *secret_key;	
	const char *api_endpoint;
	std::string rest_request;
	TIMESTAMP timestamp;
	std::string signature;
	int sign_api_rest_request();
	char *escape_given_characters(const char *http, const char *given_chars);
	void generate_hmac256bit_hash(const char *message, const char *key_buf);

} typedef AMAZON_REQUEST;


#endif /* defined(_AMAZON_API_H_) */

