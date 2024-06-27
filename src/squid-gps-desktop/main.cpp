#include <QApplication>
#include <QQmlApplicationEngine>
#include <QIcon>
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

  QGuiApplication app(argc, argv);

  QIcon::setFallbackSearchPaths({"qrc:/qt/qml/Backend/fonts"});
  QIcon::setThemeName(".");

  QQmlApplicationEngine engine;
  QObject::connect(
      &engine,
      &QQmlApplicationEngine::objectCreationFailed,
      &app,
      []() { QCoreApplication::exit(-1); },
      Qt::QueuedConnection);

  engine.loadFromModule("Backend", "Main");

  return app.exec();
}
