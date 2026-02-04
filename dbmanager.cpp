#include "dbmanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QStandardPaths>
#include <QDir>
#include <QFile>

DbManager::DbManager()
{}

DbManager& DbManager::instance()
{
    static DbManager instance;
    return instance;
}

DbManager::~DbManager()
{
    if (m_db.isOpen())
        m_db.close();
}

bool DbManager::initialize()
{
    QString path = QDir::currentPath();
    QDir dir(path);
    if (!dir.exists())
        dir.mkpath(".");

    QString dbFile = path + "/taskManager.db";
    qDebug() << "Database path:" << dbFile;

    if (!QSqlDatabase::contains("TaskManagerDB"))
    {
        m_db = QSqlDatabase::addDatabase("QSQLITE", "TaskManagerDB");
    }
    else
    {
        m_db = QSqlDatabase::database("TaskManagerDB");
    }
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName("TaskManager.db");

    if (!m_db.open())
    {
        qDebug() << "Failed to open db" << m_db.lastError().text();
        return false;
    }

    QSqlQuery query(m_db);
    query.exec("PRAGMA foreign_keys = ON");

    if (!createTables())
        return false;

    qDebug() << "Database initialized successfully";
    return true;
}

bool DbManager::isOpen() const
{
    return m_db.isOpen();
}

bool DbManager::createTables()
{
    QSqlQuery query(m_db);

    //Таблица задач
    if (!query.exec(
            R"(CREATE TABLE IF NOT EXISTS tasks
            (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                title TEXT UNIQUE NOT NULL,
                description TEXT,
                status TEXT NOT NULL DEFAULT 'backlog',
                priority INTEGER NOT NULL DEFAULT 0,
                created_at DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
                finished_at DATETIME,
                updated_at DATETIME DEFAULT CURRENT_TIMESTAMP,

                CHECK (status IN ('backlog', 'progress', 'review', 'done')),
                CHECK (priority IN (0, 1, 2, 3))

            ))"
        ))
    {
        qDebug() << "Failed to create table TASKS" << query.lastError().text();
        return false;
    }

    //Таблица тегов
    if (!query.exec(
            R"(CREATE TABLE IF NOT EXISTS tags
            (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                name TEXT UNIQUE NOT NULL,
                color TEXT DEFAULT '#95a5a6'
            ))"
        ))
    {
        qDebug() << "Failed to create table TAGS" << query.lastError().text();
        return false;
    }

    //Таблица связей задач и тегов
    if (!query.exec(
            R"(CREATE TABLE IF NOT EXISTS task_tags
            (
            task_id INTEGER NOT NULL,
            tag_id INTEGER NOT NULL,

            PRIMARY KEY (task_id, tag_id),
            FOREIGN KEY (task_id) REFERENCES tasks(id) ON DELETE CASCADE,
            FOREIGN KEY (tag_id) REFERENCES tags(id) ON DELETE CASCADE
            ))"
        ))
    {
        qDebug() << "Failed to create table TASK_TAGS" << query.lastError().text();
        return false;
    }

    //Индексы
    query.exec("CREATE INDEX IF NOT EXISTS idx_tasks_status_date ON tasks(status, created_at DESC)");
    query.exec("CREATE INDEX IF NOT EXISTS idx_tasks_created ON tasks(created_at DESC)");
    query.exec("CREATE INDEX IF NOT EXISTS idx_task_tags_task ON task_tags(task_id)");
    query.exec("CREATE INDEX IF NOT EXISTS idx_task_tags_tag ON task_tags(tag_id)");

    //Триггер для updated_at
    query.exec(R"(
        CREATE TRIGGER IF NOT EXISTS update_task_timestamp
        AFTER UPDATE ON tasks
        BEGIN
            UPDATE tasks SET updated_at = CURRENT_TIMESTAMP WHERE id = NEW.id;
        END
    )");

    // Добавляем стандартные теги
    query.exec("INSERT OR IGNORE INTO tags (name, color) VALUES ('work', '#3498db')");
    query.exec("INSERT OR IGNORE INTO tags (name, color) VALUES ('study', '#2ecc71')");
    query.exec("INSERT OR IGNORE INTO tags (name, color) VALUES ('urgent', '#e74c3c')");

    return true;
}

int DbManager::addTask(Task &task)
{
    QSqlQuery query(m_db);

    query.prepare(R"(
        INSERT INTO tasks (title, description, status, priority, created_at, finished_at)
        VALUES (:title, :description, :status, :priority, :created_at, :finished_at)
    )");

    query.bindValue(":title", task.title);
    query.bindValue(":description", task.description);
    query.bindValue(":status", task.status);
    query.bindValue(":priority", task.priority);
    query.bindValue(":created_at", task.createdAt);
    query.bindValue(":finished_at", task.finishedAt.isValid() ? task.finishedAt : QVariant());

    if (!query.exec())
    {
        qDebug() << "Failed to insert task: " + query.lastError().text();
        qDebug() << "Last query: " << query.lastQuery();
        return -1;
    }

    task.id = query.lastInsertId().toInt();
    for (const QString &tag : task.tags)
        addTagToTask(task.id, tag);

    qDebug() << "Task added with ID:" << task.id;
    return task.id;
}

