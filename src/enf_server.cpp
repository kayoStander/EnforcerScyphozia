#include <chrono>
#include <thread>

#include "enf_server.hpp"
#include "../common/assert.hpp"
#ifdef _WIN32
#define _WIN32_WINNT 0x0A00;
#endif

std::vector<char> buffer(20*1024);
namespace Enforcer{
  Server::Server() {
    boost::system::error_code errorCode;

    boost::asio::io_context context;
    boost::asio::io_context::work idleWork(context);
    std::thread threadContext{[&](){context.run();}};

    tcp::endpoint endpoint(boost::asio::ip::make_address("51.38.81.49",errorCode),80);

    tcp::socket socket(context);

    socket.connect(endpoint,errorCode);

    if (errorCode){
      LOG_ERROR(Server,"Failed to connect");
      return;
    }

    LOG_INFO(Server,"Connected");

    if (socket.is_open()){
      GetData(socket);

      std::string httpRequest{
      "GET /index.html HTTP/1.1\r\n"
      "Host: david-barr.co.uk\r\n"
      "Connection: close\r\n\r\n"};

      socket.write_some(boost::asio::buffer(httpRequest.data(), httpRequest.size()),errorCode);

      std::this_thread::sleep_for(std::chrono::milliseconds(2000)); // awful

      context.stop();
      if (threadContext.joinable()){threadContext.join();}
    }
  }
  Server::~Server() {
  }

  void Server::GetData(tcp::socket& socket){
    socket.async_read_some(boost::asio::buffer(buffer.data(),buffer.size()),[&](boost::system::error_code errorCode,
                                                                                  std::size_t length){
      if (!errorCode){
        LOG_INFO(Server,"Read {} bytes", length);
        for (u32 i{0};i<length;i++){
          std::cout << buffer[i];
        }
        GetData(socket);
      }
    });
  }
}