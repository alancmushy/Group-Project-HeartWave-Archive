#include "session.h"

#include <QString>
#include <QDebug>

Session::Session()
{
    // initialize the timer
    timer = new QTimer(this);
}

Session::~Session(){
    delete timer;
}

int Session::getTime() {return time;}
double Session::getAverageCoherence() {return averageCoherence;}
int Session::getBreathPacer() {return breathPacer;}
QTimer* Session::getTimer() {return timer;}
QString Session::getCoherenceString(){return coherenceString;}

void Session::setTime(int time){this->time=time;}
void Session::setAverageCoherence(double averageCoherence){this->averageCoherence=averageCoherence;}
void Session::setBreathPacer(int breathPacer){this->breathPacer=breathPacer;}
void Session::setCoherenceString(QString coherenceString){
    this->coherenceString= coherenceString;
    this->name = QString(coherenceString + " Coherence Test");
}
void Session::setTimeString(QString timeString){this->timeString = timeString;}
void Session::setAverageHrv(QString averageHrv){
    this->averageHrv = averageHrv;
}

QString Session::toString(){
    QString returnString;

    returnString.append("************~~~~~~~~~~*********\n");

    returnString.append(name);
    returnString.append("\nCoherence score: ");
    returnString.append(QString::number(averageCoherence));
    returnString.append("\nAchievment score: ");
    returnString.append(QString::number(achievmentScore));
    returnString.append("\nDuration: ");
    returnString.append(timeString);
    returnString.append("\nBreath Pacer: ");
    returnString.append(QString::number(breathPacer));
    returnString.append("\n");
    returnString.append("Average HRV: \n");
    returnString.append(averageHrv);

    returnString.append("\n************~~~~~~~~~~*********\n");

    return returnString;
}

QVector<double> Session::getCoherenceList() {return this->coherenceList;}
void Session::appendCoherence(double newVal) {
    this->coherenceList.append(newVal);
}

void Session::setAchievmentScore(int achievment){
    this->achievmentScore = achievment;
}
