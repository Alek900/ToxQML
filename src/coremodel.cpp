/*
    A QML GUI for tox

    Copyright (C) 2013  Alek900 (alek900@gmail.com)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see {http://www.gnu.org/licenses/}.
*/

#include "coremodel.h"


Request::Request(QObject *parent) : QObject(parent)
{}

CoreModel::CoreModel(Core *core, QObject *parent) :
    QObject(parent)
{
    m_core = core;
    m_user = new UserItem(-1);
    m_user->setusername("Hello");
    m_user->setstatus(StatusWrapper::Online);
    connect(m_core, &Core::onfriendAdded, this, &CoreModel::onfriendAdded);
    connect(m_core, &Core::onfriendRequested, this, &CoreModel::onfriendRequest);
    connect(m_core, &Core::onfriendMessaged, this, &CoreModel::onfriendMessage);
    connect(m_core, &Core::onfriendNameChanged, this, &CoreModel::onfriendNameChanged);
    connect(m_core, &Core::onfriendStatusChanged, this, &CoreModel::onfriendStatusChanged);
    connect(m_core, &Core::onfriendStatusTextChanged, this, &CoreModel::onfriendStatusNoteChanged);

    connect(m_core, &Core::connectedChanged, this, &CoreModel::onConnectedChanged);

    connect(m_core, &Core::onStarted, this, &CoreModel::coreStarted);
}

void CoreModel::onConnectedChanged()
{
    m_connected = m_core->connected();
    emit connectedChanged();
}

void CoreModel::onfriendAdded(int friendnumber, const QString &key)
{
    UserItem *newfriend = new UserItem(friendnumber);
    newfriend->setuserId(key);
    newfriend->setusername("???");
    newfriend->setstatus(StatusWrapper::Status::Offline);
    m_friendlist.append(newfriend);
    m_friendmap[friendnumber] = newfriend;
    connect(newfriend, &UserItem::m_sendmessage, this, &CoreModel::sendFriendMessage);
    connect(newfriend, &UserItem::deleteFriend, this, &CoreModel::onfriendDelete);

    emit friendsChanged();
}

void CoreModel::onfriendDelete(int friendnumber)
{
    UserItem* tmp = m_friendmap[friendnumber];
    m_friendlist.removeAll(tmp);
    m_friendmap.remove(friendnumber);
    delete tmp;
    m_core->deleteFriend(friendnumber);
    emit friendsChanged();
}

void CoreModel::onfriendRequest(const QString& key, const QString& message)
{
    Request *newrequest = new Request(this);
    newrequest->m_key = key;
    newrequest->m_message = message;

    m_friendrequests.append(newrequest);
    emit friendRequest(newrequest);
    emit requestsChanged();
}

void CoreModel::onfriendMessage(int friendnumber, const QString& message)
{
    m_friendmap[friendnumber]->messageRecived(message);
}

void CoreModel::onfriendNameChanged(int friendnumber, const QString& name)
{
    m_friendmap[friendnumber]->setusername(name);
}

void CoreModel::onfriendStatusChanged(int friendnumber, TOX_USERSTATUS status)
{
    m_friendmap[friendnumber]->setstatus((StatusWrapper::Status) status);
}

void CoreModel::onfriendStatusNoteChanged(int friendnumber, const QString& note)
{
    m_friendmap[friendnumber]->setstatusNote(note);
}

void CoreModel::acceptFriendRequest(Request *newfriend)
{
    m_core->acceptFriendRequest(newfriend->m_key);
    m_friendrequests.removeAll(newfriend);
    emit requestsChanged();
}

void CoreModel::sendFriendrequest(const QString &key, const QString &message)
{
    m_core->sendFriendRequest(key, message);
}

void CoreModel::sendFriendMessage(int id, const QString &message)
{
    m_core->sendFriendMessge(id, message);
}

void CoreModel::setuserUsername(const QString &name)
{
    m_core->setuserUsername(name);
    m_user->setusername(name);
    emit userChanged();
}

void CoreModel::setuserStatusnote(const QString &note)
{
    m_core->setuserStatusnote(note);
    m_user->setstatusNote(note);
    emit userChanged();
}

void CoreModel::coreStarted()
{
    m_user->setuserId(m_core->userId());
    m_user->setusername(m_core->username());
}
