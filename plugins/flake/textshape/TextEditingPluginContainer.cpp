/* This file is part of the KDE project
 * Copyright (C) 2010 Thomas Zander <zander@kde.org>
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

#include "TextEditingPluginContainer.h"
#include "TextTool.h"
#include <KoTextEditingRegistry.h>
#include <KoTextEditingPlugin.h>

#include <QDebug>

TextEditingPluginContainer::TextEditingPluginContainer(QObject *parent)
    : QObject(parent)
{
    foreach (const QString &key, KoTextEditingRegistry::instance()->keys()) {
        KoTextEditingFactory *factory =  KoTextEditingRegistry::instance()->value(key);
        Q_ASSERT(factory);
        if (m_textEditingPlugins.contains(factory->id())) {
            qWarning() << "Duplicate id for textEditingPlugin, ignoring one! (" << factory->id() << ")";
            continue;
        }
        KoTextEditingPlugin *plugin = factory->create();
        if (plugin) {
            m_textEditingPlugins.insert(factory->id(), plugin);
        }
    }
}

TextEditingPluginContainer::~TextEditingPluginContainer()
{
    qDeleteAll(m_textEditingPlugins);
    m_textEditingPlugins.clear();
}

KoTextEditingPlugin *TextEditingPluginContainer::spellcheck() const
{
    return plugin("spellcheck");
}
