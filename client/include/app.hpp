
//
//    Copyright 2023 Metehan Gezer
//
//     Licensed under the Apache License, Version 2.0 (the "License");
//     you may not use this file except in compliance with the License.
//     You may obtain a copy of the License at
//
//         http://www.apache.org/licenses/LICENSE-2.0
//

#pragma once

#include "behavior/w_behavior.hpp"
#include "client.h"
#include "events/w_keyevents.hpp"
#include "events/w_mouseevents.hpp"
#include "scene/w_entity.hpp"
#include "scene/w_scene.hpp"
#include "w_application.hpp"
#include "w_pch.hpp"
#include "util/w_color.hpp"

namespace WieselDemo {
class ChatApplication : public Wiesel::Application {
 public:
  ChatApplication();
  ~ChatApplication() override;

  void Init() override;
};

class ChatOverlay : public Wiesel::Layer {
 public:
  explicit ChatOverlay(ChatApplication& app);
  ~ChatOverlay() override;

  void OnAttach() override;
  void OnDetach() override;
  void OnUpdate(float_t deltaTime) override;
  void OnEvent(Wiesel::Event& event) override;

  void OnImGuiRender() override;

  void OnMessage(znet::Ref<MessagePacket> packet);
  void OnUserConnect(znet::Ref<UserConnectedPacket> packet);
  void OnUserDisconnect(znet::Ref<UserDisconnectedPacket> packet);
  void OnServerSettings(znet::Ref<ServerSettingsPacket> packet);

  void OpenLink(const std::string& url);
 private:
  ChatApplication& app_;
  znet::Ref<ChatClient> client_;
  std::string warning_;
  bool warning_showed_ = false;
  std::mutex mutex_;
  bool cleanup_ = false;
  struct Message {
    Wiesel::Colorf prefix_color_;
    std::string prefix_;
    Wiesel::Colorf text_color_;
    std::string text_;
  };
  std::vector<Message> messages_;
  std::unordered_map<int, std::string> users_;
  std::string chat_;
  std::string ip_;
  std::string username_;
  bool focus_input_;
  bool auto_scroll_ = true;
  bool scroll_to_bottom_ = true;
};
}  // namespace WieselDemo