#ifndef GROUP_H
#define GROUP_H

#include <QObject>
#include <QList>

#include "user.h"

class Group : public QObject
{
    Q_OBJECT
public:
    explicit Group(QObject *parent = 0);
private:
    int m_id;
    QList<UserItem*> m_useringroup;
signals:

public slots:

};

#endif // GROUP_H
