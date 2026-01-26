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
        case TagsRole: return m_task.tags;
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

void TaskModel::moveTask(int id, const QString &newStatus)
{
    for (int i = 0; i < tasks.size(); ++i)
    {
        if (tasks[i].id == id)
        {
            tasks[i].status = newStatus;
            if (newStatus == "done")
            {
                tasks[i].finishedAt = QDateTime::currentDateTime();
            }
            emit dataChanged(index(i), index(i));
        }
    }
}

const QList<Task> &TaskModel::getTasks() const
{
    return tasks;
}
