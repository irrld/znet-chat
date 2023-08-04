//
//    Copyright 2023 Metehan Gezer
//
//    Licensed under the Apache License, Version 2.0 (the "License");
//    you may not use this file except in compliance with the License.
//    You may obtain a copy of the License at
//
//        http://www.apache.org/licenses/LICENSE-2.0
//

#pragma once

#include "znet/znet.h"

using namespace znet;

class LoginRequestPacket : public Packet {
 public:
  LoginRequestPacket() : Packet(PacketId()) {}

  std::string username_;
  std::string password_;

  static PacketId PacketId() { return 1; }
};

class LoginResponsePacket : public Packet {
 public:
  LoginResponsePacket() : Packet(PacketId()) {}

  bool succeeded_ = false;
  int user_id_;

  static PacketId PacketId() { return 2; }
};

class ServerSettingsPacket : public Packet {
 public:
  ServerSettingsPacket() : Packet(PacketId()) {}

  std::unordered_map<int, std::string> user_list_;

  static PacketId PacketId() { return 3; }
};

class MessagePacket : public Packet {
 public:
  MessagePacket() : Packet(PacketId()) {}

  std::string message_;
  std::string sender_username_;
  int user_id_;
  bool is_server_ = false;

  static PacketId PacketId() { return 4; }
};


class UserConnectedPacket : public Packet {
 public:
  UserConnectedPacket() : Packet(PacketId()) {}

  std::string username_;
  int user_id_;

  static PacketId PacketId() { return 5; }
};

class UserDisconnectedPacket : public Packet {
 public:
  UserDisconnectedPacket() : Packet(PacketId()) {}

  int user_id_;

  static PacketId PacketId() { return 6; }
};

class LoginRequestPacketSerializerV1
    : public PacketSerializer<LoginRequestPacket> {
 public:
  LoginRequestPacketSerializerV1() : PacketSerializer<LoginRequestPacket>() {}

  Ref<Buffer> Serialize(Ref<LoginRequestPacket> packet,
                        Ref<Buffer> buffer) override {
    buffer->WriteString(packet->username_);
    buffer->WriteString(packet->password_);
    return buffer;
  }

  Ref<LoginRequestPacket> Deserialize(Ref<Buffer> buffer) override {
    auto packet = CreateRef<LoginRequestPacket>();
    packet->username_ = buffer->ReadString();
    packet->password_ = buffer->ReadString();
    return packet;
  }
};

class LoginResponsePacketSerializerV1
    : public PacketSerializer<LoginResponsePacket> {
 public:
  LoginResponsePacketSerializerV1() : PacketSerializer<LoginResponsePacket>() {}

  Ref<Buffer> Serialize(Ref<LoginResponsePacket> packet,
                        Ref<Buffer> buffer) override {
    buffer->WriteBool(packet->succeeded_);
    buffer->WriteInt(packet->user_id_);
    return buffer;
  }

  Ref<LoginResponsePacket> Deserialize(Ref<Buffer> buffer) override {
    auto packet = CreateRef<LoginResponsePacket>();
    packet->succeeded_ = buffer->ReadBool();
    packet->user_id_ = buffer->ReadInt<int>();
    return packet;
  }
};

class ServerSettingsPacketSerializerV1
    : public PacketSerializer<ServerSettingsPacket> {
 public:
  ServerSettingsPacketSerializerV1()
      : PacketSerializer<ServerSettingsPacket>() {}

  Ref<Buffer> Serialize(Ref<ServerSettingsPacket> packet,
                        Ref<Buffer> buffer) override {
    buffer->WriteMap(packet->user_list_, &Buffer::WriteInt<int>, &Buffer::WriteString);
    return buffer;
  }

  Ref<ServerSettingsPacket> Deserialize(Ref<Buffer> buffer) override {
    auto packet = CreateRef<ServerSettingsPacket>();
    packet->user_list_ = buffer->ReadMap<int, std::string>(&Buffer::ReadInt<int>, &Buffer::ReadString);
    return packet;
  }
};

class MessagePacketSerializerV1 : public PacketSerializer<MessagePacket> {
 public:
  MessagePacketSerializerV1() : PacketSerializer<MessagePacket>() {}

  Ref<Buffer> Serialize(Ref<MessagePacket> packet,
                        Ref<Buffer> buffer) override {
    buffer->WriteString(packet->message_);
    buffer->WriteString(packet->sender_username_);
    buffer->WriteInt(packet->user_id_);
    buffer->WriteBool(packet->is_server_);
    return buffer;
  }

  Ref<MessagePacket> Deserialize(Ref<Buffer> buffer) override {
    auto packet = CreateRef<MessagePacket>();
    packet->message_ = buffer->ReadString();
    packet->sender_username_ = buffer->ReadString();
    packet->user_id_ = buffer->ReadInt<int>();
    packet->is_server_ = buffer->ReadBool();
    return packet;
  }
};

class UserConnectedPacketSerializerV1 : public PacketSerializer<UserConnectedPacket> {
 public:
  UserConnectedPacketSerializerV1() : PacketSerializer<UserConnectedPacket>() {}

  Ref<Buffer> Serialize(Ref<UserConnectedPacket> packet,
                        Ref<Buffer> buffer) override {
    buffer->WriteString(packet->username_);
    buffer->WriteInt(packet->user_id_);
    return buffer;
  }

  Ref<UserConnectedPacket> Deserialize(Ref<Buffer> buffer) override {
    auto packet = CreateRef<UserConnectedPacket>();
    packet->username_ = buffer->ReadString();
    packet->user_id_ = buffer->ReadInt<int>();
    return packet;
  }
};

class UserDisconnectedPacketSerializerV1 : public PacketSerializer<UserDisconnectedPacket> {
 public:
  UserDisconnectedPacketSerializerV1() : PacketSerializer<UserDisconnectedPacket>() {}

  Ref<Buffer> Serialize(Ref<UserDisconnectedPacket> packet,
                        Ref<Buffer> buffer) override {
    buffer->WriteInt(packet->user_id_);
    return buffer;
  }

  Ref<UserDisconnectedPacket> Deserialize(Ref<Buffer> buffer) override {
    auto packet = CreateRef<UserDisconnectedPacket>();
    packet->user_id_ = buffer->ReadInt<int>();
    return packet;
  }
};