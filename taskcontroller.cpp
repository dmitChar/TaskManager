#include "taskcontroller.h"
#include <QFile>
#include <QDir>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDateTime>

TaskController::TaskController(QObject *parent)
    : QObject{parent}
{

    m_model.addTask({-1, "Изучить QML basics", "Прочитать доки", "done", {"study"}, 2, QDateTime(QDate(2026,1,20), QTime(10,0)), QDateTime(QDate(2026,1,22), QTime(15,30)) });
    m_model.addTask({-1, "Сделать прототип доски", "Kanban layout", "done", {"work", "urgent"}, 1, QDateTime(QDate(2026,1,25), QTime(9,0)),  QDateTime(QDate(2026,1,28), QTime(18,0))  });
    m_model.addTask({-1, "Тестирование drag&drop", "Проверить перемещение", "done", {"work", "study"},2,QDateTime(QDate(2026,1,30), QTime(14,0)), QDateTime(QDate(2026,2,1),  QTime(11,45)) });
    m_model.addTask({-1, "Добавить статистику", "Графики в QML","done", {"work"},1, QDateTime(QDate(2026,2,2),  QTime(8,30)),  QDateTime(QDate(2026,2,3),  QTime(16,20)) });
    m_model.addTask({-1, "Рефакторинг модели задач","Оптимизация TaskModel","done", {"study"},3, QDateTime(QDate(2026,2,5), QTime(10,0)),  QDateTime(QDate(2026,2,6),  QTime(14,10)) });

    m_model.addTask({-1, "Добавить авторизацию","Логин/регистрация","progress", {"work", "urgent"}, 1, QDateTime(QDate(2026,2,1), QTime(9,0)),  {}});
    m_model.addTask({-1, "Написать README", "Документация проекта", "backlog",  {"work"},2, QDateTime(QDate(2026,2,3), QTime(11,0)), {}});
    m_model.addTask({-1, "Тёмная тема", "Поддержка dark mode","progress", {"study"},3, QDateTime(QDate(2026,2,4), QTime(13,0)), {}});
    m_model.addTask({-1, "Экспорт в CSV/JSON","Сохранение отчётов", "backlog",  {"work"},2, QDateTime(QDate(2026,2,5), QTime(15,0)), {}});

    m_model.addTask({-1, "Первая версия прототипа", "MVP Kanban","done", {"work"},1, QDateTime(QDate(2025,12,10), QTime(9,0)), QDateTime(QDate(2025,12,15), QTime(17,0)) });

    //Фильтры для каждого статуса
    for (const QString &s: {"backlog","progress","review", "done"})
    {
        auto *proxy = new TaskFilter(this);
        proxy->setSourceModel(&m_model);
        proxy->setStatus(s);
        m_columns[s] = proxy;
    }
}

void TaskController::deleteTaskById(int id)
{
    if (!m_model.deleteTask(id))
        qDebug() << "Ошибка удаления задачи" << id;
    else
        qDebug() << "Задача" << id << "была успешно удалена";
}


QVariantMap TaskController::getTask(int id) const
{
    QVariantMap taskData{};

    for (const auto & t: m_model.getTasks())
    {
        if (t.id == id)
        {
            taskData["id"] = t.id;
            taskData["title"] = t.title;
            taskData["description"] = t.description;
            taskData["status"] = t.status;
            taskData["priority"] = t.priority;
            taskData["tags"] = t.tags;
            return taskData;
        }
    }
    qDebug() << "Task was not found by id:" << id;
    return taskData;
}

void TaskController::addTask(const QString &title, const QString &description, const QStringList &tags, int priority)
{
    if (title.trimmed().isEmpty())
        return;
    Task task;
    task.id = -1;
    task.title = title;
    task.description = description;
    task.tags = tags;
    task.priority = priority;
    task.createdAt = QDateTime::currentDateTime();
    task.status = "backlog";

    m_model.addTask(task);
}

