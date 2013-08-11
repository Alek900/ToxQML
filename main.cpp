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

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQml>
#include <QQmlContext>

#include "src/statuswrapper.h"
#include "src/core.h"
#include "src/coremodel.h"

int main(int argc, char *argv[])
{
    QGuiApplication a(argc, argv);

    //Think this should be moved into CoreModel
    Core *core = new Core();
    CoreModel *model = new CoreModel(core);
    core->start();

    QQmlApplicationEngine engine;
    //qmlRegisterUncreatableType<StatusWrapper>("tox.friends", 1, 0, "Status", "Satus enums for friends");
    engine.rootContext()->setContextProperty("CoreModel", model);
    engine.load(QUrl("qrc:/qml/newui.qml"));

    int ret = a.exec();
    core->stop();
    return ret;
}
