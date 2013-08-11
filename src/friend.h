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

#ifndef FRIEND_H
#define FRIEND_H
#include <QObject>
#include "statuswrapper.h"

class Friend : public QObject
{

    Q_OBJECT
    Q_PROPERTY(QString username READ username NOTIFY usernameChanged)
    Q_PROPERTY(StatusWrapper::Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(QString statusNote READ statusNote NOTIFY statusNoteChanged)
    Q_PROPERTY(QString userId READ userId NOTIFY userIdChanged)
    Q_PROPERTY(QString chatlog READ chatlog WRITE setchatlog NOTIFY chatlogChanged)

public:
    Friend(int id, QObject *parent = 0);

    QString username(){return m_username;}
    StatusWrapper::Status status(){return m_status;}
    QString statusNote(){return m_statusnote;}
    QString userId(){return m_userid;}
    QString chatlog(){return m_chatlog;}

    void setusername(const QString& username);
    void setuserId(const QString& public_key);
private:
    QString m_username;
    StatusWrapper::Status m_status;
    QString m_statusnote;
    QString m_userid;
    QString m_chatlog;

    int m_id; //I dont like you

signals:
    void usernameChanged();
    void statusChanged();
    void statusNoteChanged();
    void userIdChanged();
    void recivedMessage(const QString& message);
    void chatlogChanged();

    void m_sendmessage(int id, const QString &message);

public slots:
    void setstatus(StatusWrapper::Status status);
    void setstatusNote(const QString& note);
    void setchatlog(const QString& data);
    void m_recivedMessage(const QString& message);

    void sendMessage(const QString& message);
    void messageRecived(const QString &message);
};

Q_DECLARE_METATYPE(Friend*)
#endif // FRIEND_H
