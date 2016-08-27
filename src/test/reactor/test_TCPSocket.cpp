#include <gtest/gtest.h>
#include "aliens/reactor/TCPSocket.h"

using aliens::reactor::TCPSocket;

TEST(TestTCPSocket, TestSanity) {
  auto sock = TCPSocket::bindPort(9019);
  EXPECT_TRUE(sock.valid());
}
