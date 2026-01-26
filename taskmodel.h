#ifndef TASKMODEL_H
#define TASKMODEL_H

#include <QObject>
#include <QAbstractListModel>

#include "task.h"

class TaskModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit TaskModel(QObject *parent = nullptr);

    enum Roles
    {
        IdRole = Qt::UserRole + 1,
        TitleRole,
        DescriptionRole,
        StatusRole,
        TagsRole,
        PriorityRole
    };

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role)  const override;
    QHash<int, QByteArray> roleNames() const override;

    void addTask(const Task &newTask);
    void moveTask(int id, const QString &newStatus);

    QList<Task> &getTasks();
private:
    QList<Task> tasks;
signals:
};

#endif // TASKMODEL_H
