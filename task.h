

#include <QString>
#include <QStringList>
#include <QDateTime>

struct Task
{
    int id;
    QString title;
    QString description;
    QString status;
    QStringList tags;
    int priority;
    QDateTime createdAt;
    QDateTime finishedAt;
};

