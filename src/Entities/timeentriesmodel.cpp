#include "timeentriesmodel.h"
#include <QUuid>
#include <QDebug>
#include <algorithm>

namespace Entities {

TimeEntriesModel::TimeEntriesModel(QObject *parent)
    : QAbstractListModel(parent)
{

}

TimeEntriesModel::~TimeEntriesModel()
{

}

int TimeEntriesModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return count();
}

QVariant TimeEntriesModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() > count())
        return QVariant();

    auto item = m_items[index.row()];
    switch (role) {
    case ItemIdRole:
        return item->id;
    case TaskId:
        return item->taskId;
    case DurationRole:
        return item->duration;
    case StartDateRole:
        return item->startDate;
    case StartTimeRole:
        return item->startTime;
    case EndDateRole:
        return item->endDate;
    case EndTimeRole:
        return item->endTime;
    default:
        return QVariant();
    }
}

bool TimeEntriesModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    auto item = m_items[index.row()];
    switch (role) {
        case TaskId:{
            item->taskId = value.toString();
            emit dataChanged(index, index, QVector<int>() << TaskId);
            return true;
        }
        case DurationRole:{
            item->duration = value.toString();
            emit dataChanged(index, index, QVector<int>() << DurationRole);
            return true;
        }
        case StartDateRole:{
            item->startDate = value.toString();
            emit dataChanged(index, index, QVector<int>() << StartDateRole);
            return true;
        }
        case EndDateRole:{
            item->endDate = value.toString();
            emit dataChanged(index, index, QVector<int>() << EndDateRole);
            return true;
        }
        case StartTimeRole:{
            item->startTime = value.toString();
            emit dataChanged(index, index, QVector<int>() << StartTimeRole);
            return true;
        }
        case EndTimeRole:{
            item->endTime = value.toString();
            emit dataChanged(index, index, QVector<int>() << EndTimeRole);
            return true;
        }
    }

    return false;
}

QHash<int, QByteArray> TimeEntriesModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles.insert(ItemIdRole, "itemId");
    roles.insert(TaskId, "task");
    roles.insert(DurationRole, "duration");
    roles.insert(StartDateRole, "startDate");
    roles.insert(EndDateRole, "endDate");
    roles.insert(StartTimeRole, "startTime");
    roles.insert(EndTimeRole, "endTime");

    return roles;
}

int TimeEntriesModel::count() const
{
    return m_items.size();
}

void TimeEntriesModel::clearModel()
{
    if(!count())
        return;

    beginRemoveRows(QModelIndex(), 0, count() - 1);
    m_items.clear();
    endRemoveRows();

    emit countChanged(count());
}

TimeEntryPtr TimeEntriesModel::getItem(const QString &id)
{
    for (auto item : m_items)
    {
        if (item->id == id)
            return item;
    }
    return nullptr;
}

void TimeEntriesModel::addItem(const QString &id, const QString &taskId, const QString &startDate, const QString &startTime, const QString &duration)
{
    auto newItem = std::make_shared<TimeEntry>();
    newItem->id = id == "" ? QUuid::createUuid().toString()
                           : id;
    newItem->taskId = taskId;
    newItem->startDate = startDate;
    newItem->startTime = startTime;
    newItem->duration = duration;

    addItem(newItem);
}

void TimeEntriesModel::addItem(TimeEntryPtr item)
{
    if (item == nullptr)
        return;
    beginInsertRows(QModelIndex(), count(), count());
    m_items.append(item);
    endInsertRows();

    emit countChanged(count());
}

void TimeEntriesModel::removeItem(const QString &id)
{
    for(int i = 0; i < count(); i++)
    {
        if(m_items[i]->id == id)
            removeItem(i);
    }
}

void TimeEntriesModel::removeItem(const int index)
{
    if(index < 0 || index >= count())
        return;

    beginRemoveRows(QModelIndex(), index, index);
    m_items.removeAt(index);
    endRemoveRows();

    emit countChanged(count());
}

void TimeEntriesModel::sort(int column, Qt::SortOrder order)
{
    Q_UNUSED(column)
    std::sort(m_items.begin(), m_items.end(), [&order](TimeEntryPtr a, TimeEntryPtr b){
        return order == Qt::AscendingOrder ? a->startMSecsSinceEpoch < b->startMSecsSinceEpoch
                                           : a->startMSecsSinceEpoch > b->startMSecsSinceEpoch;
    });
}

int TimeEntriesModel::getIndex(const QString &id)
{
    for(int i = 0; i < count(); i++)
    {
        if(m_items[i]->id == id)
            return i;
    }

    return -1;
}

}

