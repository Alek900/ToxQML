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
    m_user = new Friend(-1);

    connect(m_core, &Core::onfriendAdded, this, &CoreModel::onfriendAdded);
    connect(m_core, &Core::onfriendRequested, this, &CoreModel::onfriendRequest);
    connect(m_core, &Core::onfriendMessaged, this, &CoreModel::onfriendMessage);
    connect(m_core, &Core::onfriendNameChanged, this, &CoreModel::onfriendNameChanged);
    connect(m_core, &Core::onfriendStatusTextChanged, this, &CoreModel::onfriendStatusNoteChanged);

    connect(m_core, &Core::onStarted, this, &CoreModel::coreStarted);
}

void CoreModel::onfriendAdded(int friendnumber, const QString &key)
{
    Friend *newfriend = new Friend(friendnumber);
    newfriend->setuserId(key);
    newfriend->setusername("???");
    newfriend->setstatus(Friend::Status::Offline);
    m_friendlist.append(newfriend);
    m_friendmap[friendnumber] = newfriend;
    connect(newfriend, &Friend::m_sendmessage, this, &CoreModel::sendFriendMessage);

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
}

void CoreModel::setuserStatusnote(const QString &note)
{
    m_core->setuserStatusnote(note);
}

void CoreModel::coreStarted()
{
    m_user->setuserId(m_core->userId());
    m_user->setusername(m_core->username());
}