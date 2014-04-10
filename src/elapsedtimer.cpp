#include "elapsedtimer.h"

ElapsedTimer::ElapsedTimer(QWidget *parent)
    : QLabel(parent)
{
    timer = new QTime();
    setTextFormat(Qt::PlainText);
    setTextInteractionFlags(Qt::NoTextInteraction);
    setText(QString("0:00/0:00"));
    setVisible(false);
}

ElapsedTimer::~ElapsedTimer()
{
    delete timer;
}

int ElapsedTimer::ms()
{
    return(timer->elapsed());
}

void ElapsedTimer::update(unsigned long long progress, unsigned long long total)
{
    unsigned short eSec = 0, eMin = 0, eHour = 0;
    unsigned short tSec = 0, tMin = 0, tHour = 0;

    unsigned int baseSecs = timer->elapsed() / MS_PER_SEC;
    eSec = baseSecs % SECS_PER_MIN;
    if (baseSecs >= SECS_PER_MIN)
    {
        eMin = baseSecs / SECS_PER_MIN;
        if (baseSecs >= SECS_PER_HOUR)
        {
            eHour = baseSecs / SECS_PER_HOUR;
        }
    }

    unsigned int totalSecs = (unsigned int)((float)baseSecs * ( (float)total/(float)progress ));
    unsigned int totalMin = 0;
    tSec = totalSecs % SECS_PER_MIN;
    if (totalSecs >= SECS_PER_MIN)
    {
        totalMin = totalSecs / SECS_PER_MIN;
        tMin = totalMin % MINS_PER_HOUR;
        if (totalMin > MINS_PER_HOUR)
        {
            tHour = totalMin / MINS_PER_HOUR;
        }
    }

    const QChar & fillChar = QLatin1Char( '0' );
    QString qs = QString("%1:%2/").arg(eMin, 2, 10, fillChar).arg(eSec, 2, 10, fillChar);
    if (eHour > 0)
    {
        qs.prepend(QString("%1:").arg(eHour, 2, 10, fillChar));
    }
    if (tHour > 0)
    {
        qs += (QString("%1:").arg(tHour, 2, 10, fillChar));
    }
    qs += (QString("%1:%2 ").arg(tMin, 2, 10, fillChar).arg(tSec, 2, 10, fillChar));
        // added a space following the times to separate the text slightly from the right edge of the status bar...
        // there's probably a more "QT-correct" way to do that (like, margins or something),
        //    but this was simple and effective.
    setText(qs);
}

void ElapsedTimer::start()
{
    setVisible(true);
    timer->start();
}

void ElapsedTimer::stop()
{
    setVisible(false);
    setText(QString("0:00/0:00"));
}
