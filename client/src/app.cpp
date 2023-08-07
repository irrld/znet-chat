
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
  ImGui::Text("zchat by Metehan \"irrelevantdev\" Gezer");
  if (ImGui::Button("znet-chat: https://github.com/irrld/znet-chat")) {
    OpenLink("https://github.com/irrld/znet-chat");
  }
  if (ImGui::Button("znet: https://github.com/irrld/znet")) {
    OpenLink("https://github.com/irrld/znet-chat");
  }
  if (ImGui::Button("wiesel: https://github.com/irrld/wiesel")) {
    OpenLink("https://github.com/irrld/wiesel");
  }
  ImGui::PopStyleVar(1);
  if (cleanup_) {
    cleanup_ = false;
    client_ = nullptr;
  }
  if (client_) {
    ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_FirstUseEver);
    ImGui::Begin("Chat");
    // Reserve enough left-over height for 1 separator + 1 input text
    const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
    if (ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar)) {
      for (int i = 0; i < messages_.size(); i++) {
        auto& item = messages_[i];
        ImVec4 prefix_color{item.prefix_color_.Red, item.prefix_color_.Green, item.prefix_color_.Blue, item.prefix_color_.Alpha};
        ImVec4 text_color{item.text_color_.Red, item.text_color_.Green, item.text_color_.Blue, item.text_color_.Alpha};
        ImGui::PushStyleColor(ImGuiCol_Text, prefix_color);
        ImGui::TextWrapped("%s:", item.prefix_.c_str());
        ImGui::PopStyleColor();
        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Text, text_color);
        ImGui::TextWrapped("%s", item.text_.c_str());
        ImGui::PopStyleColor();
      }

      // Keep up at the bottom of the scroll region if we were already at the bottom at the beginning of the frame.
      // Using a scrollbar or mouse-wheel will take away from the bottom edge.
      if (scroll_to_bottom_ || (auto_scroll_ && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
        ImGui::SetScrollHereY(1.0f);
      scroll_to_bottom_ = false;
    }
    ImGui::EndChild();
    ImGui::Separator();

    ImVec2 contentRegionAvail = ImGui::GetContentRegionAvail();
    // Set the cursor position to the bottom of the available space
    float inputTextHeight = ImGui::GetTextLineHeightWithSpacing();
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + contentRegionAvail.y -
                         inputTextHeight - ImGui::GetStyle().SeparatorTextPadding.y + ImGui::GetStyle().WindowPadding.x);
    static const int kSendButtonWidth = 80;
    float inputTextWidth =
        ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x -
        kSendButtonWidth - ImGui::GetStyle().WindowPadding.x;

    ImGui::PushItemWidth(inputTextWidth);
    bool send = false;
    if (focus_input_) {
      ImGui::SetKeyboardFocusHere();
      focus_input_ = false;
    }
    if (ImGui::InputText("##", &chat_, ImGuiInputTextFlags_None | ImGuiInputTextFlags_EnterReturnsTrue)) {
      send = true;
      focus_input_ = true;
    }
    ImGui::SetItemDefaultFocus();
    ImGui::PopItemWidth();
    ImGui::SameLine();
    bool disable = chat_.empty();
    if (disable) {
      ImGui::BeginDisabled();
    }
    if (ImGui::Button("Send", ImVec2(kSendButtonWidth, 0))) {
      send = true;
      focus_input_ = true;
    }
    if (disable) {
      ImGui::EndDisabled();
      send = false;
    }
    if (send) {
      client_->SendChat(chat_);
      Message message{
          .prefix_ = client_->username(),
          .prefix_color_ = {0.8f, 0.8f, 0.8f, 1.0f},
          .text_ = chat_,
          .text_color_ = {1.0f, 1.0f, 1.0f, 1.0f}
      };
      messages_.push_back(message);

      chat_ = "";
    }

    ImGui::End();

/*

    ImGui::Begin("Chat", nullptr, ImGuiWindowFlags_NoCollapse);
    // Get remaining vertical space available in the current window
    ImGui::BeginGroup();
    for (const auto& item : messages_) {
      ImGui::TextWrapped("%s", item.c_str());
    }

    ImGui::EndGroup();
    ImGui::End();*/

    ImGui::Begin("Users", nullptr, ImGuiWindowFlags_NoCollapse);
    for (const auto& [key, value] : users_) {
      // ImGui will crash if a user has empty username_.
      if (value.empty())
        continue;
      ImGui::Selectable(value.c_str());
    }
    ImGui::End();
  } else {
    ImGui::SetNextWindowPos(ImVec2((viewport->WorkSize.x - 300) / 2, (viewport->WorkSize.y - 120) / 2));
    ImGui::SetNextWindowSize(ImVec2(300, 120));
    ImGui::Begin("Connect", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
    ImGui::InputText(PrefixLabel("IP").c_str(), &ip_);
    ImGui::InputText(PrefixLabel("Username").c_str(), &username_);
    if (ImGui::Button("Connect")) {
      if (ip_.empty() || username_.empty()) {
        warning_ = "IP or username_ cannot be empty.";
        warning_showed_ = true;
      } else {
        cleanup_ = false;
        client_ = CreateRef<ChatClient>(ip_, username_, "");
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
    ImGui::SetNextWindowPos(ImVec2((viewport->WorkSize.x - 400) / 2, (viewport->WorkSize.y - 150) / 2));
    ImGui::SetNextWindowSize(ImVec2(400, 150));
    if (ImGui::Begin("Warning", &warning_showed_,
                     ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove)) {
      ImGui::Text("%s", warning_.c_str());
      ImGui::End();
    }
  }
  ImGui::End();
}

void ChatOverlay::OnMessage(znet::Ref<MessagePacket> packet) {
  std::scoped_lock lock(mutex_);
  Message message{
      .prefix_ = packet->sender_username_,
      .prefix_color_ = {1.0f, 1.0f, 1.0f, 1.0f},
      .text_ = packet->message_,
      .text_color_ = {1.0f, 1.0f, 1.0f, 1.0f}
  };
  messages_.push_back(message);
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

void ChatOverlay::OpenLink(const std::string& url) {
#ifdef _WIN32
    // Note: executable path must use  backslashes!
    ::ShellExecuteA(NULL, "open", url.c_str(), NULL, NULL, SW_SHOWDEFAULT);
#else
#if __APPLE__
    const char* open_executable = "open";
#else
    const char* open_executable = "xdg open";
#endif
  char command[256];
    snprintf(command, 256, "%s \"%s\"", open_executable, url.c_str());
    system(command);
#endif
}

void ChatApplication::Init() {
  LOG_DEBUG("Init");
  PushOverlay(CreateReference<ChatOverlay>(*this));
}

ChatApplication::ChatApplication() : Application({"Chat App"}) {
}

ChatApplication::~ChatApplication() {
}
}  // namespace WieselDemo

// Called from entrypoint
Application* Wiesel::CreateApp() {
  return new WieselDemo::ChatApplication();
}