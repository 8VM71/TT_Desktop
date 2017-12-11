#include "workspacesmodel.h"
#include <QUuid>

namespace Enteties {

WorkspacesModel::WorkspacesModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_count(0)
{

}

WorkspacesModel::~WorkspacesModel()
{

}

int WorkspacesModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_items.count();
}

QVariant WorkspacesModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() > count())
        return QVariant();

    auto item = m_items[index.row()];
    switch (role) {
    case ItemIdRole:
        return item->id;
    case NameRole:
        return item->name;
    case OwnerRole:
        return item->ownerId;
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> WorkspacesModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles.insert(ItemIdRole, "itemId");
    roles.insert(NameRole, "name");
    roles.insert(OwnerRole, "owner");

    return roles;
}

int WorkspacesModel::count() const
{
    return m_count;
}

void WorkspacesModel::clearModel()
{
    if(!count())
        return;

    beginRemoveRows(QModelIndex(), 0, count() - 1);
    m_items.clear();
    endRemoveRows();

    emit countChanged(count());
}

WorkspacePtr WorkspacesModel::getItem(const QString &id)
{
    for (auto item : m_items)
    {
        if (item->id == id)
            return item;
    }
    return nullptr;
}

void WorkspacesModel::addItem(const QString &id, const QString &name, const QString &ownerId)
{
    auto newItem = std::make_shared<Workspace>();
    newItem->id = id == "" ? QUuid::createUuid().toString()
                           : id;
    newItem->name = name;
    newItem->ownerId = ownerId;

    beginInsertRows(QModelIndex(), count(), count());
    m_items.append(newItem);
    endInsertRows();
}

void WorkspacesModel::removeItem(const QString &id)
{
    for(int i = 0; i < count(); i++)
    {
        if(m_items[i]->id == id)
            removeItem(i);
    }
}

void WorkspacesModel::removeItem(const int index)
{
    if(index < 0 || index >= count())
            return;

    beginRemoveRows(QModelIndex(), index, index);
    m_items.removeAt(index);
    endRemoveRows();

    emit countChanged(count());
}

}
