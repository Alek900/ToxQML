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

#include "core.h"
#include <QFile> //This should not be here
#include <QDir> //or this
#include <QStandardPaths> // or this

//#define ENABLEPERSISTENT

//TODO: Clean up and move settings out of here and into a independet object

Core::Core(QObject *parent) :
    QObject(parent)
{
    eventtimer = new QTimer(this);
}

QString Core::userId()
{
    uint8_t* data = new uint8_t[TOX_FRIEND_ADDRESS_SIZE];
    tox_get_address(m_tox, data);
    QByteArray array(reinterpret_cast<char*>(data), TOX_FRIEND_ADDRESS_SIZE);
    return array.toHex();
}

QString Core::username()
{
    uint8_t *name = new uint8_t[TOX_MAX_NAME_LENGTH];
    int size = tox_get_self_name(m_tox, name, TOX_MAX_NAME_LENGTH);

    QString ret = toQString(name, size);

    delete name;
    return ret;
}

QString Core::toQString(uint8_t *data, uint16_t length)
{
    QString string = QString::fromUtf8((char*)data,length);
    return string;
}

Core::cString Core::fromQString(const QString &string)
{
    QByteArray data(string.toUtf8());

    cString ret;
    ret.size = data.size() + 1;
    ret.data = new uint8_t[ret.size];

    memcpy(ret.data, reinterpret_cast<uint8_t*>(data.data()), data.size());
    ret.data[ret.size - 1] = 0;
    data.clear();

    return ret;
}

void Core::m_friendrequest(uint8_t *public_key, uint8_t *data, uint16_t length, void* userdata)
{
    Core* _this = (Core*)userdata;
    QString message = toQString(data, length);
    QString key = QByteArray(reinterpret_cast<char*>(public_key), CLIENT_ID_SIZE).toHex();

    emit _this->onfriendRequested(key, message);
}

void Core::m_friendmessage(Tox *m, int friendnumber, uint8_t *message, uint16_t length, void* userdata)
{
    Core* _this = (Core*)userdata;
    QString qmessage = toQString(message, length);
    emit _this->onfriendMessaged(friendnumber, qmessage);
}

void Core::m_friendnamechange(Tox *m, int friendnumber, uint8_t *newname, uint16_t length, void* userdata)
{
    Core* _this = (Core*)userdata;
    QString qname = toQString(newname, length);

    emit _this->onfriendNameChanged(friendnumber, qname);
}

void Core::m_frienduserstatuschange(Tox *m, int friendnumber, TOX_USERSTATUS kind, void* userdata)
{
    Core* _this = (Core*)userdata;
    emit _this->onfriendStatusChanged(friendnumber, kind);
}

void Core::m_friendstatusnotechange(Tox *m, int friendnumber, uint8_t *status, uint16_t length, void* userdata)
{
    Core* _this = (Core*)userdata;
    QString message = toQString(status, length);
    emit _this->onfriendStatusTextChanged(friendnumber, message);
}

void Core::m_friendstatuschange(Tox *m, int friendnumber, uint8_t status, void* userdata)
{
    Core* _this = (Core*)userdata;
    emit _this->onfriendStatusChanged(friendnumber, (TOX_USERSTATUS)status);
}
void Core::m_groupinvite(Tox *tox, int friendnumber, uint8_t *group_public_key, void *userdata)
{
    Core* _this =(Core*)userdata;
    QString key = QByteArray(reinterpret_cast<char*>(group_public_key), CLIENT_ID_SIZE).toHex();

    emit _this->ongroupInvite(friendnumber, key);
}

void Core::m_groupmessage(Tox *tox, int groupnumber, int peernumber,
                          uint8_t * message, uint16_t length, void *userdata)
{
    Core* _this = (Core*)userdata;
    QString msg = toQString(message, length);

    emit _this->ongroupMessage(groupnumber, peernumber, msg);
}

void Core::m_groupnamelistchanged(Tox *tox, int groupnumber, int peernumber,
                                  TOX_CHAT_CHANGE change, void *userdata)
{
    Core* _this =(Core*)userdata;

    switch(change)
    {
    case TOX_CHAT_CHANGE_PEER_ADD:
    {
        /* I belive I need to wait for the CHANGE_PEER_NAME to be able to get a peers name.
         *Before that, it's blank.*/

         /*uint8_t name[TOX_MAX_NAME_LENGTH];
        int length = tox_group_peername(tox, groupnumber, peernumber,&name);
        QString peername = toQstirng(name, length);*/

        emit _this->ongroupPeerAdd(groupnumber, peernumber);
    }break;
    case TOX_CHAT_CHANGE_PEER_DEL:
    {
        emit _this->ongroupPeerDel(groupnumber, peernumber);
    }break;
    case TOX_CHAT_CHANGE_PEER_NAME:
    {
        uint8_t *name = new uint8_t[TOX_MAX_NAME_LENGTH];
        uint16_t length = tox_group_peername(tox, groupnumber, peernumber,name);
        QString peername = toQString(name, length);

        emit _this->ongroupPeerNameChanged(groupnumber, peernumber, peername);
    }break;
    }
}

void Core::m_checkdhtconnection()
{
    if (tox_isconnected(m_tox) && !m_connected)
    {
        m_connected = true;
        emit connectedChanged();
    }
    else if (!tox_isconnected(m_tox) && m_connected)
    {
        m_connected = false;
        emit connectedChanged();
    }
}

