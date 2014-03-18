/**********************************************************************
 *  This program is free software; you can redistribute it and/or     *
 *  modify it under the terms of the GNU General Public License       *
 *  as published by the Free Software Foundation; either version 2    *
 *  of the License, or (at your option) any later version.            *
 *                                                                    *
 *  This program is distributed in the hope that it will be useful,   *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of    *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the     *
 *  GNU General Public License for more details.                      *
 *                                                                    *
 *  You should have received a copy of the GNU General Public License *
 *  along with this program; if not, see http://gnu.org/licenses/
 *  ---                                                               *
 *  Copyright (C) 2009, Justin Davis <tuxdavis@gmail.com>             *
 *  Copyright (C) 2009-2014 ImageWriter developers                    *
 *                          https://launchpad.net/~image-writer-devs  *
 **********************************************************************/

#ifndef DROPPABLELINEEDIT_H
#define DROPPABLELINEEDIT_H

#include <QtWidgets>
#include <QLineEdit>

class DroppableLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    DroppableLineEdit(QWidget *parent = 0);

    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

private:

};


#endif
