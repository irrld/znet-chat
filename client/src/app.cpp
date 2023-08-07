
//
//    Copyright 2023 Metehan Gezer
//
//     Licensed under the Apache License, Version 2.0 (the "License");
//     you may not use this file except in compliance with the License.
//     You may obtain a copy of the License at
//
//         http://www.apache.org/licenses/LICENSE-2.0
//

#include "app.hpp"
#include "input/w_input.hpp"
#include "layer/w_layerimgui.hpp"
#include "scene/w_componentutil.hpp"
#include "script/lua/w_luabehavior.hpp"
#include "script/lua/w_scriptglue.hpp"
#include "systems/w_canvas_system.hpp"
#include "util/w_keycodes.hpp"
#include "util/w_math.hpp"
#include "w_engine.hpp"
#include "w_entrypoint.hpp"

#include <random>
#include "theme.h"

using namespace Wiesel;

namespace WieselDemo {

ChatOverlay::ChatOverlay(ChatApplication& app)
    : app_(app), Layer("Demo Overlay") {}

ChatOverlay::~ChatOverlay() = default;

void ChatOverlay::OnAttach() {
  LOG_DEBUG("OnAttach");
  Engine::GetRenderer()->SetVsync(true);
  ImGui::StyleColorsDark(&ImGui::GetStyle());
  ImGui::SetupImGuiStyle();
}

void ChatOverlay::OnDetach() {
  LOG_DEBUG("OnDetach");
}

void ChatOverlay::OnUpdate(float_t deltaTime) {}

void ChatOverlay::OnEvent(Wiesel::Event& event) {}

void ChatOverlay::OnImGuiRender() {
  std::scoped_lock lock(mutex_);
  ImGuiViewport* viewport = ImGui::GetMainViewport();
  ImGui::SetNextWindowPos(viewport->WorkPos);
  ImGui::SetNextWindowSize(viewport->WorkSize);
  ImGui::SetNextWindowViewport(viewport->ID);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
  ImGui::Begin("Main", nullptr,
               ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize |
                   ImGuiWindowFlags_NoBringToFrontOnFocus |
                   ImGuiWindowFlags_NoMove);
  ImGui::PopStyleVar(1);
  if (cleanup_) {
    cleanup_ = false;
    client_ = nullptr;
  }
  if (client_) {
    ImGui::Begin("Chat", nullptr, ImGuiWindowFlags_NoCollapse);
    for (const auto& item : messages_) {
      ImGui::TextWrapped("%s", item.c_str());
    }
    static std::string chat;
    // Get remaining vertical space available in the current window
    ImVec2 contentRegionAvail = ImGui::GetContentRegionAvail();

    // Set the cursor position to the bottom of the available space
    float inputTextHeight = ImGui::GetTextLineHeightWithSpacing();
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + contentRegionAvail.y -
                         inputTextHeight);
    float inputTextWidth =
        ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x -
        ImGui::CalcTextSize("Send").x - ImGui::GetStyle().WindowPadding.x;

    ImGui::PushItemWidth(inputTextWidth);
    bool send = false;
    ImGui::InputText("##", &chat, ImGuiInputTextFlags_None);
    ImGui::PopItemWidth();
    ImGui::SameLine();
    if (ImGui::Button("Send") || send) {
      client_->SendMessage(chat);
      messages_.push_back(client_->username() + ": " + chat);
      chat = "";
    }
    ImGui::End();

    ImGui::Begin("Users", nullptr, ImGuiWindowFlags_NoCollapse);
    for (const auto& [key, value] : users_) {
      // ImGui will crash if a user has empty username.
      if (value.empty())
        continue;
      ImGui::Selectable(value.c_str());
    }
    ImGui::End();
  } else {
    ImGui::Begin("Connect", nullptr, ImGuiWindowFlags_NoCollapse);
    static std::string ip;
    static std::string username;
    ImGui::InputText(PrefixLabel("IP").c_str(), &ip);
    ImGui::InputText(PrefixLabel("Username").c_str(), &username);
    if (ImGui::Button("Connect")) {
      if (ip.empty() || username.empty()) {
        warning_ = "IP or username cannot be empty.";
        warning_showed_ = true;
      } else {
        cleanup_ = false;
        client_ = CreateRef<ChatClient>(ip, username, "");
        client_->SetMessageCallback(ZNET_BIND_FN(OnMessage));
        client_->SetUserConnectCallback(ZNET_BIND_FN(OnUserConnect));
        client_->SetUserDisconnectCallback(ZNET_BIND_FN(OnUserDisconnect));
        client_->SetServerSettingsCallback(ZNET_BIND_FN(OnServerSettings));
        client_->SetResultCallback([this](Result result) {
          std::scoped_lock lock(mutex_);
          if (result == Result::Completed) {
            cleanup_ = true;
          } else if (result != Result::Success) {
            warning_ = std::to_string(
                static_cast<std::underlying_type_t<Result>>(result));
            warning_showed_ = true;
            cleanup_ = true;
          }
        });
        client_->Connect();
      }
    }
    ImGui::End();
  }
  if (!warning_.empty() && warning_showed_) {
    if (ImGui::Begin("Warning", &warning_showed_,
                     ImGuiWindowFlags_NoCollapse)) {
      ImGui::Text("%s", warning_.c_str());
      ImGui::End();
    }
  }
  ImGui::End();
}

void ChatOverlay::OnMessage(znet::Ref<MessagePacket> packet) {
  std::scoped_lock lock(mutex_);
  messages_.push_back(packet->sender_username_ + ": " + packet->message_);
}

void ChatOverlay::OnUserConnect(znet::Ref<UserConnectedPacket> packet) {
  users_[packet->user_id_] = packet->username_;
}

void ChatOverlay::OnUserDisconnect(znet::Ref<UserDisconnectedPacket> packet) {
  users_.erase(packet->user_id_);
}

void ChatOverlay::OnServerSettings(znet::Ref<ServerSettingsPacket> packet) {
  users_ = packet->user_list_;
}

void ChatApplication::Init() {
  LOG_DEBUG("Init");
  PushOverlay(CreateReference<ChatOverlay>(*this));
}

ChatApplication::ChatApplication() : Application({"Wiesel Demo"}) {
  LOG_DEBUG("DemoApp constructor");
}

ChatApplication::~ChatApplication() {
  LOG_DEBUG("DemoApp destructor");
}
}  // namespace WieselDemo

// Called from entrypoint
Application* Wiesel::CreateApp() {
  return new WieselDemo::ChatApplication();
}