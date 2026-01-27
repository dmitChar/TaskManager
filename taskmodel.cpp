#include "taskmodel.h"

TaskModel::TaskModel(QObject *parent)
    : QAbstractListModel{parent}
{

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
    beginInsertRows({}, tasks.size(), tasks.size());
    tasks.append(newTask);
    endInsertRows();
}

// void TaskModel::moveTask(int id, const QString &newStatus, int targetIndex)
// {
//     for (int i = 0; i < tasks.size(); ++i)
//     {
//         if (tasks[i].id == id)
//         {
//             tasks[i].status = newStatus;
//             if (newStatus == "done")
//             {
//                 tasks[i].finishedAt = QDateTime::currentDateTime();
//             }
//             emit dataChanged(index(i), index(i));
//             return;
//         }
//     }
// }

void TaskModel::moveTask(int id, const QString &newStatus, int sourceTargetIndex)
{
    qDebug() << "TaskModel::moveTask: id=" << id << "newStatus=" << newStatus
             << "sourceTargetIndex=" << sourceTargetIndex;

    // Находим задачу
    int currentSourceIndex = -1;
    for (int i = 0; i < tasks.size(); ++i) {
        if (tasks[i].id == id) {
            currentSourceIndex = i;
            break;
        }
    }

    if (currentSourceIndex == -1) {
        qWarning() << "Task not found:" << id;
        return;
    }

    QString oldStatus = tasks[currentSourceIndex].status;

    qDebug() << "Found task at index" << currentSourceIndex
             << "with status" << oldStatus;



    // Используем beginResetModel для гарантированного обновления
    beginResetModel();

    // Обновляем статус
    tasks[currentSourceIndex].status = newStatus;
    if (newStatus == "done" && oldStatus != "done") {
        tasks[currentSourceIndex].finishedAt = QDateTime::currentDateTime();
    }

    // Извлекаем задачу
    Task temp = tasks.takeAt(currentSourceIndex);

    // Находим позицию для вставки в начало группы задач с новым статусом
    int insertIndex = 0;

    if (sourceTargetIndex >= 0) {
        // Если указан конкретный индекс, используем его
        insertIndex = sourceTargetIndex;
    } else {
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

    // Вставляем задачу
    tasks.insert(insertIndex, temp);

    endResetModel();

    qDebug() << "Task moved from index" << currentSourceIndex
             << "to index" << insertIndex << "(top of" << newStatus << "group)";

    // Выводим текущий порядок для отладки
    qDebug() << "Current tasks order:";
    for (int i = 0; i < qMin(10, tasks.size()); ++i) {
        qDebug() << "  " << i << ":" << tasks[i].id << "-"
                 << tasks[i].title << "(" << tasks[i].status << ")";
    }
}


const QList<Task> &TaskModel::getTasks() const
{
    return tasks;
}
