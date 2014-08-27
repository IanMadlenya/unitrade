#ifndef _security_H_
#define _security_H_

#include <string>
#include <iostream>
#include <openssl/rsa.h>
#include <openssl/sha.h>
#include <openssl/pem.h>
#include <string>
#include <iostream>
#include <stdint.h>
#include <chrono>
#include <ctime>


using namespace std;

namespace binsec{

    typedef std::chrono::system_clock Clock;

    std::string getMAC();

    std::string sha256(std::string str);

    class CPUID {
        uint32_t regs[4];

        public:
        void load(unsigned i) {
            asm volatile("cpuid"
                    : "=a"(regs[0]), "=b"(regs[1]), "=c"(regs[2]), "=d"(regs[3])
                    : "a"(i), "c"(0));
            // ECX is set to zero for CPUID function 4
            // #endif
        }
        //
        std::string getCPUID(){
            std::string vendor;
            vendor += std::string((const char *)&EBX(), 4);
            vendor += std::string((const char *)&EDX(), 4);
            vendor += std::string((const char *)&ECX(), 4);

            return vendor;
        }
        const uint32_t &EAX() const { return regs[0]; }
        const uint32_t &EBX() const { return regs[1]; }
        const uint32_t &ECX() const { return regs[2]; }
        const uint32_t &EDX() const { return regs[3]; }
    };


    std::string GetUniqueMachineCode();
    std::string GetUniqueMachineCodeWithTime(); 
    std::string getCode();
    int verifyCode(std::string);
}

#endif
