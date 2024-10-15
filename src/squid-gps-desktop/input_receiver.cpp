#include "input_receiver.h"

#include <spdlog/spdlog.h>

InputReceiver::InputReceiver(QSettings& settings, QObject* parent):
 QObject(parent), m_settings_ref(settings) { }

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
