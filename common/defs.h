#ifndef YOBAHACK_COMMON_DEFS_H_
#define YOBAHACK_COMMON_DEFS_H_

#include <cstdint>
#include <boost/asio/ip/tcp.hpp>

typedef boost::asio::ip::tcp GameProtocol;

const std::uint16_t kGamePort = 4440; // how about 1984, 4444?

#endif // YOBAHACK_COMMON_DEFS_H_
