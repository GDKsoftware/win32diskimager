#ifndef DROPPABLELINEEDIT_H
#define DROPPABLELINEEDIT_H

#include <QtGui>
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
