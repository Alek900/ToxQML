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

#include "user.h"

UserItem::UserItem(int id, QObject *parent) : QObject(parent)
{
    m_id = id;
}

void UserItem::setusername(const QString &username)
{
    m_username = username;
    emit usernameChanged();
}

void UserItem::setuserId(const QString &public_key)
{
    m_userid = public_key;
    emit userIdChanged();
}

void UserItem::setstatus(StatusWrapper::Status status)
{
    m_status = status;
    emit statusChanged();
}

void UserItem::setstatusNote(const QString &note)
{
    m_statusnote = note;
    emit statusNoteChanged();
}

void UserItem::setchatlog(const QString& data)
{
    m_chatlog = data;
    emit chatlogChanged();
}

void UserItem::m_recivedMessage(const QString &message)
{
    emit recivedMessage(message);
}

void UserItem::sendMessage(const QString &message)
{
    emit m_sendmessage(m_id, message);
    m_chatlog.append("You: ");
    m_chatlog.append(message);
    m_chatlog.append("\n"); //this is kinda ugly
    emit chatlogChanged();
}

void UserItem::messageRecived(const QString &message)
{
    m_chatlog.append(username() + ": ");
    m_chatlog.append(message);
    m_chatlog.append("\n");
    emit chatlogChanged();
}

void UserItem::deleteMe()
{
    emit deleteFriend(m_id);
}
