#ifndef TASKCONTROLLER_H
#define TASKCONTROLLER_H

#include <QObject>

#include "taskmodel.h"
#include "taskfilter.h"

class TaskController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int todayDone READ todayDone NOTIFY statsChanged)
    Q_PROPERTY(int weekDone READ weekDone NOTIFY statsChanged)
    Q_PROPERTY(int avgComplTime READ avgComplTime NOTIFY statsChanged)
public:
    explicit TaskController(QObject *parent = nullptr);

    int todayDone() const;
    int weekDone() const;
    double avgComplTime() const;

    Q_INVOKABLE QObject* modelForStatus(const QString &status);

    Q_INVOKABLE void setSearchText(const QString &text);
    Q_INVOKABLE void setTagFilter(const QString &tag);
    Q_INVOKABLE void setStatusFilter(const QString &status);

    Q_INVOKABLE void moveTask(int id, const QString &newStatus);

    Q_INVOKABLE void exportJson();

private:
    TaskModel m_model;
    QMap<QString, TaskFilter*> m_columns;

    QString m_search;
    QString m_tag;
    QString m_status;

signals:
    void statsChanged();
};

#endif // TASKCONTROLLER_H
