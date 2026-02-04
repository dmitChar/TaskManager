#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QVector>
#include "task.h"

class DbManager : public QObject
{
    Q_OBJECT
public:
    static DbManager& instance();
    bool initialize();
    bool isOpen() const;

    //CRUD операции
    int addTask(Task &task);
    bool updateTask(const Task& task);
    bool deleteTask(int taskId);
    Task getTask(int taskId);
    QList<Task> getAllTasks();
    //QVector<Task> getTasksByStatus(const QString& status);

    //Операции с тегами
    bool addTag(const QString& name, const QString& color = "#95a5a6");
    bool deleteTag(int tagId);
    QStringList getAllTagNames();

    //Связь тегов с задачами
    bool addTagToTask(int taskId, const QString &tagName);
    QStringList getTaskTags(int taskId);

    QList<Task> getTasksByStatus(const QString& status);

    QVariantList getCountTasks(const QString &grorupFormat, const QDateTime &from, const QDateTime &to);

private:
    DbManager();
    ~DbManager();
    DbManager(const DbManager&) = delete;
    DbManager& operator=(const DbManager&) = delete;

    bool createTables();

    QSqlDatabase m_db;
signals:
};

#endif // DBMANAGER_H
