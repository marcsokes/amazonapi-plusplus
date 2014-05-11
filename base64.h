/* Portions of the base64 module were found on an online forum and are no work of my own! */

#ifndef _BASE64_H_
#define _BASE64_H_
#include <string>
std::string base64_encode(unsigned char const* , unsigned int len);
std::string base64_decode(std::string const& s);
#endif 
