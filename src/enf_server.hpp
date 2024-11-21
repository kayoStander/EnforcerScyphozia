#pragma once

#include <algorithm>
#include <deque>
#include <memory>
#include <thread>
#include <vector>

#define ASIO_STANDALONE
#include <boost/asio.hpp>
#include <boost/asio/ts/internet.hpp>

#include "../common/types.hpp"
#include "../common/logging/log.hpp"

using tcp = boost::asio::ip::tcp;

namespace Enforcer {
  enum class CustomMessageTypes : u32 {
    MessageClient = 0,
    MessageAllClients,
    OnClientConnect,
    OnClientDisconnect,
    OnMessage,
    ServerPing
  };

  template<typename T>
  struct Header {
    T id{};
    u32 size{0};
  };

  template<typename T>
  struct Message {
    Header<T> header{};
    std::vector<u8> body;

    [[nodiscard]] u32 size() const noexcept {
      return sizeof(Header<T>) + body.size();
    }

    // for logging
    friend std::ostream &operator<<(std::ostream &os, const Message<T> &msg) {
      LOG_INFO(Client, "ID,Size => {},{}", msg.header.id, msg.body.size());
      return os;
    }

    template<typename DataType>
    friend Message<T> &operator<<(Message<T> &msg, const DataType &data) {
      static_assert(std::is_standard_layout_v<DataType>, "DataType must be standard layout");
      const u32 i{static_cast<u32>(msg.body.size())};

      msg.body.resize(msg.body.size() + sizeof(DataType));

      std::memcpy(msg.body.data() + i, &data, sizeof(DataType));

      msg.header.size = msg.size();
      return msg;
    }

    template<typename DataType>
    friend Message<T> &operator>>(Message<T> &msg, DataType &data) {
      static_assert(std::is_standard_layout_v<DataType>, "DataType must be standard layout");

      const u32 i{static_cast<u32>(msg.body.size() - sizeof(DataType))};

      std::memcpy(&data, msg.body.data() + i, sizeof(DataType));

      msg.body.resize(i);

      msg.header.size = msg.size();
      return msg;
    }
  };

  template<typename T>
  class ThreadSafeQueue {
  public:
    ThreadSafeQueue() = default;

    ThreadSafeQueue(const ThreadSafeQueue<T> &) = delete;

    ~ThreadSafeQueue() { Clear(); }
    // returns and maintain item at front of queue
    const T &Front() {
      std::scoped_lock lock(mutexQueue);
      return dequeQueue.front();
    }

    // returns and maintain item at back of queue
    const T &Back() {
      std::scoped_lock lock(mutexQueue);
      return dequeQueue.back();
    }

    void PushBack(const T& item)
    {
        std::scoped_lock lock(mutexQueue);
        dequeQueue.emplace_back(std::move(item));
    }

