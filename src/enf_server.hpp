#pragma once

#include <deque>
#include <vector>

#define ASIO_STANDALONE
#include <boost/asio.hpp>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>

#include "../common/types.hpp"
#include "../common/logging/log.hpp"

using tcp = boost::asio::ip::tcp;

namespace Enforcer {
  enum class CustomMessageTypes : u32 {
    A=0,
    B
  };

  template <typename T>
  struct Header{
    T id{};
    u32 size{0};
  };
  template <typename T>
  struct Message {
    Header<T> header{};
    std::vector<u8> body;

    [[nodiscard]] u32 size() const noexcept {
      return sizeof(Header<T>)+body.size();
    }
    // for logging
    friend std::ostream& operator<<(std::ostream& os, const Message<T>& msg){
      os << "ID => " << static_cast<int>(msg.header.id) << " Size => " << msg.body.size();
      return os;
    }

    template<typename DataType>
    friend Message<T>&operator<<(Message<T>&msg, const DataType& data){
      static_assert(std::is_standard_layout_v<DataType>, "DataType must be standard layout");
      const u32 i{static_cast<u32>(msg.body.size())};

      msg.body.resize(msg.body.size()+sizeof(DataType));

      std::memcpy(msg.body.data()+i,&data,sizeof(DataType));

      msg.header.size = msg.size();
      return msg;
    }

    template<typename DataType>
    friend Message<T>&operator>>(Message<T>&msg, DataType& data){
      static_assert(std::is_standard_layout_v<DataType>, "DataType must be standard layout");

      const u32 i{static_cast<u32>(msg.body.size()-sizeof(DataType))};

      std::memcpy(&data,msg.body.data()+i,sizeof(DataType));

      msg.body.resize(i);
      return msg;
    }
  };

  template<typename T>
  class ThreadSafeQueue {
  public:
    ThreadSafeQueue() = default;
    ThreadSafeQueue(const ThreadSafeQueue<T>&) = delete;
    ~ThreadSafeQueue() {Clear();}
    // returns and maintain item at front of queue
    const T& Front() {
      std::scoped_lock lock(mutexQueue);
      return dequeQueue.front();
    }
    // returns and maintain item at back of queue
    const T& Back() {
      std::scoped_lock lock(mutexQueue);
      return dequeQueue.back();
    }
    // adds item to front of queue
    void PushFront(const T& item) {
      std::scoped_lock lock(mutexQueue);
      dequeQueue.emplace_front(std::move(item));
    }

    T PopFront() {
      std::scoped_lock lock(mutexQueue);
      T t{std::move(dequeQueue.front())};
      dequeQueue.pop_front();
      return t;
    }

    T PopBack() {
      std::scoped_lock lock(mutexQueue);
      T t{std::move(dequeQueue.back())};
      dequeQueue.pop_back();
      return t;
    }

    bool Empty() const {
      std::scoped_lock lock(mutexQueue);
      return dequeQueue.empty();
    }

    u32 Count() const {
      std::scoped_lock lock(mutexQueue);
      return dequeQueue.size();
    }

    void Clear() {
      std::scoped_lock lock(mutexQueue);
      dequeQueue.clear();
    }
  protected:
    std::mutex mutexQueue;
    std::deque<T> dequeQueue;
  };

 template<typename T>
 struct OwnedMessage;

 template<typename T>
  class Connection : public std::enable_shared_from_this<Connection<T>> /*same as *this but as shared ptr*/{
  public:
    Connection(boost::asio::io_context& asioContext, tcp::socket &socket):socket(std::move(socket)),asioContext(asioContext) {} // error cuz not initialize

    virtual ~Connection(){}

    bool ConnectToServer();
    bool Disconnect();
    [[nodiscard]] bool IsConnected() const noexcept;

    bool Send(const Message<T>& msg);
  protected:
    tcp::socket socket;

    boost::asio::io_context& asioContext;

    ThreadSafeQueue<Message<T>> messageOut;
    ThreadSafeQueue<OwnedMessage<T>> messagesIn;
  };

  template <typename T>
  struct OwnedMessage {
    std::shared_ptr<Connection<T>> remote;
    Message<T> message;

    friend std::ostream& operator<<(std::ostream& os, const OwnedMessage<T>& message) {
      os << message.message;
      return os;
    }
  };

  template <typename T>
  class ClientInterface {
  public:
    ClientInterface():socket(context){}
    virtual ~ClientInterface(){Disconnect();}

    bool Connect([[maybe_unused]]const std::string& host, [[maybe_unused]]const u16 port) {
      try {
        connection = std::make_unique<Connection<T>>();
        tcp::resolver resolver(context);
        //endpoints = resolver.resolve(host, std::to_string(port));

        //connection->ConnectToServer(endpoints);

        threadContext = std::thread([this](){context.run();});
      } catch (std::exception& e) {
        LOG_CRITICAL(Client,"Client exception => ", e.what());
        return false;
      }
      return true;
    }
    void Disconnect() {
      if (IsConnected()) {
        connection->Disconnect();
      }

      context.stop();
      if (threadContext.joinable()) {threadContext.join();}

      connection.release();
    }
    [[nodiscard]] bool IsConnected() const noexcept {
      if (!connection) {return false;}
      return connection->IsConnected();
    }
    ThreadSafeQueue<OwnedMessage<T>>& Incoming() {
      return messagesIn;
    }
  private:
    // incoming messages from server aka In
    ThreadSafeQueue<OwnedMessage<T>> messagesIn;
  protected:
    // asio deals with the data transfer
    boost::asio::io_context context;
    // thread to execute its work cmds
    std::thread threadContext;
    // hardware socket that is connected to server
    tcp::socket socket;
    // client can only have a single instance of a connection to the server
    std::unique_ptr<Connection<T>> connection;
  };

  class Server {
  public:
    Server();

    ~Server();
  private:
    void GetData(tcp::socket& socket);
  };

  class Client {
  };
}
