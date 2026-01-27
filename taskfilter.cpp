#include "taskfilter.h"
#include "taskmodel.h"

TaskFilter::TaskFilter(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    setDynamicSortFilter(true);
}

void TaskFilter::setStatus(const QString &status)
{
    beginFilterChange();
    m_status = status;
    endFilterChange(QSortFilterProxyModel::Direction::Rows);
    emit statusChanged();
}

void TaskFilter::setSearch(const QString &search)
{
    beginFilterChange();
    m_search = search;
    endFilterChange(QSortFilterProxyModel::Direction::Rows);
    emit searchChanged();
}

void TaskFilter::setTag(const QString &tag)
{
    beginFilterChange();
    m_tag = tag;
    endFilterChange(QSortFilterProxyModel::Direction::Rows);
    emit tagChanged();
}

void TaskFilter::setPrioritySort(PrioritySortOrder order)
{
    if (m_prioritySort == order)
        return;

    m_prioritySort = order;

    if (m_prioritySort == NoSort)
    {
        sort(-1);
    }
    else if (m_prioritySort == HighFirst)
    {
        sort(0, Qt::AscendingOrder);
    }
    else if (m_prioritySort == LowFirst)
    {
        sort(0, Qt::DescendingOrder);
    }

    //emit prioritySortChanged();
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

bool TaskFilter::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    if (m_prioritySort == NoSort)
    {
        // Без сортировки - сохраняем исходный порядок
        return left.row() < right.row();
    }

    int leftPriority = left.data(TaskModel::PriorityRole).toInt();
    int rightPriority = right.data(TaskModel::PriorityRole).toInt();

    // Если приоритеты не указаны, считаем их самыми низкими
    if (leftPriority == 0) leftPriority = 99;
    if (rightPriority == 0) rightPriority = 99;

    if (leftPriority != rightPriority)
    {
        return leftPriority < rightPriority;
    }

    // Если приоритеты одинаковые, сохраняем исходный порядок
    return left.row() < right.row();

}