void TaskController::updateTask(int id,const QString &status, const QString &title, const QString &description, const QStringList &tags, int priority)
{
    qDebug() << "TaskController::updateTask:";
    qDebug() << "  proxyIndex:" << id;
    qDebug() << "  status:" << status;

    TaskFilter* filter = m_columns.value(status);
    if (!filter)
    {
        qWarning() << "Filter not found for status:" << status;
        return;
    }

    QModelIndex proxyIndx = filter->index(id, 0);
    int taskId = filter->data(proxyIndx, TaskModel::IdRole).toInt();
    qDebug() << "  Converted proxyIndex" << id << "to taskId:" << taskId;

    m_model.updateTask(id, title, description, tags, priority);
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
        qDebug() << "Will insert at beginning (" << sourceTargetIndex << ")";
    }

    m_model.moveTask(id, newStatus, sourceTargetIndex);
    emit statsChanged();
}

void TaskController::exportJson()
{
    QString filePath = QDir::currentPath() + "/saves";
    QDir path(filePath);

    if (!path.exists())
    {
        if (QDir().mkpath(filePath))
            qDebug() << "Создана директория для сохранений, путь:" << filePath;
        else
        {
            qDebug() << "Ошибка создания директории для сохранений";
            return;
        }
    }
    else qDebug() << "Открыт каталог для сохранений";

    QJsonArray arr;
    for (const Task &t : m_model.getTasks())
    {
        QJsonObject obj;
        obj["title"] = t.title;
        obj["description"] = t.description;
        obj["status"] = t.status;
        obj["tags"] = QJsonArray::fromStringList(t.tags);
        obj["priority"] = t.priority;
        obj["createdAt"] = t.createdAt.toString(Qt::ISODate);
        obj["finishedAt"] = t.finishedAt.toString(Qt::ISODate);

        arr.append(obj);
    }

    filePath += "/";
    filePath += "task_" + QDateTime::currentDateTime().toString("dd-MM-yyyy_HH:mm:ss");
    filePath += ".json";
    QFile f(filePath);

    if (!f.open(QIODevice::WriteOnly))
    {
        qDebug() << "Не удалось открыть файл для записи сохранения";
        return;
    }
    f.write(QJsonDocument(arr).toJson(QJsonDocument::Indented));
    qDebug() << "Сохранения записаны в файл" << filePath;
    f.close();
}

//Статистика
QVariantList TaskController::getCompletedTasks(Period period) const
{
    qDebug() << "start of getCompltasks";
    QDateTime from = QDateTime(QDate::currentDate(), QTime(0, 0, 0));
    QDateTime to = QDateTime::currentDateTime();
    QString format;

    switch (period)
    {
    case Period::Today:
        format = "%H";
        break;

    case Period::Week:
        from = from.addDays(-7);
        format = "%d-%m";
        break;

    case Period::Month:
        from = QDateTime(QDate(to.date().year(), to.date().month(), 1), QTime(0, 0, 0));
        format = "%d-%m";
        break;

    case Period::Year:
        from = QDateTime(QDate(to.date().year(), 1, 1), QTime(0, 0, 0));
        format = "%m";
        break;
    }

    return DbManager::instance().getCountTasks(format, from, to);
}

QVariantList TaskController::getTagsCount(Period period) const
{
    qDebug() << "Start of getTagsCount";
    QDateTime from = QDateTime(QDate::currentDate(), QTime(0, 0, 0));
    QDateTime to = QDateTime::currentDateTime();

    switch (period)
    {
    case Period::Today:
        break;

    case Period::Week:
        from = from.addDays(-7);
        break;

    case Period::Month:
        from = QDateTime(QDate(to.date().year(), to.date().month(), 1), QTime(0, 0, 0));
        break;

    case Period::Year:
        from = QDateTime(QDate(to.date().year(), 1, 1), QTime(0, 0, 0));
        break;
    }

        return DbManager::instance().getTagsCount(from);
}

