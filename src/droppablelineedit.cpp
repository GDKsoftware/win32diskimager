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
