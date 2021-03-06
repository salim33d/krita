/* This file is part of the KDE project
 * Copyright (C) 2006-2007 Thomas Zander <zander@kde.org>
 * Copyright (C) 2009 Boudewijn Rempt <boud@valdyas.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef KO_UPDATERPRIVATE__P_H
#define KO_UPDATERPRIVATE__P_H

#include "KoProgressUpdater.h"

#include <QTime>
#include <QVector>

/**
 * KoUpdaterPrivate is the gui-thread side of KoUpdater. Communication
 * between KoUpdater and KoUpdaterPrivate is handled through queued
 * connections -- this is the main app thread part of the
 * KoUpdater-KoUpdaterPrivate bridge.
 *
 * The gui thread can iterate over its list of KoUpdaterPrivate
 * instances for the total progress computation: the queued signals
 * from the threads will only arrive when the eventloop in the gui
 * thread has a chance to deliver them.
 */
class KoUpdaterPrivate : public QObject
{

    Q_OBJECT

public:

    KoUpdaterPrivate(KoProgressUpdater *parent, int weight, const QString& name, bool isPersistent = false);

    /// when deleting an updater, make sure the accompanying thread is
    /// interrupted, too.
    ~KoUpdaterPrivate() override;

    bool interrupted() const { return m_interrupted; }

    int progress() const { return m_progress; }

    int weight() const { return m_weight; }

    QString autoNestedName() const;
    QString subTaskName() const;
    QString mergedSubTaskName() const;

    bool hasValidRange() const;
    bool isPersistent() const;
    bool isCompleted() const;

    QPointer<KoUpdater> connectedUpdater() const;

public Q_SLOTS:

    /// Cancel comes from KoUpdater
    void cancel();

    void setInterrupted(bool value = true);

    /// progress comes from KoUpdater
    void setProgress( int percent );

    void setAutoNestedName(const QString &name);
    void setHasValidRange(bool value);


Q_SIGNALS:

    /// Emitted whenever the progress changed
    void sigUpdated();

    /// Emitted whenever the parent KoProgressUpdater is interrupted,
    /// for instance through a press on a cancel button
    void sigInterrupted(bool value);

private:
    int m_progress; // always in percent
    int m_weight;
    bool m_interrupted;
    QString m_subTaskName;
    QString m_autoNestedName;
    bool m_hasValidRange;
    bool m_isPersistent;


    KoProgressUpdater *m_parent;
    QPointer<KoUpdater> m_connectedUpdater;
};

#endif
