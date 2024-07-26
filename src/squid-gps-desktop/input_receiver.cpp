#include "input_receiver.h"

#include <spdlog/spdlog.h>

QString InputReceiver::error_message() const {
    return m_error_message;
}

void InputReceiver::set_error_message(const QString& message) {
  if (!message.isEmpty()) {
    spdlog::error("{}", message.toStdString());
  }
  if (m_error_message != message) {
    m_error_message = message;
    emit errorMessageChanged();
  }
}
