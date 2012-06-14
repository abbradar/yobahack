#ifndef YOBAHACK_COMMON_DEFS_H_
#define YOBAHACK_COMMON_DEFS_H_

#include <cstdint>
#include <boost/asio.hpp>

typedef boost::asio::ip::tcp GameTransport;

const uint16_t kDefaultPort = 4440; // how about 1894, 4444?


#endif // YOBAHACK_COMMON_DEFS_H_
