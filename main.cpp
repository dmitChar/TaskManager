#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "taskcontroller.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    TaskController controller;
    engine.rootContext()->setContextProperty("taskController", &controller);
    const QUrl url("qrc:/qml/main.qml");
    engine.load(url);
    return app.exec();
}
