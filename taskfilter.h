#ifndef TASKFILTER_H
#define TASKFILTER_H

#include <QSortFilterProxyModel>

class TaskFilter : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit TaskFilter(QObject *parent = nullptr);
    void setStatus(const QString &status);
    void setSearch(const QString &search);
    void setTag(const QString &tag);

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

private:
    QString m_status;
    QString m_search;
    QString m_tag;
};

#endif // TASKFILTER_H
