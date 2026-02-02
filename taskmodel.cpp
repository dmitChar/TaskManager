#include "taskmodel.h"

TaskModel::TaskModel(QObject *parent)
    : QAbstractListModel{parent}
{
    DbManager::instance().initialize();
    tasks = loadFromDatabase();
}

QList<Task> TaskModel::loadFromDatabase()
{
    beginResetModel();
    QList<Task> t = DbManager::instance().getAllTasks();
    endResetModel();
    return t;
}

int TaskModel::rowCount(const QModelIndex &) const
{
    return tasks.size();
}

QVariant TaskModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return {};
    const auto &m_task = tasks[index.row()];

    switch (role)
    {
        case IdRole: return m_task.id;
        case TitleRole: return m_task.title;
        case DescriptionRole: return m_task.description;
        case StatusRole: return m_task.status;
        case TagsRole:
        {
            QVariantList list;
            for (const auto & tag : m_task.tags)
                list.append(tag);
            return list;
        }
        case PriorityRole: return m_task.priority;
    }
    return {};
}

QHash<int, QByteArray> TaskModel::roleNames() const
{
    return
    {
        {IdRole, "id"},
        {TitleRole, "title"},
        {DescriptionRole, "description"},
        {StatusRole, "status"},
        {TagsRole, "tags"},
        {PriorityRole, "priority"}
    };
}

void TaskModel::addTask(const Task &newTask)
{
    Task task = newTask;
    task.id = DbManager::instance().addTask(task);
    qDebug() << "id = " << task.id;
    if (task.id != -1)
    {
        beginInsertRows({}, tasks.size(), tasks.size());
        tasks.append(task);
        endInsertRows();
    }
}

bool TaskModel::deleteTask(int id)
{
    int taskIndex = -1;
    for (size_t i = 0; i < tasks.size(); ++i)
    {
        if (tasks[i].id == id)
        {
            taskIndex = i;
            break;
        }
    }
    if (taskIndex == -1)
    {
        qWarning() << "Task not found in model with ID:" << id;
        return false;
    }

    if (!DbManager::instance().deleteTask(id)) {
        qWarning() << "Failed to delete task from database";
        return false;
    }
    beginRemoveRows({}, taskIndex, taskIndex);
    tasks.removeAt(taskIndex);
    endRemoveRows();
    return true;

}

void TaskModel::updateTask(int id, const QString &title, const QString &description, const QStringList &tags, int priority)
{
    qDebug() << "TaskModel::updateTask called for ID:" << id;
    qDebug() << "  title:" << title;
    qDebug() << "  description:" << description;
    qDebug() << "  tags:" << tags;
    qDebug() << "  priority:" << priority;

    int taskIndex = -1;
    for (size_t i = 0; i < tasks.size(); ++i)
    {
        if (tasks[i].id == id)
        {
            taskIndex = i;
            break;
        }
    }

    if (taskIndex == -1)
    {
        qWarning() << "Task not found in model with ID:" << id;
        return;
    }

    tasks[taskIndex].title = title;
    tasks[taskIndex].description = description;
    tasks[taskIndex].tags = tags;
    tasks[taskIndex].priority = priority;

    // Обновляем в БД
    if (!DbManager::instance().updateTask(tasks[taskIndex]))
    {
        qWarning() << "Failed to update task in database";
        return;
    }

    QModelIndex changedIndex = index(taskIndex);
    emit dataChanged(changedIndex, changedIndex, QVector<int>() << TitleRole << DescriptionRole << TagsRole << PriorityRole);

    qDebug() << "Task updated successfully in model and database";
}

void TaskModel::moveTask(int id, const QString &newStatus, int indexToPlace)
{
    qDebug() << "TaskModel::moveTask: id=" << id << "newStatus=" << newStatus << "indexToPlace=" << indexToPlace;

    // Находим задачу
    int currentIndexInModel = -1;
    for (int i = 0; i < tasks.size(); ++i) {
        if (tasks[i].id == id) {
            currentIndexInModel = i;
            break;
        }
    }

    if (currentIndexInModel == -1) {
        qWarning() << "Task not found:" << id;
        return;
    }

    QString oldStatus = tasks[currentIndexInModel].status;

    qDebug() << "Found task at index" << currentIndexInModel << "with status" << oldStatus;

    beginRemoveRows({}, currentIndexInModel, currentIndexInModel);

    // Обновляем статус
    tasks[currentIndexInModel].status = newStatus;
    if (newStatus == "done" && oldStatus != "done") {
        tasks[currentIndexInModel].finishedAt = QDateTime::currentDateTime();
    }
    // Извлекаем задачу
    Task temp = tasks.takeAt(currentIndexInModel);
    endRemoveRows();

    // Находим позицию для вставки в начало группы задач с новым статусом
    int insertIndex = 0;

    if (indexToPlace >= 0) {
        // Если указан конкретный индекс, используем его
        insertIndex = indexToPlace;
    }
    else
    {
        // Иначе ищем первую задачу с таким же статусом
        for (int i = 0; i < tasks.size(); ++i) {
            if (tasks[i].status == newStatus) {
                insertIndex = i;
                break;
            }
        }
        // Если задач с таким статусом нет, ищем правильное место по порядку статусов
        if (insertIndex == 0) {
            QStringList statusOrder = {"backlog", "progress", "review", "done"};
            int newStatusPos = statusOrder.indexOf(newStatus);

            for (int i = 0; i < tasks.size(); ++i) {
                int currentStatusPos = statusOrder.indexOf(tasks[i].status);
                if (currentStatusPos > newStatusPos) {
                    insertIndex = i;
                    break;
                }
                insertIndex = i + 1;
            }
        }
    }

    beginInsertRows({}, insertIndex, insertIndex);
    // Вставляем задачу
    tasks.insert(insertIndex, temp);
    endInsertRows();

    DbManager::instance().updateTask(temp);

    qDebug() << "Task moved from index" << currentIndexInModel << "to index" << insertIndex << "(top of" << newStatus << "group)" << "description:" << temp.description;
    // Выводим текущий порядок для отладки
    qDebug() << "Current tasks order:";
    for (int i = 0; i < qMin(10, tasks.size()); ++i)
        qDebug() << "  " << i << ":" << tasks[i].id << "-" << tasks[i].title << "(" << tasks[i].status << ")";
}


const QList<Task> &TaskModel::getTasks() const
{
    return tasks;
}
