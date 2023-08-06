#pragma once

#include <thread>
#include <utility>
#include "packets.h"
#include "znet/znet.h"

using namespace znet;

class ChatClient {
 public:
  template <typename T>
  using PacketCallbackFn = std::function<void(Ref<T>)>;
  using ResultCallbackFn = std::function<void(Result)>;

  ChatClient(const std::string& ip, const std::string& username,
             const std::string& password);
  ~ChatClient();

  void Connect();

  void SendMessage(const std::string& message);

  void SetMessageCallback(PacketCallbackFn<MessagePacket> fn) {
    callback_ = std::move(fn);
  }

  void SetUserConnectCallback(PacketCallbackFn<UserConnectedPacket> fn) {
    user_connected_callback_ = std::move(fn);
  }

  void SetUserDisconnectCallback(PacketCallbackFn<UserDisconnectedPacket> fn) {
    user_disconnected_callback_ = std::move(fn);
  }

  void SetServerSettingsCallback(PacketCallbackFn<ServerSettingsPacket> fn) {
    server_settings_callback_ = std::move(fn);
  }

  void SetResultCallback(ResultCallbackFn fn) {
    result_callback_ = std::move(fn);
  }

  const std::string& username() const { return username_; }

  bool CanSendMessage();

  Result GetResult() const { return result_; }

 private:
  void OnEvent(znet::Event& event);
  bool OnClientConnect(ClientConnectedToServerEvent& event);
  void SetResult(Result result);

  std::string username_;
  std::string password_;
  Ref<std::thread> thread_;
  Result result_;

  Ref<Client> client_;
  PacketCallbackFn<MessagePacket> callback_;
  PacketCallbackFn<UserConnectedPacket> user_connected_callback_;
  PacketCallbackFn<UserDisconnectedPacket> user_disconnected_callback_;
  PacketCallbackFn<ServerSettingsPacket> server_settings_callback_;
  ResultCallbackFn result_callback_;
  PacketId expected_packet_ = 0;
  bool received_settings_ = false;
  bool received_login_ = false;
  int user_id_;
};