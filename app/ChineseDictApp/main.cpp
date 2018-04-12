/*
 * Copyright Justin Armstrong 2012, 2018.
 *
 * This file is part of the application "Chinese-English Dictionary for Qt"
 *
 * "Chinese-English Dictionary for Qt" is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QFontDatabase>
#include "dictdb.h"
#include "settings.h"
#include "textutils.h"

int main(int argc, char *argv[])
{
    QGuiApplication::setApplicationName("Chinese Dictionary");
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    qputenv("QT_QUICK_CONTROLS_STYLE", "material");
    //qputenv("QT_QUICK_CONTROLS_STYLE", "imagine");


    qmlRegisterType<QObjectListModel>();
    qmlRegisterType<DictDb>();
    qmlRegisterType<SearchResult>();

    DictDb dictDb;
    Settings settings;
    QObjectListModel resultsModel;

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    QQmlContext* ctxt = engine.rootContext();
    ctxt->setContextProperty("dict", &dictDb);
    ctxt->setContextProperty("searchResults", &resultsModel);
    ctxt->setContextProperty("settings", &settings);

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    QObject* item = engine.rootObjects()[0];
    QObject::connect(item, SIGNAL(matchChineseAsync(QString)), &dictDb,
                     SLOT(onMatchChineseAsync(QString)), Qt::QueuedConnection);
    QObject::connect(item, SIGNAL(matchEnglishAsync(QString)), &dictDb,
                     SLOT(onMatchEnglishAsync(QString)), Qt::QueuedConnection);
    QObject::connect(item, SIGNAL(requestDetailsAsync(int)), &dictDb,
                     SLOT(onRequestDetailsAsync(int)), Qt::QueuedConnection);
    QObject::connect(&dictDb, SIGNAL(extraInfoChanged(QVariant)), item,
                     SLOT(onExtraInfoChanged(QVariant)), Qt::QueuedConnection);
    QObject::connect(&dictDb, SIGNAL(componentCharactersChanged(QVariant)), item,
                     SLOT(onComponentCharactersChanged(QVariant)), Qt::QueuedConnection);


    QObject::connect(&dictDb, SIGNAL(classifiersChanged(QVariant)), item,
                     SLOT(onClassifiersChanged(QVariant)), Qt::QueuedConnection);

    QObject::connect(&dictDb, SIGNAL(searchInProgressChanged(QVariant)), item,
            SLOT(onSearchInProgressChanged(QVariant)), Qt::QueuedConnection);

    QObject::connect(&dictDb, SIGNAL(clearAndDeleteResultList()), &resultsModel,
            SLOT(onClearAndDeleteAsync()), Qt::QueuedConnection);
    QObject::connect(&dictDb, SIGNAL(changeResultList(QObjectList*)), &resultsModel,
            SLOT(onSetObjectListAsync(QObjectList*)), Qt::QueuedConnection);


    dictDb.start();
    return app.exec();
}
