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

#ifndef ELAPSEDTIMER_H
#define ELAPSEDTIMER_H

#ifndef WINVER
#define WINVER 0x0601
#endif

#include <QtWidgets>
#include <QLabel>
#include <QTime>
#include <QString>
//#include <cstdio>
//#include <cstdlib>
//#include <windows.h>
//#include <winioctl.h>

class ElapsedTimer : public QLabel
{
    Q_OBJECT

public:
    ElapsedTimer(QWidget *parent = 0);
    ~ElapsedTimer();
    int ms();
    void update(unsigned long long progress, unsigned long long total);
    void start();
    void stop();

private:
//    QLabel *lDisplay;
    QTime *timer;
    static const unsigned short MS_PER_SEC = 1000;
    static const unsigned short SECS_PER_MIN = 60;
    static const unsigned short MINS_PER_HOUR = 60;
    static const unsigned short SECS_PER_HOUR = (SECS_PER_MIN * MINS_PER_HOUR);
};

#endif // ELAPSEDTIMER_H
