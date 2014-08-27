#include <errno.h>
#include <unistd.h>

#include <string.h>
#include "openssl/ssl.h"
#include "openssl/err.h"
#include "ssl_conversation.h"
#include "rsacrypto.h"
#include "security.h"
#include "bin.h"
#include "license.h"
#include "action.h"

#define FAIL -1
using namespace std;

namespace unitrade
{
  binsec::Meta meta;
  binsec::License license;
}


int main(int argc, char *argv[]) {
  
  unitrade::license.Load("license.lic");
  cout << "Lisense: " << unitrade::license.time_to_expire << " days left" << endl;
  unitrade::license.time_to_expire--;
  unitrade::license.Save();
  // binsec::AesCrypto aescrypto;
  // aescrypto.Init(binsec::GetUniqueMachineCode());
  // cout<<aescrypto.decrypt()<<endl;

  binsec::SSLStream sslstream("server");
  if (argc != 5) {
    printf("Usage: %s <portnum> <cert> <key> <pubkey> \n", argv[0]);
    exit(0);
  }
  SSL_library_init();
  bin::CommandPacket command;
  sslstream.stream_server_setup(atoi(argv[1]), argv[2], argv[3]);
  binsec::RSACrypto crypto(argv[4]);
  crypto.encrypt(binsec::GetUniqueMachineCode());
  auto hash_for_check = binsec::sha256(binsec::GetUniqueMachineCode());

  sslstream.server_start();
  unsigned int status;

  signal(SIGPIPE, SIG_IGN);
  while (1) {
    sslstream.stream_accept();
    sslstream.isDisConnected = false;
    status = sslstream.stream_send_key(crypto.full_encrypted_msg,
                                       crypto.full_encrypted_msg_len);
    if (status != 0) {
      cerr << "--->>> keys not sent, ignore broken connection" << endl;
      continue;
    } 
    else 
    {
      while (1)
      {
        command.Reset();
        bin::ParseCommandPacket(sslstream.stream_read(), command); 
        if (sslstream.isDisConnected) {
          cerr << "--->>> client connection broken" << endl;
          sslstream.stream_disconnect();
          break;
        }
        if (command.auth == hash_for_check) { 
          if (command.code=="CheckAuth") 
            status = sslstream.stream_answer("--->>> Authorization Granted");
          else
          {
            bincon::ActionOnCommand(command);
            status = sslstream.stream_answer(command.answer);
          }
          if (status != 0) {
            cerr << "--->>> answer not sent, ignored" << endl;
            sslstream.stream_disconnect();
            break;
          }
        } else {
          status =
              sslstream.stream_answer("--->>> ERROR: password not accepted");
          sslstream.stream_disconnect();
          break;
        }
      }
    }
    // sslstream.stream_process();
    // sslstream.stream_disconnect();
  }
  sslstream.stream_clean();
}
