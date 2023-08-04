#pragma once

#include <utility>
#include <thread>
#include "znet/znet.h"
#include "packets.h"

using namespace znet;

class ChatClient {
 public:
  template <typename T>
  using PacketCallbackFn = std::function<void(Ref<T>)>;

  ChatClient(const std::string& ip, const std::string& username, const std::string& password);
  ~ChatClient();

  void SendMessage(const std::string& message);
  void SetCallback(PacketCallbackFn<MessagePacket> fn) { callback_ = std::move(fn); }
  void SetCallback(PacketCallbackFn<UserConnectedPacket> fn) { user_connected_callback_ = std::move(fn); }
  void SetCallback(PacketCallbackFn<UserDisconnectedPacket> fn) { user_disconnected_callback_ = std::move(fn); }
  void SetCallback(PacketCallbackFn<ServerSettingsPacket> fn) { server_settings_callback_ = std::move(fn); }

  const std::string& username() const { return username_; }

  bool CanSendMessage();
 private:
  void OnEvent(znet::Event& event);
  bool OnClientConnect(ClientConnectedToServerEvent& event);

  std::string username_;
  std::string password_;
  Ref<std::thread> thread_;

  Ref<Client> client_;
  PacketCallbackFn<MessagePacket> callback_;
  PacketCallbackFn<UserConnectedPacket> user_connected_callback_;
  PacketCallbackFn<UserDisconnectedPacket> user_disconnected_callback_;
  PacketCallbackFn<ServerSettingsPacket> server_settings_callback_;
  PacketId expected_packet_ = 0;
  bool received_settings_ = false;
  bool received_login_ = false;
  int user_id_;



};