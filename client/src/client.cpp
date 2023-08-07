#include "client.h"
#include "packets.h"

ChatClient::ChatClient(const std::string& ip, const std::string& username,
                       const std::string& password) {
  username_ = username;
  password_ = password;
  result_ = Result::Failure;

  ClientConfig config{.server_ip_ = ip, .server_port_ = 25333};
  client_ = CreateRef<Client>(config);
  client_->SetEventCallback(ZNET_BIND_FN(OnEvent));
}

ChatClient::~ChatClient() {
  client_->Disconnect();
  if (thread_ && thread_->joinable()) {
    thread_->join();
  }
}

void ChatClient::Connect() {
  thread_ = CreateRef<std::thread>([this]() {
    if (client_->Bind() != Result::Success) {
      SetResult(Result::CannotBind);
      return;
    }
    SetResult(client_->Connect());
  });
}

void ChatClient::SendChat(const std::string& message) {
  auto session = client_->client_session();
  Ref<MessagePacket> packet = CreateRef<MessagePacket>();
  packet->message_ = message;
  session->SendPacket(packet);
}

bool ChatClient::CanSendMessage() {
  return true;
}

void ChatClient::OnEvent(znet::Event& event) {
  EventDispatcher dispatcher{event};

  dispatcher.Dispatch<ClientConnectedToServerEvent>(
      ZNET_BIND_FN(OnClientConnect));
}

bool ChatClient::OnClientConnect(ClientConnectedToServerEvent& event) {
  SetResult(Result::Success);
  ZNET_LOG_DEBUG("Connection successful!");

  HandlerLayer& handler = event.session()->handler_layer();

  auto login_request_packet = CreateRef<
      PacketHandler<LoginRequestPacket, LoginRequestPacketSerializerV1>>();
  handler.AddPacketHandler(login_request_packet);

  auto login_response_packet = CreateRef<
      PacketHandler<LoginResponsePacket, LoginResponsePacketSerializerV1>>();
  login_response_packet->AddReceiveCallback(
      [this](ConnectionSession& session, Ref<LoginResponsePacket> packet) {
        if (received_login_ ||
            (expected_packet_ != 0 && expected_packet_ != packet->id())) {
          ZNET_LOG_DEBUG(
              "Received packet LoginResponsePacket but it was not expected!");
          session.Close();
          return true;
        }
        if (!packet->succeeded_) {
          ZNET_LOG_DEBUG("Login was not successful!");
          session.Close();
          return true;
        }
        // Update the token
        user_id_ = packet->user_id_;
        expected_packet_ = ServerSettingsPacket::PacketId();
        received_login_ = true;
        return false;
      });
  handler.AddPacketHandler(login_response_packet);

  auto server_settings_packet = CreateRef<
      PacketHandler<ServerSettingsPacket, ServerSettingsPacketSerializerV1>>();
  server_settings_packet->AddReceiveCallback(
      [this](ConnectionSession& session, Ref<ServerSettingsPacket> packet) {
        if (received_settings_ ||
            (expected_packet_ != 0 && expected_packet_ != packet->id())) {
          ZNET_LOG_DEBUG(
              "Received packet ServerSettingsPacket but it was not expected!");
          session.Close();
          return true;
        }

        expected_packet_ = 0;
        received_settings_ = true;
        server_settings_callback_(packet);
        return false;
      });
  handler.AddPacketHandler(server_settings_packet);

  auto message_packet =
      CreateRef<PacketHandler<MessagePacket, MessagePacketSerializerV1>>();
  message_packet->AddReceiveCallback([this](ConnectionSession& session,
                                            Ref<MessagePacket> packet) {
    if (expected_packet_ != 0) {
      ZNET_LOG_DEBUG("Received packet MessagePacket but it was not expected!");
      session.Close();
      return true;
    }

    if (packet->user_id_ == user_id_) {
      return false;
    }
    callback_(packet);
    return false;
  });
  handler.AddPacketHandler(message_packet);

  auto user_connected_packet = CreateRef<
      PacketHandler<UserConnectedPacket, UserConnectedPacketSerializerV1>>();
  user_connected_packet->AddReceiveCallback(
      [this](ConnectionSession& session, Ref<UserConnectedPacket> packet) {
        user_connected_callback_(packet);
        return false;
      });
  handler.AddPacketHandler(user_connected_packet);

  auto user_disconnected_packet =
      CreateRef<PacketHandler<UserDisconnectedPacket,
                              UserDisconnectedPacketSerializerV1>>();
  user_disconnected_packet->AddReceiveCallback(
      [this](ConnectionSession& session, Ref<UserDisconnectedPacket> packet) {
        if (packet->user_id_ == user_id_) {
          return false;
        }
        user_disconnected_callback_(packet);
        return false;
      });
  handler.AddPacketHandler(user_disconnected_packet);

  auto packet = CreateRef<LoginRequestPacket>();
  packet->username_ = username_;
  packet->password_ = password_;

  event.session()->SendPacket(packet);
  expected_packet_ = LoginResponsePacket::PacketId();
  return false;
}

void ChatClient::SetResult(Result result) {
  result_ = result;
  result_callback_(result);
}
