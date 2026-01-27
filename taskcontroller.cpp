#include "taskcontroller.h"
#include <QFile>
#include <QTextStream>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

TaskController::TaskController(QObject *parent)
    : QObject{parent}
{
    m_model.addTask({1, "Учить Qt", "QML + C++", "backlog", {"study"}, 1, QDateTime::currentDateTime(), {}});
    m_model.addTask({2, "Проект", "Kanban app", "progress", {"work", "urgent"}, 2, QDateTime::currentDateTime(), {}});
    m_model.addTask({3, "Дизайн", "UI/UX", "progress", {"work"}, 3, QDateTime::currentDateTime(), {}});
    m_model.addTask({4, "Test", "Testing app", "done", {"work", "urgent", "study"}, 3, QDateTime::currentDateTime(), QDateTime::currentDateTime()});
    m_model.addTask({5, "Документация", "Написать README", "backlog", {"work"}, 2, QDateTime::currentDateTime(), {}});

    //Фильтры для каждого статуса
    for (const QString &s: {"backlog","progress","review", "done"})
    {
        auto *proxy = new TaskFilter(this);
        proxy->setSourceModel(&m_model);
        proxy->setStatus(s);
        m_columns[s] = proxy;
    }

}

void TaskController::setPrioritySort(const QString &status, int sortOrder)
{
    TaskFilter *filter = m_columns.value(status);
    if (filter)
    {
        filter->setPrioritySort(static_cast<TaskFilter::PrioritySortOrder>(sortOrder));
        qDebug() << "Priority sort set for" << status << "to" << sortOrder;
    }
    else qWarning() << "Filter not found for status:" << status;
}

int TaskController::getPrioritySort(const QString &status)
{
    TaskFilter *filter = m_columns.value(status);
    if (filter)
    {
        return static_cast<int>(filter->prioritySort());
    }
    return 0;
}

int TaskController::proxyToSourceIndex(TaskFilter *filter, int proxyIndex)
{
    if (!filter || proxyIndex < 0)
        return -1;
    QModelIndex proxyIdx = filter->index(proxyIndex, 0);
    QModelIndex sourceIdx = filter->mapToSource(proxyIdx);
    return sourceIdx.row();

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
    qDebug() << "TaskController::moveTask: id=" << id << "newStatus=" << newStatus;

    // Всегда вставляем в начало (индекс 0 в proxy модели)
    TaskFilter* targetFilter = m_columns.value(newStatus);
    int sourceTargetIndex = -1;

    if (targetFilter && targetFilter->rowCount() > 0) {
        // Конвертируем proxy индекс 0 в source индекс
        sourceTargetIndex = proxyToSourceIndex(targetFilter, 0);
        qDebug() << "Will insert at beginning (proxy 0 = source" << sourceTargetIndex << ")";
    }

    // Вызываем метод модели
    m_model.moveTask(id, newStatus, sourceTargetIndex);
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
