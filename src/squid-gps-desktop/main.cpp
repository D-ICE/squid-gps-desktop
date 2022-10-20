#include <QApplication>
#include <QQmlApplicationEngine>
#include <spdlog/spdlog.h>

// Asio needs a call to CoInitialize on windows
#ifdef CO_INITIALIZE
#include <windows.h>
void Initialize() {
  CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
}
#else  // CO_INITIALIZE
void Initialize() {}
#endif  // CO_INITIALIZE

int main(int argc, char** argv) {
  Initialize();
  spdlog::set_level(spdlog::level::info);
  QApplication app(argc, argv);
  QQmlApplicationEngine engine;
  engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
  return app.exec();
}
