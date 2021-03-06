/* This file is part of the KDE project
 * Copyright (C) 2008 Jan Hambrecht <jaham@gmx.net>
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

#ifndef KOSHAPESHADOWCOMMAND_H
#define KOSHAPESHADOWCOMMAND_H

#include "kritaflake_export.h"
#include <kundo2command.h>
#include <QList>

class KoShape;
class KoShapeShadow;

/// The undo / redo command for setting the shape shadow
class KRITAFLAKE_EXPORT KoShapeShadowCommand : public KUndo2Command
{
public:
    /**
     * Command to set a new shape shadow.
     * @param shapes a set of all the shapes that should get the new shadow.
     * @param shadow the new shadow, the same for all given shapes
     * @param parent the parent command used for macro commands
     */
    KoShapeShadowCommand(const QList<KoShape*> & shapes, KoShapeShadow *shadow, KUndo2Command *parent = 0);

    /**
     * Command to set new shape shadows.
     * @param shapes a set of all the shapes that should get a new shadow.
     * @param shadows the new shadows, one for each shape
     * @param parent the parent command used for macro commands
     */
    KoShapeShadowCommand(const QList<KoShape*> &shapes, const QList<KoShapeShadow*> &shadows, KUndo2Command *parent = 0);

    /**
     * Command to set a new shape shadow.
     * @param shape a single shape that should get the new shadow.
     * @param shadow the new shadow
     * @param parent the parent command used for macro commands
     */
    KoShapeShadowCommand(KoShape *shape, KoShapeShadow *shadow, KUndo2Command *parent = 0);

    ~KoShapeShadowCommand() override;
    /// redo the command
    void redo() override;
    /// revert the actions done in redo
    void undo() override;
private:
    class Private;
    Private * const d;
};

#endif // KOSHAPESHADOWCOMMAND_H
