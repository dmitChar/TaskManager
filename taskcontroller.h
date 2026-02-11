#ifndef TASKCONTROLLER_H
#define TASKCONTROLLER_H

#include <QObject>

#include "taskmodel.h"
#include "taskfilter.h"
#include "dbmanager.h"

class TaskController : public QObject
{
    Q_OBJECT
    // Q_PROPERTY(int todayDone READ todayDone NOTIFY statsChanged)
    // Q_PROPERTY(int weekDone READ weekDone NOTIFY statsChanged)
    // Q_PROPERTY(int avgComplTime READ avgComplTime NOTIFY statsChanged)


public:
    enum Period
    {
        Today,
        Week,
        Month,
        Year
    };
    Q_ENUM(Period)
    explicit TaskController(QObject *parent = nullptr);

    //Получение модели для стутуса
    Q_INVOKABLE QObject* modelForStatus(const QString &status);

    //Фильтрация
    Q_INVOKABLE void setSearchText(const QString &text);
    Q_INVOKABLE void setTagFilter(const QString &tag);
    Q_INVOKABLE void setStatusFilter(const QString &status);

    // Методы для управления сортировкой
    Q_INVOKABLE void setPrioritySort(const QString& status, int sortOrder);
    Q_INVOKABLE int getPrioritySort(const QString& status);

    //Метод перемещения задачи из одной колонки в другую
    Q_INVOKABLE void moveTask(int id, const QString &newStatus);

    //Экспорт
    Q_INVOKABLE void exportJson();

    //Добавление новой задачи
    Q_INVOKABLE void addTask(const QString &title, const QString &description, const QStringList &tags, int priority);
    //Обновление задачи
    Q_INVOKABLE void updateTask(int id, const QString &status, const QString &title, const QString &description, const QStringList &tags, int priority);

    //Получение задачи по id
    Q_INVOKABLE QVariantMap getTask(int id) const;

    //Удаление задачи
    Q_INVOKABLE void deleteTaskById(int id);

    //Статистика
    Q_INVOKABLE QVariantList getCompletedTasks(Period period) const;
    Q_INVOKABLE QVariantList getTagsCount(Period period) const;

private:
    TaskModel m_model;
    QMap<QString, TaskFilter*> m_columns;
    QString m_search;
    QString m_tag;
    QString m_status;

    //Вспомогательный метод для конвертации индексов
    int proxyToSourceIndex(TaskFilter* filter, int proxyIndex);

signals:
    void statsChanged();
};

#endif // TASKCONTROLLER_H
