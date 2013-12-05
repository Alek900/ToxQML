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

//A class for making tox.h more QT friendly
#ifndef CORE_H
#define CORE_H

#include <QObject>
#include <QTimer>
#include "tox.h"
//#include "Messenger.h"
#include "DHT.h"

class Core : public QObject
{
    Q_OBJECT

    //Shouldn't really use Q_PROPERTY on the core, not very usfull really
    /*Q_PROPERTY(bool connected READ connected NOTIFY connectedChanged)
    Q_PROPERTY(QString userId READ userId)
    Q_PROPERTY(QString username READ username)*/

public:
    explicit Core(QObject *parent = 0);
    QString userId();
    QString username();
    bool connected(){return m_connected;}

private:

    struct cString{
        uint8_t *data;
        uint16_t size;
    };

    QTimer *eventtimer;
    bool m_connected;
    QString m_id;

    Tox *m_tox;

    static QString toQString(uint8_t *data, uint16_t length = -1);
    static cString fromQString(const QString& string);

    static void m_friendrequest(uint8_t *public_key, uint8_t  *data, uint16_t length, void *userdata);
    static void m_friendmessage(Tox *m, int friendnumber, uint8_t *message, uint16_t length, void *userdata);
    static void m_friendnamechange(Tox *m, int friendnumber, uint8_t *newname, uint16_t length, void *userdata);
    static void m_frienduserstatuschange(Tox *m, int friendnumber,TOX_USERSTATUS kind, void *userdata);
    static void m_friendstatusnotechange(Tox *m, int friendnumber, uint8_t *status, uint16_t length, void *userdata);
    static void m_friendstatuschange(Tox *m, int friendnumber, uint8_t status, void* userdata);

    static void m_groupinvite(Tox *tox, int friendnumber, uint8_t *group_public_key, void *userdata);

    static void m_groupmessage(Tox *tox, int groupnumber, int friendgroupnumber,
                               uint8_t * message, uint16_t length, void *userdata);

    static void m_groupnamelistchanged(Tox *tox, int groupnumber,
                                       int peernumber, TOX_CHAT_CHANGE change, void *userdata);



    void m_checkdhtconnection();
    void addDHTServer(const QString& id, const QString ip, int port);

signals:

    void onStarted();
    void connectedChanged();

    void onfriendAdded(int friendnumber, const QString& public_key);

    //Friends
    void onfriendRequested(const QString& public_key, const QString& message);
    void onfriendMessaged(int friendnumber, const QString& message);
    void onfriendNameChanged(int friendnumber, const QString& name);
    void onfriendStatusChanged(int friendnumber, TOX_USERSTATUS status);
    void onfriendStatusTextChanged(int friendnumber, const QString& note);

    //Groups
    void ongroupInvite(int friendnumber, QString& public_key);
    void ongroupMessage(int groupnumber,int peernumber, const QString& message);
    void ongroupPeerAdd(int groupnumber, int peernumber);
    void ongroupPeerDel(int groupnumber, int peernumber);
    void ongroupPeerNameChanged(int groupnumber, int peernumber, QString& newname);

private slots:
    void m_processevents();

public slots:
    void start();
    void stop();

    void setuserUsername(const QString& name);
    void setuserStatusnote(const QString& note);

    void acceptFriendRequest(const QString& key);
    void sendFriendRequest(const QString& key, const QString& message);

    void sendFriendMessge(int friendnumber, const QString& message);

    void deleteFriend(int friendnumber);

    int addGroupchat();
    int delGroupchat(int groupnumber);
    int joinGroup(int friendnumber, QString& friend_group_public_key);
    int inviteFriend(int friendnumber, int groupnumber);

    QByteArray saveSettings();
    void loadSettings(QByteArray settings);
};

#endif // CORE_H
