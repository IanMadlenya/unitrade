#include <string>
#include <iostream>
#include <stdint.h>
#include <chrono>
#include <ctime>
#include <openssl/sha.h>
#include <openssl/evp.h>
#include "security.h"
#include <unistd.h>

namespace binsec {

#if defined(__linux__)

/* Linux. --------------------------------------------------- */
#include <sys/ioctl.h>
#include <net/if.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <cstdio>

std::string getMAC() {
  struct ifreq ifr;
  struct ifconf ifc;
  char buf[1024];
  int success = 0;
  int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
  if (sock == -1) {/* handle error*/
  };
  ifc.ifc_len = sizeof(buf);
  ifc.ifc_buf = buf;
  if (ioctl(sock, SIOCGIFCONF, &ifc) == -1) { /* handle error */
  }
  struct ifreq *it = ifc.ifc_req;
  const struct ifreq *const end = it + (ifc.ifc_len / sizeof(struct ifreq));
  for (; it != end; ++it) {
    strcpy(ifr.ifr_name, it->ifr_name);
    if (ioctl(sock, SIOCGIFFLAGS, &ifr) == 0) {
      if (!(ifr.ifr_flags & IFF_LOOPBACK)) {  // don't count loopback
        if (ioctl(sock, SIOCGIFHWADDR, &ifr) == 0) {
          success = 1;
          break;
        }
      }
    } else { /* handle error */
    }
  }

  char mac_address[20];

  if (success)
    sprintf(mac_address, "%02x%02x%02x%02x%02x%02x",
            (unsigned char)ifr.ifr_hwaddr.sa_data[0],
            (unsigned char)ifr.ifr_hwaddr.sa_data[1],
            (unsigned char)ifr.ifr_hwaddr.sa_data[2],
            (unsigned char)ifr.ifr_hwaddr.sa_data[3],
            (unsigned char)ifr.ifr_hwaddr.sa_data[4],
            (unsigned char)ifr.ifr_hwaddr.sa_data[5]);

  return std::string(mac_address);
}
#endif

#if defined(__APPLE__) && defined(__MACH__)

//  get mac from OSX
std::string getMAC() {
  char mac[20];
  FILE *fp = popen("ifconfig en0 | awk '/ether/{print $2}'", "r");
  fscanf(fp, "%s", mac);
  pclose(fp);
  return std::string(mac);
}

#endif

std::string sha256(std::string str) {
  char outputBuffer[65];
  unsigned char hash[SHA256_DIGEST_LENGTH];
  SHA256_CTX sha256;
  SHA256_Init(&sha256);
  SHA256_Update(&sha256, str.c_str(), strlen(str.c_str()));
  SHA256_Final(hash, &sha256);
  int i = 0;
  for (i = 0; i < SHA256_DIGEST_LENGTH; i++) {
    sprintf(outputBuffer + (i * 2), "%02x", hash[i]);
  }
  outputBuffer[64] = 0;
  return std::string(outputBuffer);
}

std::string hashdigest(std::string str, std::string digest) {
  EVP_MD_CTX *mdctx;
  const EVP_MD *md;

  unsigned char md_value[EVP_MAX_MD_SIZE];
  int md_len;
  OpenSSL_add_all_digests();

  md = EVP_get_digestbyname(digest.c_str());
  if (!md) {
    std::cerr << "Unknown message digest " + digest << std::endl;
    exit(1);
  }
  mdctx = EVP_MD_CTX_create();
  EVP_DigestInit_ex(mdctx, md, NULL);
  EVP_DigestUpdate(mdctx, str.c_str(), str.size());

  EVP_DigestFinal_ex(mdctx, md_value, (unsigned int *)&md_len);
  EVP_MD_CTX_destroy(mdctx);

  char *outputBuffer = (char *)malloc((1 + md_len) * sizeof(char));
  for (int i = 0; i < md_len; i++) {
    sprintf(outputBuffer + (i * 2), "%02x", md_value[i]);
  }
  outputBuffer[md_len * 2 - 1] = 0;

  std::string ret = std::string(outputBuffer);
  free(outputBuffer);
  return ret;
}

std::string getCode() {
  std::string ran = "MXNhtDvs";
  return GetUniqueMachineCode() + ran;
}

int verifyCode(std::string inans) {
  std::string ran = "MXNhtDvs";
  auto ans = sha256(GetUniqueMachineCode() + ran);
  if (ans == inans)
    return 0;
  else
    return 1;
}

std::string GetUniqueMachineCode() {
  char buf[64];
  std::string unique_str = getMAC();
  CPUID cpuID;
  cpuID.load(0);
  unique_str += cpuID.getCPUID();
  auto z = gethostname(buf, sizeof buf);
  if (z == -1) {
    fprintf(stderr, "%s: gethostname(2)\n", strerror(errno));
  }
  unique_str +=  string(buf);
  return unique_str;
};

std::string GetUniqueMachineCodeWithTime() {
  auto now = Clock::now();
  std::time_t now_c = Clock::to_time_t(now);
  struct tm *parts = std::localtime(&now_c);
  std::string unique_str = std::to_string(parts->tm_mon + 1) +
               std::to_string(1900 + parts->tm_year) + GetUniqueMachineCode() +
               std::to_string(parts->tm_mon);
  return unique_str;
};
}
//

