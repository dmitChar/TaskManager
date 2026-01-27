#ifndef TASKFILTER_H
#define TASKFILTER_H

#include <QSortFilterProxyModel>

class TaskFilter : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(QString status READ status WRITE setStatus NOTIFY statusChanged)
    Q_PROPERTY(QString search READ search WRITE setSearch NOTIFY searchChanged)
    Q_PROPERTY(QString tag READ tag WRITE setTag NOTIFY tagChanged)
    Q_PROPERTY(PrioritySortOrder prioritySort READ prioritySort WRITE setPrioritySort NOTIFY prioritySortChanged)
public:
    explicit TaskFilter(QObject *parent = nullptr);

    enum PrioritySortOrder
    {
        NoSort,        // Без сортировки (порядок как в модели)
        HighFirst,     // Высокий приоритет (1 -> 2 -> 3)
        LowFirst       // Низкий приоритет (3 -> 2 -> 1)
    };
    Q_ENUM(PrioritySortOrder)

    QString status() const { return m_status; }
    void setStatus(const QString &status);

    QString search() const { return m_search; }
    void setSearch(const QString &search);

    QString tag() const { return m_tag; }
    void setTag(const QString &tag);

    PrioritySortOrder prioritySort() const { return m_prioritySort; }
    void setPrioritySort(PrioritySortOrder order);
protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;

private:
    QString m_status;
    QString m_search;
    QString m_tag;
    PrioritySortOrder m_prioritySort = NoSort;

signals:
    void statusChanged();
    void searchChanged();
    void tagChanged();
    void prioritySortChanged();
};

#endif // TASKFILTER_H
