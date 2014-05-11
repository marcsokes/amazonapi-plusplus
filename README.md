amazonapi-plusplus
==================

An API (wrapper) for the Amazon product advertising API for C/C++. It comes complete with a ready built command line utility for pulling API requests but it can obviously also be used as a C++ library for dev.

Current caveats: Everything works but you have to link your own SHA256-HMAC implementation through the generate_hmac function provided. This can be done very easily... if somebody wants to add a quality, light-weight and portable implementation they are welcome to do so. 

Additionally, currently you must make sure that all values going in as "optionals" are either properly percent encoded (only an issue if you want to use non ascii characters or reserved characters within values). This isn't really a big deal considering there has been a function provided for this use, escape_http_for_rfc(), which you can use on the constructor's arguments before you call the actual instructor. If somebody wants to integrate this function so the arguements get escaped after ingestion to take the responsibility off the user, you are welcome to so! 

Enjoy!