void Core::addDHTServer(const QString &id, const QString ip, int port)
{
    /*IP_Port ipport;
    QStringList server_ip = ip.split(".");

    //Ugly
    uint32_t ipint = (server_ip.at(0).toLong() << 24) |
            (server_ip.at(1).toLong() << 16) |
            (server_ip.at(2).toLong() << 8) |
            (server_ip.at(3).toLong());

    ipport.ip.ip_v = ipint;
    ipport.port = (uint16_t)port;

    cString key = fromQString(id);

    //DHT_bootstrap(ipport, key.data);*/
}

void Core::m_processevents()
{
    tox_do(m_tox);
    m_checkdhtconnection();
}

void Core::start()
{
    m_tox = tox_new(0);
#ifdef ENABLEPERSISTENT
    QFile config(QStandardPaths::standardLocations(QStandardPaths::DataLocation)[0].append("/data.cfg"));
    if(config.exists())
    {
        config.open(QIODevice::ReadOnly);
        QByteArray cfg = config.readAll();
        loadSettings(cfg);
        config.close();
    }
#endif
    tox_callback_friend_request(m_tox, &Core::m_friendrequest, this);
    tox_callback_friend_message(m_tox, &Core::m_friendmessage, this);
    tox_callback_name_change(m_tox, &Core::m_friendnamechange, (void*)this);
    tox_callback_user_status(m_tox, &Core::m_frienduserstatuschange, (void*)this);
    tox_callback_connection_status(m_tox, &Core::m_friendstatuschange, (void*)this);
    tox_callback_status_message(m_tox, &Core::m_friendstatusnotechange, (void*)this);
    connect(eventtimer, &QTimer::timeout, this, &Core::m_processevents);
    eventtimer->start(30);

    emit onStarted();
}

void Core::stop()
{
#ifdef ENABLEPERSISTENT
    QDir dir = QDir(QStandardPaths::standardLocations(QStandardPaths::DataLocation)[0]);
    if (!dir.exists())
        dir.mkpath(".");
    QFile config(QStandardPaths::standardLocations(QStandardPaths::DataLocation)[0].append("/data.cfg"));
    QByteArray cfg = saveSettings();
    config.open(QIODevice::WriteOnly | QIODevice::Truncate);
    config.write(cfg);
    config.close();
#endif
}

void Core::setuserUsername(const QString &name)
{
    cString tmp = fromQString(name);
    tox_set_name(m_tox, tmp.data,tmp.size);

    delete tmp.data;
}

void Core::setuserStatusnote(const QString &note)
{
    cString tmp = fromQString(note);

    tox_set_status_message(m_tox, tmp.data, tmp.size);
}

void Core::acceptFriendRequest(const QString &key)
{

    QByteArray akey = QByteArray::fromHex(key.toLower().toLatin1());
    uint8_t *ckey = new uint8_t[CLIENT_ID_SIZE];

    memcpy(ckey, reinterpret_cast<uint8_t*>(akey.data()), akey.size());
    int newfriendid = tox_add_friend_norequest(m_tox, ckey);

    if (newfriendid == -1)
    {
        //Fail
    } else {
        //Success
        emit onfriendAdded(newfriendid, key);
    }
}

void Core::sendFriendRequest(const QString &address, const QString &message)
{
    QString key = address.left(crypto_box_PUBLICKEYBYTES);
    QByteArray akey = QByteArray::fromHex(address.toLower().toLatin1());
    uint8_t *ckey = new uint8_t[TOX_FRIEND_ADDRESS_SIZE];

    memcpy(ckey, reinterpret_cast<uint8_t*>(akey.data()), akey.size());
    cString tmp = fromQString(message);
    int newfriendid = tox_add_friend(m_tox, ckey, tmp.data, tmp.size);

    if (newfriendid >= 0)
    {
        emit onfriendAdded(newfriendid, key);
    } else {

    }

    delete tmp.data;
}

void Core::sendFriendMessge(int friendnumber, const QString &message)
{
    cString ret = fromQString(message);
    tox_send_message(m_tox, friendnumber, ret.data, ret.size);
    delete ret.data;
}

void Core::deleteFriend(int friendnumber)
{
    tox_del_friend(m_tox, friendnumber);
}


int Core::addGroupchat()
{

}

int Core::delGroupchat(int groupnumber)
{

}

int Core::joinGroup(int friendnumber, QString& friend_group_public_key)
{

}

int Core::inviteFriend(int friendnumber, int groupnumber)
{

}

QByteArray Core::saveSettings()
{
    uint8_t *data = new uint8_t[tox_size(m_tox)];
    tox_save(m_tox, data);

    return QByteArray(reinterpret_cast<char*>(data),tox_size(m_tox));
}

void Core::loadSettings(QByteArray settings)
{
    uint8_t *data = new uint8_t[settings.size() + 1];

    memcpy(data, reinterpret_cast<uint8_t*>(settings.data()), settings.size());
    tox_load(m_tox, data, settings.size());

    uint8_t *idptr = new uint8_t[CLIENT_ID_SIZE];
    int id = 0;
    QString key;

    while (tox_get_client_id(m_tox, id, idptr) != -1)
    {
        key = QByteArray(reinterpret_cast<char*>(idptr), CLIENT_ID_SIZE).toHex();
        emit onfriendAdded(id, key);
        id++;
    }
}
