#include <QGuiApplication>
#include <QQmlApplicationEngine>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    //qmlRegisterType<Count>("count", 1, 0, "Cou");
    //const QUrl url("qrc:/firstProgramm/main.qml"); // ссылка на файл qml
    //engine.load(url);   // загружаем файл qml
    return app.exec();
}
