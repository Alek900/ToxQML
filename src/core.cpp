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
#include <QDebug> //this is ok

//#define ENABLEPERSISTENT

static Core *_this = 0;

//TODO: Clean up and move settings out of here and into a independet object

Core::Core(QObject *parent) :
    QObject(parent)
{
    Q_ASSERT(_this == 0);
    _this = this;

    eventtimer = new QTimer(this);
}

QString Core::userId()
{
    return QByteArray(reinterpret_cast<char*>(self_public_key),CLIENT_ID_SIZE).toHex();
}

QString Core::username()
{
    uint8_t *name = new uint8_t[MAX_NAME_LENGTH];
    int size = getself_name(name);

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

void Core::m_friendrequest(uint8_t *public_key, uint8_t *data, uint16_t length)
{
    QString message = toQString(data, length);
    QString key = QByteArray(reinterpret_cast<char*>(public_key), CLIENT_ID_SIZE).toHex();

    emit _this->onfriendRequested(key, message);
}

void Core::m_friendmessage(int friendnumber, uint8_t *message, uint16_t length)
{
    QString qmessage = toQString(message, length);
    emit _this->onfriendMessaged(friendnumber, qmessage);
}

void Core::m_friendnamechange(int friendnumber, uint8_t *newname, uint16_t length)
{
    QString qname = toQString(newname, length);

    emit _this->onfriendNameChanged(friendnumber, qname);
}

void Core::m_frienduserstatuschange(int friendnumber, USERSTATUS kind)
{
    emit _this->onfriendStatusChanged(friendnumber, kind);
}

void Core::m_friendstatusnotechange(int friendnumber, uint8_t *status, uint16_t length)
{
    QString message = toQString(status, length);
    emit _this->onfriendStatusTextChanged(friendnumber, message);
}

void Core::m_friendstatuschange(int friendnumber, uint8_t status)
{
    if (status == 4)
    {
        emit _this->onfriendStatusChanged(friendnumber, USERSTATUS_NONE);
    } else {
        emit _this->onfriendStatusChanged(friendnumber, USERSTATUS_INVALID);
    }
}

void Core::m_checkdhtconnection()
{
    if (DHT_isconnected() && !m_connected)
    {
        m_connected = true;
        emit connectedChanged();
    }
    else if (!DHT_isconnected() && m_connected)
    {
        m_connected = false;
        emit connectedChanged();
    }
}

void Core::addDHTServer(const QString &id, const QString ip, int port)
{
    IP_Port ipport;
    QStringList server_ip = ip.split(".");

    //Ugly
    uint32_t ipint = (server_ip.at(0).toLong() << 24) |
            (server_ip.at(1).toLong() << 16) |
            (server_ip.at(2).toLong() << 8) |
            (server_ip.at(3).toLong());

    ipport.ip.i = ipint;
    ipport.port = (uint16_t)port;

    cString key = fromQString(id);

    DHT_bootstrap(ipport, key.data);
}

void Core::m_processevents()
{
    doMessenger();
    m_checkdhtconnection();
}

void Core::start()
{
    initMessenger();
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
    m_callback_friendrequest(&Core::m_friendrequest);
    m_callback_friendmessage(&Core::m_friendmessage);
    m_callback_namechange(&Core::m_friendnamechange);
    m_callback_userstatus(&Core::m_frienduserstatuschange);
    m_callback_statusmessage(&Core::m_friendstatusnotechange);
    m_callback_friendstatus(&Core::m_friendstatuschange);
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
    setname(tmp.data, tmp.size);
    delete tmp.data;
}

void Core::setuserStatusnote(const QString &note)
{
    cString tmp = fromQString(note);
    qDebug() << QByteArray((char*)tmp.data, tmp.size).toHex();
    m_set_statusmessage(tmp.data,tmp.size);
}

void Core::acceptFriendRequest(const QString &key)
{

    QByteArray akey = QByteArray::fromHex(key.toLower().toLatin1());
    uint8_t *ckey = new uint8_t[CLIENT_ID_SIZE];

    memcpy(ckey, reinterpret_cast<uint8_t*>(akey.data()), akey.size());
    int newfriendid = m_addfriend_norequest(ckey);

    if (newfriendid == -1)
    {
        //Fail
    } else {
        //Success
        emit onfriendAdded(newfriendid, key);
    }
}

void Core::sendFriendRequest(const QString &key, const QString &message)
{
    QByteArray akey = QByteArray::fromHex(key.toLower().toLatin1());
    uint8_t *ckey = new uint8_t[CLIENT_ID_SIZE];

    memcpy(ckey, reinterpret_cast<uint8_t*>(akey.data()), akey.size());
    cString tmp = fromQString(message);

    int newfriendid = m_addfriend(ckey, tmp.data, tmp.size);
    emit onfriendAdded(newfriendid, key);

    delete tmp.data;
}

void Core::sendFriendMessge(int friendnumber, const QString &message)
{
    cString ret = fromQString(message);
    m_sendmessage(friendnumber, ret.data, ret.size);
    delete ret.data;
}

void Core::deleteFriend(int friendnumber)
{
    m_delfriend(friendnumber);
}

QByteArray Core::saveSettings()
{
    uint8_t *data = new uint8_t[Messenger_size()];
    Messenger_save(data);

    return QByteArray(reinterpret_cast<char*>(data),Messenger_size());
}

void Core::loadSettings(QByteArray settings)
{
    uint8_t *data = new uint8_t[settings.size() + 1];

    memcpy(data, reinterpret_cast<uint8_t*>(settings.data()), settings.size());
    Messenger_load(data, settings.size());

    uint8_t *idptr = new uint8_t[CLIENT_ID_SIZE];
    int id = 0;
    QString key;

    while (getclient_id(id, idptr) != -1)
    {
        key = QByteArray(reinterpret_cast<char*>(idptr), CLIENT_ID_SIZE).toHex();
        emit onfriendAdded(id, key);
        id++;
    }
}