bool DbManager::updateTask(const Task &task)
{
    QSqlQuery query(m_db);
    query.prepare(R"(
        UPDATE OR IGNORE tasks SET
        title = :title,
        description = :description,
        status = :status,
        priority = :priority,
        finished_at = :finished_at
        WHERE id = :id )");
    query.bindValue(":id", task.id);
    query.bindValue(":title", task.title);
    query.bindValue(":description", task.description);
    query.bindValue(":status", task.status);
    query.bindValue(":priority", task.priority);
    query.bindValue(":finished_at", task.finishedAt.isValid() ? task.finishedAt : QVariant());

    if (!query.exec())
    {
        qDebug()<< "Failed to update task: " + query.lastError().text();
        return false;
    }
    qDebug() << "desc = " << task.description;
    // Обновляем теги
    query.prepare("DELETE FROM task_tags WHERE task_id = :id");
    query.bindValue(":id", task.id);
    query.exec();

    for (const QString& tagName : task.tags)
        addTagToTask(task.id, tagName);

    return true;
}

bool DbManager::deleteTask(int taskId)
{
    QSqlQuery query(m_db);
    query.prepare(R"(DELETE FROM TASKS WHERE id = :id)");
    query.bindValue(":id", taskId);

    if (!query.exec()) {
        qDebug() << "Failed to delete task:" << query.lastError().text();
        return false;
    }

    qDebug() << "Task deleted from database, ID:" << taskId;
    return true;
}

bool DbManager::deleteTag(int tagId)
{

}

bool DbManager::addTag(const QString& name, const QString& color)
{
    QSqlQuery query(m_db);
    query.prepare(R"(INSERT OR IGNORE INTO tags (name, color) VALUES(:name, :color))");
    query.bindValue(":name", name);
    query.bindValue(":color", color);
    if (!query.exec())
    {
        qDebug() << query.lastError().text();
        return false;
    }
    return true;
}

bool DbManager::addTagToTask(int taskId, const QString &tagName)
{
    QSqlQuery query(m_db);
    query.prepare(R"(SELECT id FROM tags WHERE name = :name)");
    query.bindValue(":name", tagName);

    //Находим id тега
    int tagId = -1;
    if (query.exec() && query.next())
    {
        tagId = query.value(0).toInt();
    }
    else //Создаем новый тег
    {
        query.prepare(R"(INSERT INTO tags (name) VALUES (:name))");
        query.bindValue(":name", tagName);
        if (query.exec())
            tagId = query.lastInsertId().toInt();
    }
    if (tagId == -1)
    {
        qDebug() << "addTagToTask error, tagId wasnt found";
        return false;
    }
    query.prepare(R"(INSERT OR IGNORE INTO task_tags (task_id, tag_id) VALUES(:task_id, :tag_id))");
    query.bindValue(":task_id", taskId);
    query.bindValue(":tag_id", tagId);

    return query.exec();
}

Task DbManager::getTask(int taskId)
{

}

QList<Task> DbManager::getAllTasks()
{
    QList<Task> tasks;
    QSqlQuery query(m_db);

    // Сортировка по статусу и дате создания
    if (!query.exec("SELECT * FROM tasks ORDER BY status, created_at DESC"))
    {
        qDebug() << "Failed to get all tasks: " + query.lastError().text();
        return tasks;
    }

    while (query.next())
    {
        Task task;
        task.id = query.value("id").toInt();
        task.title = query.value("title").toString();
        task.description = query.value("description").toString();
        task.status = query.value("status").toString();
        task.priority = query.value("priority").toInt();
        task.createdAt = query.value("created_at").toDateTime();
        task.finishedAt = query.value("finished_at").toDateTime();
        task.tags = getTaskTags(task.id);

        tasks.append(task);
    }

    return tasks;
}

QStringList DbManager::getTaskTags(int taskId)
{
    QSqlQuery query(m_db);
    QStringList tags{};
    query.prepare(R"(SELECT t.name FROM tags t JOIN task_tags tt ON t.id = tt.tag_id WHERE tt.task_id = :task_id)");
    query.bindValue(":task_id", taskId);

    if (query.exec())
    {
        while (query.next())
            tags.append(query.value(0).toString());
    }

    return tags;
}

QStringList DbManager::getAllTagNames()
{

}

QList<Task> DbManager::getTasksByStatus(const QString& status)
{
    QVector<Task> tasks;
    QSqlQuery query(m_db);

    query.prepare("SELECT * FROM tasks WHERE status = :status ORDER BY created_at DESC");
    query.bindValue(":status", status);

    if (!query.exec()) {
        qDebug() << "Failed to get tasks by status: " + query.lastError().text();
        return tasks;
    }

    while (query.next())
    {
        Task task;
        task.id = query.value("id").toInt();
        task.title = query.value("title").toString();
        task.description = query.value("description").toString();
        task.status = query.value("status").toString();
        task.priority = query.value("priority").toInt();
        task.createdAt = query.value("created_at").toDateTime();
        task.finishedAt = query.value("finished_at").toDateTime();
        task.tags = getTaskTags(task.id);

        tasks.append(task);
    }

    return tasks;
}


//Статистика
//Количество выполненных заданий за период
QVariantList DbManager::getCountTasks(const QString &groupFormat, const QDateTime &from, const QDateTime &to)
{
    QVariantList result;
    QSqlQuery query(m_db);
    query.prepare(R"(
        SELECT strftime(:groupFormat, finished_at) AS label, COUNT(*) AS value
        FROM tasks WHERE finished_at BETWEEN :from AND :to
        GROUP BY label
        ORDER BY MIN(finished_at)
    )");
    query.bindValue(":groupFormat", groupFormat);
    query.bindValue(":from", from);
    query.bindValue(":to", to);
    if (query.exec())
    {
        while (query.next())
        {
            QVariantMap point;
            point["label"] = query.value("label").toString();
            point["value"] = query.value("value").toInt();
            result.append(point);
        }
    }
    else
    {
        qDebug() << "Ошибка в SELECT count tasks:" << query.lastError().text();
    }
    return result;
}
