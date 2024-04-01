#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSpinBox>
#include <QListWidget>
#include <QGraphicsView>
#include <QTime>
#include <QStatusBar>
#include <QVector>
#include <QtGlobal>
#include <QRandomGenerator>

#include "menu.h"
#include "session.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    //variables

    //The UI
    Ui::MainWindow *ui;

    //The master menus
    Menu* masterMenu;
    Menu* mainMenuOG;

    //Variables
    bool isOn;
    bool hrContact;
    int batteryLevel;
    int time;
    int currentBreathPacer;
    int breathPacerIncrement;
    double currentAchievement;
    double currentCoherence;
    int breathPacerBar;
    double averageHrv;
    double currentHrv;
    int totalHrv;

    QString timeString;
    QString achievmentString;
    QString coherenceString;

    //Vector to hold all of the sessions
    QVector<Session*> sessions;

    //Pointer for the current running session
    Session* currentSession;

    //QListWidgetList to keep track of the menus
    QListWidget *activeQListWidget;

    //Record
    QStringList record;

    void updateMenu(const QString selectedMenuItem, const QStringList menuItems);
    //functions
    void initializeHeartWaveMachine(); //creates the machine
    void incoherentGraph(); //incoherent graph
    void coherentGraph(); //coherent graph
    void initializeMainMenu(Menu *menu);
    void initializeTimer(QTimer* t);
    void drainBattery();
    void updateTimer();
    void beginSession(Session* session, QString coherence);
    int coherenceVals(QString coherenceString); //coherence levels
    void changePower();
    void hideSessionUI();
    void showSessionUI();
    void changeBatteryLevel(double newLevel);
    void changeBreathPacerLevel();
    double getRandomNumber(double min, double max);
    double getAverageCoherence();
    void updateGraph(); //update graph slot based on HRV inputs
    void calculateAverageHrv();
    void calculateCurrentHrv();


private slots:
    //slots for 8 buttons
    void powerButton(); //power on and off
    void menuButton(); //go to menu
    void backButton(); //go back to menu
    void upButton(); //go up in screen
    void downButton(); //go down in screen
    void select(); //selector button that handles the menu for now
    void selectHrContact();
    void chargeButton();
};
#endif // MAINWINDOW_H
