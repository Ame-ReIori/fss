#include <iostream>
#include <vector>

#include <cryptoTools/Common/Defines.h>
#include <cryptoTools/Network/Channel.h>
#include <cryptoTools/Network/Session.h>
#include <cryptoTools/Network/IOService.h>

void test_net() {

  osuCrypto::IOService ios;

  auto ip = std::string("127.0.0.1");
  auto port = 4215;

  std::string server_socket = ip + ':' + std::to_string(port);
  std::string session_hint = "party0_1";

  osuCrypto::Session server(ios, server_socket, osuCrypto::SessionMode::Server, session_hint);
  osuCrypto::Session client(ios, server_socket, osuCrypto::SessionMode::Client, session_hint);

  osuCrypto::Channel chl0 = client.addChannel();
  osuCrypto::Channel chl1 = server.addChannel();

  std::cout << "Channel connected: " << chl0.isConnected() << std::endl;

  // chl0.waitForConnection();

  // chl0.onConnect([](const osuCrypto::error_code& ec) {
  //   if (ec)
  //     std::cout << "chl0 failed to connect: " << ec.message() << std::endl;
  // });

  // std::cout << "Channel connected = " << chl0.isConnected() << std::endl;

  std::vector<uint8_t> data{ 0, 1, 2, 3, 4 };
  chl0.asyncSend(data);

  std::vector<uint8_t> dest;
  chl1.recv(dest);

  for (int i = 0; i < dest.size(); i++) {
    printf("%u ", dest[i]);
  }
  std::cout << std::endl;

  std::cout << "Session: " << chl0.getSession().getName() << std::endl
            << "Channel: " << chl0.getName() << std::endl
            << "   Sent: " << chl0.getTotalDataSent() << std::endl
            << "   Recv: " << chl1.getTotalDataRecv() << std::endl;
}