    // adds item to front of queue
    void PushFront(const T &item) {
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

    [[nodiscard]] bool Empty() {
      std::scoped_lock lock(mutexQueue);
      return dequeQueue.empty();
    }

    [[nodiscard]] u32 Count() {
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
  class Connection : public std::enable_shared_from_this<Connection<T> > /*same as *this but as shared ptr*/ {
  public:
    enum class owner : u8 {
      server = 0, client
    };

    Connection(owner parent, boost::asio::io_context &asioContext, tcp::socket socket,
               ThreadSafeQueue<OwnedMessage<T>> &messagesIn): socket(std::move(socket)), asioContext(asioContext),
                                                               messagesIn(messagesIn) {
      ownerType = parent;
    }

    virtual ~Connection() {
    }

    void ConnectToClient(const u32 uid=0) {
      if (ownerType == owner::server) {
        if (socket.is_open()) {
          id = uid;
          ReadHeader();
        }
      }
    }
    void ConnectToServer(tcp::resolver::results_type& endpoints) {
      if (ownerType == owner::client) {
        boost::asio::async_connect(socket,endpoints,
          [this](boost::system::error_code ec, [[maybe_unused]] tcp::endpoint endpoint) {
            if (!ec) {
              ReadHeader();
            }
          });
      }
    }
    void Disconnect() {
      if (IsConnected()) {
        boost::asio::post(asioContext,[this]() {
          socket.close();
        });
      }
    }

    [[nodiscard]] bool IsConnected() const noexcept {
      return socket.is_open();
    }
    [[nodiscard]] u32 GetId() const noexcept {
      return id;
    }

    void Send(const Message<T> &msg) {
      boost::asio::post(asioContext, [this, msg]() {
        const bool writingMessage{messageOut.Empty()};
        messageOut.PushBack(msg);
        if (writingMessage) {
          WriteHeader();
        }
      });
    }
  private:
    // ASYNC
    void ReadHeader() {
      boost::asio::async_read(socket, boost::asio::buffer(&messageTemporaryIn.header,sizeof(Header<T>)),
        [this](boost::system::error_code ec, [[maybe_unused]] std::size_t length) {
          if (!ec) {
            if (messageTemporaryIn.header.size > 0) {
              messageTemporaryIn.body.resize(messageTemporaryIn.header.size);
              ReadBody();
            }else {
              AddToIncomingMessageQueue();
            }
          }else {
            LOG_CRITICAL(Server, "Read Header {} fail.", id);
            socket.close();
          }
        });

    }
    // ASYNC
    void ReadBody() {
      boost::asio::async_read(socket,boost::asio::buffer(
        messageTemporaryIn.body.data(), messageTemporaryIn.body.size()),
        [this](boost::system::error_code ec, [[maybe_unused]] std::size_t length) {
          if (!ec) {
            AddToIncomingMessageQueue();
          }else {
            LOG_CRITICAL(Server, "Read Body {} fail.", id);
            socket.close();
          }
        });
    }
    // ASYNC
    void WriteHeader() {
      boost::asio::async_write(socket,boost::asio::buffer(&messageOut.Front().header, sizeof(Header<T>)),
        [this](boost::system::error_code ec, [[maybe_unused]] std::size_t length) {
          if (!ec) {
            if (messageOut.Front().body.size() > 0) {
              WriteBody();
            } else {
              messageOut.PopFront();
              if (!messageOut.Empty()) {
                WriteHeader();
              }
            }
          } else {
            LOG_CRITICAL(Server, "Write Header {} fail.", id);
            socket.close();
          }
        });
    };
    // ASYNC
    void WriteBody() {
      boost::asio::async_write(socket,boost::asio::buffer(messageOut.Front().body.data(), messageOut.Front().body.size()),
        [this](boost::system::error_code ec, [[maybe_unused]] std::size_t length) {
          if (!ec) {
            messageOut.PopFront();
            if (messageOut.Empty()) {
              WriteHeader();
            }
          }else {
            LOG_CRITICAL(Server, "Write Body {} fail.", id);
            socket.close();
          }
      });
    }

    void AddToIncomingMessageQueue() {
      if (ownerType == owner::server) {
        messagesIn.PushBack({this->shared_from_this(), messageTemporaryIn});
      } else {
        messagesIn.PushBack({nullptr, messageTemporaryIn});
      }

      ReadHeader();
    }
  protected:
    tcp::socket socket;

    boost::asio::io_context &asioContext;

    ThreadSafeQueue<Message<T> > messageOut;
    ThreadSafeQueue<OwnedMessage<T> >& messagesIn;
    Message<T> messageTemporaryIn;

    owner ownerType{owner::server};
    u32 id{0};
  };

  template<typename T>
  struct OwnedMessage {
    std::shared_ptr<Connection<T> > remote;
    Message<T> message;

    friend std::ostream &operator<<(std::ostream &os, const OwnedMessage<T> &message) {
      os << message.message;
      return os;
    }
  };

  template<typename T>
  class ClientInterface {
  public:
    ClientInterface(): socket(context) {
    }

    virtual ~ClientInterface() { Disconnect(); }

    bool Connect(const std::string &host, u16 port) {
      try {
        tcp::resolver resolver(context);
        tcp::resolver::results_type endpoints = resolver.resolve(host,std::to_string(port));

        connection = std::make_unique<Connection<T>>(Connection<T>::owner::client,
          context,tcp::socket(context), messagesIn);

        connection->ConnectToServer(endpoints);

        threadContext = std::thread([this](){context.run();});
      } catch (std::exception& e) {
        LOG_CRITICAL(Client,"Client exception => {}", e.what());
        return false;
      }
      return true;
    }

    void Disconnect() {
      if (IsConnected()) {
        connection->Disconnect();
      }

      context.stop();
      if (threadContext.joinable()) { threadContext.join(); }

      connection.release();
    }

    [[nodiscard]] bool IsConnected() const noexcept {
      if (!connection) { return false; }
      return connection->IsConnected();
    }

    [[nodiscard]] ThreadSafeQueue<OwnedMessage<T> > &Incoming() {
      return messagesIn;
    }

  private:
    // incoming messages from server aka In
    ThreadSafeQueue<OwnedMessage<T> > messagesIn;

  protected:
    // asio deals with the data transfer
    boost::asio::io_context context;
    // thread to execute its work cmds
    std::thread threadContext;
    // hardware socket that is connected to server
    tcp::socket socket;
    // client can only have a single instance of a connection to the server
    std::unique_ptr<Connection<T> > connection;
  };

  template<typename T>
  class ServerInterface {
  public:
    ServerInterface(const u16 port): asioAcceptor(asioContext, tcp::endpoint(tcp::v4(), port)) {
    }

    virtual ~ServerInterface() {
      Stop();
    }

    bool Start() {
      try {
        WaitForClientConnection();
        threadContext = std::thread([this]() { asioContext.run(); });
      } catch (std::exception &e) {
        LOG_CRITICAL(Server, "Server exception => {}", e.what());
        return false;
      }
      LOG_INFO(Server, "Server started!");
      return true;
    }

    void Stop() {
      asioContext.stop();

      if (threadContext.joinable()) { threadContext.join(); }

      LOG_INFO(Server, "Server stopped!");
    }

    // ASYNC
    void WaitForClientConnection() {
      asioAcceptor.async_accept(
        [this]([[maybe_unused]] boost::system::error_code errorCode, [[maybe_unused]] tcp::socket socket) {
          if (!errorCode) {
            LOG_INFO(Server, "New connection => {}:{}", socket.remote_endpoint().address().to_string(),
                     socket.remote_endpoint().port());
            std::shared_ptr<Connection<T> > newConnection = std::make_shared<Connection<T> >(
              Connection<T>::owner::server, asioContext, std::move(socket), messagesIn);

            if (OnClientConnect(newConnection)) {
              dequeConnections.push_back(std::move(newConnection));
              dequeConnections.back()->ConnectToClient(idCounter++);

              LOG_INFO(Server, "Connection {} approved", dequeConnections.back()->GetId());
            } else {
              LOG_WARNING(Client, "Connection to server denied");
            }
          } else {
            LOG_CRITICAL(Server, "New connection error => {}", errorCode.message());
          }

          WaitForClientConnection();
        });
    }

    void MessageClient(std::shared_ptr<Connection<T> > client, const Message<T> &message) {
      if (client && client->IsConnected()) {
        client->Send(message);
      } else {
        OnClientDisconnect(client);
        client.reset();
        dequeConnections.erase(
          std::remove(dequeConnections.begin(), dequeConnections.end(), client),
          dequeConnections.end());
      }
    }

    void MessageAllClients(const Message<T> &message, std::shared_ptr<Connection<T> > ignoreClient = nullptr) {
      bool invalidClientExists{false};
      for (auto &client: dequeConnections) {
        if (client && client->IsConnected()) {
          if (client != ignoreClient) {
            client->Send(message);
          }
        } else {
          OnClientDisconnect(client);
          client.reset();
          invalidClientExists = true;
        }
      }
      if (invalidClientExists) {
        dequeConnections.erase(
          std::remove(dequeConnections.begin(), dequeConnections.end(), nullptr),
          dequeConnections.end());
      }
    }

    void Update(const u32 maxMessages = -1) {
      u32 messageCount{0};
      while (messageCount < maxMessages && !messagesIn.Empty()) {
        auto message = messagesIn.PopFront();
        OnMessage(message.remote, message.message);
        messageCount++;
      }
    }

  protected:
    virtual bool OnClientConnect([[maybe_unused]] std::shared_ptr<Connection<T> > client) {
      return false;
    }

    virtual void OnClientDisconnect([[maybe_unused]] std::shared_ptr<Connection<T> > client) {
    }

    virtual void OnMessage([[maybe_unused]] std::shared_ptr<Connection<T> > client,
                           [[maybe_unused]] const Message<T> &message) {
    }

    ThreadSafeQueue<OwnedMessage<T> > messagesIn;

    std::deque<std::shared_ptr<Connection<T> > > dequeConnections;

    boost::asio::io_context asioContext;
    std::thread threadContext;

    tcp::acceptor asioAcceptor;

    [[maybe_unused]] u32 idCounter{10000};
  };
}
