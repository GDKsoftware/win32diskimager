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

#include <QtWidgets>
#include "droppablelineedit.h"

DroppableLineEdit::DroppableLineEdit(QWidget *parent)
    : QLineEdit(parent)
{
    setAcceptDrops(true);
}

void DroppableLineEdit::dragEnterEvent(QDragEnterEvent *event)
{
    // accept just text/uri-list and text/plain mime formats
    if ( (event->mimeData()->hasFormat("text/uri-list")) ||
         (event->mimeData()->hasFormat("text/plain")) )
    {
        event->acceptProposedAction();
    }
}

void DroppableLineEdit::dropEvent(QDropEvent *event)
{
    QList<QUrl> urlList;
    QString fName;
    QFileInfo info;
    const QMimeData *data = event->mimeData();

    if (data->hasUrls())
    {
        urlList = data->urls(); // returns list of QUrls
        // if just text was dropped, urlList is empty (size == 0)

        if ( urlList.size() > 0) // if at least one QUrl is present in list
        {
            fName = urlList[0].toLocalFile(); // convert first QUrl to local path
            info.setFile( fName ); // information about file
            if ( info.isFile() )
            {
                setText( fName ); // if is file, setText
                event->acceptProposedAction();
            } else {
//				setText("has url but Cannot drop");
                event->ignore();
            }
        }
    }
    else if (data->hasText())
    {
        setText(data->text());
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}
