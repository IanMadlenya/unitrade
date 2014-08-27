#include <cstdio>
#include <errno.h>
#include <unistd.h>

#include <string.h>
#include <sys/socket.h>
#include <resolv.h>
#include <netdb.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <iostream>
#include "ssl_conversation.h"
#include "security.h"
#include "rsacrypto.h"
#include "bin.h"
#include "control.h"
#include <string>

using namespace std;
#define FAIL    -1

bincon::CommandFormatList command_book;

int main(int argc, char *argv[])
{
    int key_len;
    unsigned char* key {nullptr};
    std::string ans;
    binsec::SSLStream sslstream("client");
    if ( argc != 5 )
    {
        printf("Usage: %s <host> <portnum> <prikey> <pass>\n", argv[0]);
        exit(0);
    }
    binsec::RSACrypto crypto(argv[3],argv[4]);
    SSL_library_init();
    sslstream.stream_client_setup(atoi(argv[2]), argv[1]);
    if (sslstream.stream_connect())
    {
      cerr<<"--->>> ERROR: cannot connect"<<std::endl;
      return -1;
    }
    key_len = sslstream.stream_get_key(key);
    auto hash = binsec::sha256(crypto.decrypt(key, key_len));
    bin::CommandPacket command(hash); 
    command.SetCommand("check", "auth"); 
    sslstream.stream_ask(CompileCommandPacket(command),ans);

    cout<<ans<<endl;
  
    bincon::ParseCommandFormat("command.ini", command_book);
    //PrintCommandFormatList(command_book);
    bincon::Shell comshell(&sslstream, hash);
    comshell.Run(command_book);

    
    sslstream.stream_clean();


    if (key)
    free(key);
}
