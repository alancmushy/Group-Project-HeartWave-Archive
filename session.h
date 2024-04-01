#ifndef SESSION_H
#define SESSION_H

#include <QTimer>
#include <QVector>

class Session: public QObject
{
public:
    Session();
    Session(QString name);
    ~Session();

    int getTime();
    double getAverageCoherence();
    QString getCoherenceString();
    int getBreathPacer();
    QTimer* getTimer();
    QVector<double> getCoherenceList();

    void setTime(int time);
    void setAverageCoherence(double averageCoherence);
    void setBreathPacer(int breathPacer);
    void setCoherenceString(QString coherenceString);
    void appendCoherence(double newVal);
    void setAchievmentScore(int achievement);
    void setAverageHrv(QString averageHrv);
    void setTimeString(QString timeString);
    QString toString();
private:
    QString name;
    QTimer* timer;
    int time;
    QString timeString;
    double averageCoherence;
    QString coherenceString;
    QString averageHrv;
    int breathPacer;
    QVector<double> coherenceList;
    int achievmentScore;
};

#endif // SESSION_H
