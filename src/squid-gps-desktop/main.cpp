#include <QApplication>
#include <QQmlApplicationEngine>
#include <spdlog/spdlog.h>

int main(int argc, char** argv) {
  spdlog::set_level(spdlog::level::info);
  QApplication app(argc, argv);
  QQmlApplicationEngine engine;
  engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
  return app.exec();
}
