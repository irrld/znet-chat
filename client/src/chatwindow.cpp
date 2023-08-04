#include "chatwindow.h"

ChatWindow::ChatWindow(const QString& ip, const QString& username, const QString& password, QWidget *parent) : QMainWindow(parent) {
  setWindowTitle("Znet Chat Application");
  resize(500, 400);

  // Chat History
  chat = new ChatWidget(this);

  // Input Field
  inputField = new QLineEdit(this);
  connect(inputField, &QLineEdit::returnPressed, this, &ChatWindow::onSendButtonClicked);

  // Send Button
  sendButton = new QPushButton("Send", this);
  connect(sendButton, &QPushButton::clicked, this, &ChatWindow::onSendButtonClicked);

  // Other setup as before
  userList = new UserListWidget(this); // Create the UserListWidget

  // Layout
  auto* chatLayout = new QVBoxLayout();
  chatLayout->addWidget(chat);
  chatLayout->addWidget(inputField);

  auto* userListLayout = new QVBoxLayout();
  userListLayout->addWidget(userList);
  userListLayout->addStretch(); // Add stretch to push user list to the top

  auto* splitter = new QSplitter(Qt::Horizontal, this);
  splitter->addWidget(chat);
  splitter->addWidget(userList);
  int splitterWidth = width(); // Current width of the splitter
  int chatWidth = splitterWidth * 0.7; // 70% of the width
  int userListWidth = splitterWidth - chatWidth; // Remaining 30% of the width
  splitter->setSizes({chatWidth, userListWidth});

  auto* inputLayout = new QHBoxLayout();
  inputLayout->addWidget(inputField);
  inputLayout->addWidget(sendButton);

  auto* mainLayout = new QVBoxLayout();
  mainLayout->addWidget(splitter);
  mainLayout->addLayout(inputLayout);

  auto* centralWidget = new QWidget(this);
  centralWidget->setLayout(mainLayout);
  setCentralWidget(centralWidget);

  client = std::make_shared<ChatClient>(ip.toStdString(), username.toStdString(), password.toStdString());
  client->SetCallback([this, username](Ref<MessagePacket> packet) {
    std::scoped_lock<std::mutex> lock(mutex);
    if (packet->is_server_) {
      chat->addSystem(QString::fromStdString(packet->message_));
    } else {
      chat->addMessage(QString::fromStdString(packet->sender_username_), QString::fromStdString(packet->message_));
    }
  });
  client->SetCallback([this](Ref<UserConnectedPacket> packet) {
    std::scoped_lock<std::mutex> lock(mutex);
    userList->addUser(packet->user_id_, QString::fromStdString(packet->username_));
  });
  client->SetCallback([this](Ref<UserDisconnectedPacket> packet) {
    std::scoped_lock<std::mutex> lock(mutex);
    userList->removeUser(packet->user_id_);
  });
  client->SetCallback([this](Ref<ServerSettingsPacket> packet) {
    std::scoped_lock<std::mutex> lock(mutex);
    for (const auto& [key, value] : packet->user_list_) {
        userList->addUser(key, QString::fromStdString(value));
    }
  });
}

ChatWindow::~ChatWindow() {
}

void ChatWindow::onSendButtonClicked() {
  if (!client->CanSendMessage()) {
    return;
  }
  QString message = inputField->text();
  if (!message.isEmpty()) {
    std::scoped_lock<std::mutex> lock(mutex);
    client->SendMessage(message.toStdString());
    chat->addMessage(QString::fromStdString(client->username()), message);
  }
  inputField->clear();
}

