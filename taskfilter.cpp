#include "taskfilter.h"
#include "taskmodel.h"

TaskFilter::TaskFilter(QObject *parent)
    : QSortFilterProxyModel(parent)
{

}

void TaskFilter::setStatus(const QString &status)
{
    beginFilterChange();
    m_status = status;
    endFilterChange(QSortFilterProxyModel::Direction::Rows);

}

void TaskFilter::setSearch(const QString &search)
{
    beginFilterChange();
    m_search = search;
    endFilterChange(QSortFilterProxyModel::Direction::Rows);
}

void TaskFilter::setTag(const QString &tag)
{
    beginFilterChange();
    m_tag = tag;
    endFilterChange(QSortFilterProxyModel::Direction::Rows);
}

bool TaskFilter::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QModelIndex idx = sourceModel()->index(sourceRow, 0, sourceParent);

    QString title = idx.data(TaskModel::TitleRole).toString();
    QString status = idx.data(TaskModel::StatusRole).toString();
    QStringList tags = idx.data(TaskModel::TagsRole).toStringList();

    if (!m_search.isEmpty() && !title.contains(m_search, Qt::CaseInsensitive))
        return false;
    if (!m_status.isEmpty() && m_status != "all" && status != m_status)
        return false;
    if (!m_tag.isEmpty() && m_tag != "all" && !tags.contains(m_tag))
        return false;
    return true;
}
