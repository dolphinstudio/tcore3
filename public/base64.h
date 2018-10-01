#ifndef __Base64_h__
#define __Base64_h__

#include <string>

namespace tlib {
    std::string base64_encode(unsigned char const*, unsigned int len);
    std::string base64_decode(std::string const& s);
}

#endif /* __Base64_h__ */
