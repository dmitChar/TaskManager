#include "taskcontroller.h"
#include <QFile>
#include <QTextStream>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

TaskController::TaskController(QObject *parent)
    : QObject{parent}
{
    m_model.addTask({1,"Учить Qt","QML + C++","backlog",{"study"},1,QDateTime::currentDateTime(),{}});
    m_model.addTask({2,"Проект","Kanban app","progress",{"work","urgent"},2,QDateTime::currentDateTime(),{}});

    for (const QString &s: {"backlog","progress","done"})
    {
        auto *proxy = new TaskFilter(this);
        proxy->setSourceModel(&m_model);
        proxy->setStatus(s);
        m_columns[s] = proxy;
    }

}

QObject *TaskController::modelForStatus(const QString &status)
{
    return m_columns.value(status);
}

void TaskController::setSearchText(const QString &text)
{
    m_search = text;
    for (auto *column : m_columns)
        column->setSearch(text);
}

void TaskController::setTagFilter(const QString &tag)
{
    m_tag = tag;
    for (auto *column : m_columns)
        column->setTag(tag);
}

void TaskController::setStatusFilter(const QString &status)
{
    m_status = status;
    for (auto *column : m_columns)
        column->setStatus(status);
}

void TaskController::moveTask(int id, const QString &newStatus)
{
    m_model.moveTask(id, newStatus);
    emit statsChanged();
}

void TaskController::exportJson()
{
    QJsonArray arr;
    for (const Task &t : m_model.getTasks())
    {
        QJsonObject obj;
        obj["title"] = t.title;

    }
}

//Статистика
//За сегодня
int TaskController::todayDone() const
{
    int count = 0;
    QDate today = QDate::currentDate();

    for (const Task &t : m_model.getTasks())
    {
        if (t.status == "done" && t.finishedAt.date() == today)
            ++count;
    }
    return count;
}

//За неделю
int TaskController::weekDone() const
{
    int count = 0;
    QDateTime weekAgo = QDateTime::currentDateTime().addDays(-7);

    for (const Task &t : m_model.getTasks())
    {
        if (t.status == "done" && t.finishedAt.isValid() && t.finishedAt >= weekAgo)
            ++count;
    }
    return count;
}

//Ср время выполенения
double TaskController::avgComplTime() const
{
    qint64 totalSecs = 0;
    int doneCount = 0;

    for (const Task &t : m_model.getTasks())
    {
        if (t.status == "done" && t.finishedAt.isValid())
        {
            totalSecs += t.createdAt.secsTo(t.createdAt);
            ++doneCount;
        }
    }

    if (doneCount == 0) return 0.0;
    return totalSecs / doneCount / 3600; // возвращаем время в часах
}
