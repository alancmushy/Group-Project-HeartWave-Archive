#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QRandomGenerator>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    //Setup the UI
    ui->setupUi(this);

    //Initialize the timer count
    time = -1;

    //Initialize the breath pacer
    currentBreathPacer = 0;

    //Setup the HeartWave Machine
    initializeHeartWaveMachine();

    //Get the Menu - Referenced from Denas Project Team 3
    masterMenu = new Menu("MAIN MENU", {"START A NEW SESSION","SETTINGS","LOG/HISTORY"}, nullptr);
    mainMenuOG = masterMenu;
    initializeMainMenu(masterMenu);

    //initialize the sessions
    sessions.push_back(new Session());

    //Initialize the main menu view: Referenced from Denas Project Team 3
    activeQListWidget = ui->menu;
    activeQListWidget->addItems(masterMenu->getMenuItems());
    activeQListWidget->setCurrentRow(0);
    ui->menuLabel->setText(masterMenu->getName());

    //connect all of the slots
    connect(ui->power, &QPushButton::clicked, this, &MainWindow::powerButton);
    connect(ui->menuButton, &QPushButton::clicked, this, &MainWindow::menuButton);
    connect(ui->returnB, &QPushButton::clicked, this, &MainWindow::backButton);
    connect(ui->up, &QPushButton::clicked, this, &MainWindow::upButton);
    connect(ui->down, &QPushButton::clicked, this, &MainWindow::downButton);
    connect(ui->selector, &QPushButton::clicked, this, &MainWindow::select);
    connect(ui->hrContact, QOverload<int>::of(&QComboBox::activated), this, &MainWindow::selectHrContact);
    connect(ui->charge, &QPushButton::clicked, this, &MainWindow::chargeButton);

    //Hide the session screen
    hideSessionUI();

    //Power off initially
    powerButton();

    //Set up the graph
    ui->plot->addGraph();
    ui->plot->graph(0)->setPen(QPen(QColor(40, 110, 255)));
    ui->plot->axisRect()->setupFullAxesBox();
    ui->plot->yAxis->setRange(50, 100);
    ui->plot->xAxis->setRange(0, 200);
    ui->plot->yAxis->setLabel("Heart Rate");
    ui->plot->xAxis->setLabel("Time");
    ui->plot->xAxis->setTickLabelColor("white");

    QSharedPointer<QCPAxisTickerTime> xAxisTicker(new QCPAxisTickerTime);
    xAxisTicker->setTickCount(5);
    xAxisTicker->setTickOrigin(0);
    ui->plot->xAxis->setTicker(QSharedPointer<QCPAxisTickerFixed>(new QCPAxisTickerFixed));

    // make left and bottom axes transfer their ranges to right and top axes:
    connect(ui->plot->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->plot->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui->plot->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->plot->yAxis2, SLOT(setRange(QCPRange)));

    chargeButton();
}

//destructor removes sessions
MainWindow::~MainWindow()
{
    delete mainMenuOG;
    delete ui;

    for (int i = 0; i < sessions.size(); i++) {
        delete sessions[i];
    }
}

//main menu initializer
void MainWindow::initializeMainMenu(Menu *menu){
    //Create the appropriate menus for the sessions
    Menu* sessionMenu = new Menu("START NEW SESSION", {"High Coherence", "Medium Coherence", "Low Coherence"}, menu);
    Menu* lowCoherenceSession = new Menu("Low Coherence", {}, sessionMenu);
    Menu* mediumCoherenceSession = new Menu("Medium Coherence", {}, sessionMenu);
    Menu* highCoherenceSession = new Menu("High Coherence", {}, sessionMenu);
    menu->addChildMenu(sessionMenu);
    sessionMenu->addChildMenu(highCoherenceSession);
    sessionMenu->addChildMenu(mediumCoherenceSession);
    sessionMenu->addChildMenu(lowCoherenceSession);

    //Create the settings menu page
    Menu* settingsMenu = new Menu("SETTINGS", {"Breath Pacer Settings"}, menu);
    Menu* breathPacerMenu = new Menu("Choose your breath pacer level", {"4", "5", "10", "20"}, settingsMenu);
    menu->addChildMenu(settingsMenu);
    settingsMenu->addChildMenu(breathPacerMenu);

    //Create the history menu
    Menu* historyMenu = new Menu("HISTORY OF SESSIONS", {"View", "Clear"}, menu);
    Menu* clearMenu = new Menu("Clear?", {"Yes", "No"}, historyMenu);
    Menu* viewMenu = new Menu("View", {}, historyMenu);
    menu->addChildMenu(historyMenu);
    historyMenu->addChildMenu(viewMenu);
    historyMenu->addChildMenu(clearMenu);
}

//coherence value calculator
int MainWindow::coherenceVals(QString coherenceString){
   double newCoherence;

   if(coherenceString == QString("High")){
       newCoherence = getRandomNumber(3, 6);
   }
   else if(coherenceString == QString("Medium")){
       newCoherence = getRandomNumber(1.1, 2.9);
   }
   else if(coherenceString == QString("Low")){
       newCoherence = getRandomNumber(0.1, 1);
   }

    currentSession->appendCoherence(newCoherence);
    currentCoherence = getAverageCoherence();
}


//average coherence calculator
double MainWindow::getAverageCoherence(){
    double total;
    int size = currentSession->getCoherenceList().size();

    for(double val: currentSession->getCoherenceList()){
        total += val;
    }

    currentAchievement = total;
    if(!currentSession->getCoherenceList().isEmpty()){
        return total/size;
    }else {
        return 0;
    }
}

//random number generator
double MainWindow::getRandomNumber(double min, double max)
{
    // Generate a random number between min and max for the coherence scores
    return QRandomGenerator::global()->generateDouble() * (max - min) + min;
}


//Referenced from https://www.qcustomplot.com/index.php/demos/realtimedatademo
void MainWindow::updateGraph(){
    //Calculate HRV
    calculateCurrentHrv();
    calculateAverageHrv();

    double key = time; // time elapsed since start of demo, in seconds
    static double lastPointKey = 0;

    if (key-lastPointKey > 0.002) // at most add point every 2 ms
    {
      // add data here
      ui->plot->graph(0)->addData(key, currentHrv);
      lastPointKey = key;
    }

    // make key axis range scroll with the data (at a constant range size of 8):
    ui->plot->xAxis->setRange(key, 4);
    ui->plot->replot();

    // calculate frames per second:
    static double lastFpsKey;
    static int frameCount;
    ++frameCount;
    if (key-lastFpsKey > 2){
        lastFpsKey = key;
        frameCount = 0;
    }// average fps over 2 seconds
}

//ui layout setup
void MainWindow::initializeHeartWaveMachine(){
    QPixmap up("/home/student/Team-61-Group-Project/up.png"); //full path to image
    QIcon upButton(up);
    ui->up->setIcon(upButton);
    ui->up->setIconSize(QSize(25, 25));
    ui->up->show();

    QPixmap down("/home/student/Team-61-Group-Project/down.png");
    QIcon downButton(down);
    ui->down->setIcon(downButton);
    ui->down->setIconSize(QSize(25, 25));
    ui->down->show();

    QPixmap right("/home/student/Team-61-Group-Project/right.png");
    QIcon rightButton(right);
    ui->right->setIcon(rightButton);
    ui->right->setIconSize(QSize(25, 25));
    ui->right->show();

    QPixmap left("/home/student/Team-61-Group-Project/left.png");
    QIcon leftButton(left);
    ui->left->setIcon(leftButton);
    ui->left->setIconSize(QSize(25, 25));
    ui->left->show();


    QPixmap onOff("/home/student/Team-61-Group-Project/off_on.png");
    QIcon onButton(onOff);
    ui->power->setIcon(onButton);
    ui->power->setIconSize(QSize(30,30));
    ui->power->show();

    QPixmap returnPix("/home/student/Team-61-Group-Project/return.png");
    QIcon returnButton(returnPix);
    ui->returnB->setIcon(returnButton);
    ui->returnB->setIconSize(QSize(50,50));
    ui->returnB->show();

    QPixmap selector("/home/student/Team-61-Group-Project/circle.png");
    QIcon selectorButton(selector);
    ui->selector->setIcon(selectorButton);
    ui->selector->setIconSize(QSize(30,30));
    ui->selector->show();

    QPixmap charge("/home/student/Team-61-Group-Project/charge.png"); //full path to image
    QIcon chargeButton(charge);
    ui->charge->setIcon(chargeButton);
    ui->charge->setIconSize(QSize(30, 30));
    ui->charge->show();

    ui->menuLabel->setAlignment(Qt::AlignCenter);

    ui->plot->hide();

    this->setStyleSheet("background-color: white");
    ui->menu->setStyleSheet("QListWidget { background-color: pink; } " "QListWidget::item:selected { background-color: red; } background-color: pink");

    ui->hrContact->addItem("TRUE", true);
    ui->hrContact->addItem("FALSE", false);
    ui->hrContact->setCurrentText("False");


    ui->right->blockSignals(true);
    ui->left->blockSignals(true);

    //set battery levels
    batteryLevel = 100;
    changeBatteryLevel(batteryLevel);

    breathPacerBar = 0;
    currentBreathPacer = 4;
    breathPacerIncrement = 4;
    ui->breathPacer->setFormat("");
    isOn = false;
}

//update menu
void MainWindow::updateMenu(const QString selectedMenuItem, const QStringList menuItems) {

    activeQListWidget->clear();
    activeQListWidget->addItems(menuItems);
    activeQListWidget->setCurrentRow(0);

    ui->menuLabel->setText(selectedMenuItem);
}

/*Power off and on the device*/
void MainWindow::powerButton(){
    if (batteryLevel > 0) {
        isOn  = !isOn;
        changePower();
    }

    if(time != -1){
        //save information:
        currentSession->setBreathPacer(currentBreathPacer);
        currentSession->setAverageCoherence(currentCoherence);
        currentSession->setAchievmentScore(currentAchievement);
        currentSession->setTime(time);
        currentSession->setTimeString(timeString);
        record += currentSession->toString();

        //reset variables
        time = -1;
        currentSession->getTimer()->stop();
        currentSession->getTimer()->disconnect();
        ui->hrContact->setCurrentText("False");

        currentCoherence = 0;
        currentBreathPacer = 4;
        breathPacerIncrement = 4;
        breathPacerBar = 0;
        currentAchievement = 0;
        currentHrv = 0;
        averageHrv = 0;
        totalHrv = 0;
        achievmentString = QString("0");
        coherenceString = QString("0");
        timeString = QString("0");
        hideSessionUI();
        backButton();
    }
}

//power calculator for system
void MainWindow::changePower(){
    activeQListWidget->setVisible(isOn);
    ui->menu->setVisible(isOn);
    ui->menuLabel->setVisible(isOn);
    ui->heart->setVisible(isOn);
    ui->hrContact->setVisible(isOn);
    ui->batteryBar->setVisible(isOn);
    ui->hrContactLabel->setVisible(isOn);

    ui->menu->setEnabled(isOn);
    ui->menuLabel->setEnabled(isOn);
    ui->heart->setEnabled(isOn);
    ui->hrContact->setEnabled(isOn);
    ui->batteryBar->setEnabled(isOn);
    ui->hrContactLabel->setEnabled(isOn);
    hideSessionUI();
}

//Return to the menu
void MainWindow::menuButton(){
    if(time != -1){
        //save information:
        currentSession->setBreathPacer(currentBreathPacer);
        currentSession->setAverageCoherence(currentCoherence);
        currentSession->setAchievmentScore(currentAchievement);
        currentSession->setTime(time);
        currentSession->setTimeString(timeString);
        record += currentSession->toString();

        //reset variables
        time = -1;
        currentSession->getTimer()->stop();
        currentSession->getTimer()->disconnect();
        ui->hrContact->setCurrentText("False");

        currentCoherence = 0;
        currentBreathPacer = 4;
        breathPacerIncrement = 4;
        breathPacerBar = 0;
        currentAchievement = 0;
        currentHrv = 0;
        averageHrv = 0;
        totalHrv = 0;
        achievmentString = QString("0");
        coherenceString = QString("0");
        timeString = QString("0");
        hideSessionUI();
        backButton();
    }

    while (masterMenu->getName() != "MAIN MENU") {
        masterMenu = masterMenu->getParent();
    }

    updateMenu(masterMenu->getName(), masterMenu->getMenuItems());

    //Hide the session UI
    hideSessionUI();
}

//hide session ui
void MainWindow::hideSessionUI(){
    QPixmap heart("/home/student/Team-61-Group-Project/heartOFF.png");
    QIcon heartOFF(heart);
    ui->heart->setIcon(heartOFF);
    ui->heart->setIconSize(QSize(15, 15));
    ui->heart->show();

    ui->achievement->setVisible(false);
    ui->achievementLabel->setVisible(false);
    ui->length->setVisible(false);
    ui->lengthLabel->setVisible(false);
    ui->coherence->setVisible(false);
    ui->coherenceLabel->setVisible(false);
    ui->plot->setVisible(false);
    ui->breathPacer->setVisible(false);
    ui->red->setVisible(false);
    ui->blue->setVisible(false);
    ui->green->setVisible(false);
    ui->beep->setVisible(false);

    ui->achievement->setEnabled(false);
    ui->achievementLabel->setEnabled(false);
    ui->length->setEnabled(false);
    ui->lengthLabel->setEnabled(false);
    ui->coherence->setEnabled(false);
    ui->coherenceLabel->setEnabled(false);
    ui->plot->setEnabled(false);
    ui->breathPacer->setEnabled(false);
    ui->red->setEnabled(false);
    ui->green->setEnabled(false);
    ui->blue->setEnabled(false);
    ui->beep->setEnabled(false);
}

void MainWindow::showSessionUI(){
    QPixmap heart("/home/student/Team-61-Group-Project/heartON.png");
    QIcon heartON(heart);
    ui->heart->setIcon(heartON);
    ui->heart->setIconSize(QSize(15, 15));
    ui->heart->show();

    ui->achievement->setVisible(true);
    ui->achievementLabel->setVisible(true);
    ui->length->setVisible(true);
    ui->lengthLabel->setVisible(true);
    ui->coherence->setVisible(true);
    ui->coherenceLabel->setVisible(true);
    ui->plot->setVisible(true);
    ui->breathPacer->setVisible(true);
    ui->achievement->setEnabled(true);
    ui->achievementLabel->setEnabled(true);
    ui->length->setEnabled(true);
    ui->lengthLabel->setEnabled(true);
    ui->coherence->setEnabled(true);
    ui->coherenceLabel->setEnabled(true);
    ui->plot->setEnabled(true);
    ui->breathPacer->setEnabled(true);
}

//go back to the menu
void MainWindow::backButton(){
    ui->right->blockSignals(true);
    ui->left->blockSignals(true);

    if(time != -1){
        //save information:
        currentSession->setBreathPacer(currentBreathPacer);
        currentSession->setAverageCoherence(currentCoherence);
        currentSession->setAchievmentScore(currentAchievement);
        currentSession->setTime(time);
        currentSession->setTimeString(timeString);
        record += currentSession->toString();

        //reset variables
        time = -1;
        currentSession->getTimer()->stop();
        currentSession->getTimer()->disconnect();
        ui->hrContact->setCurrentText("False");
        currentCoherence = 0;
        currentBreathPacer = 4;
        breathPacerIncrement = 4;
        breathPacerBar = 0;
        currentAchievement = 0;
        currentHrv = 0;
        averageHrv = 0;
        totalHrv = 0;
        achievmentString = QString("0");
        coherenceString = QString("0");
        timeString = QString("0");
        hideSessionUI();
        backButton();
    }


    if (masterMenu->getName() == "MAIN MENU") {
        activeQListWidget->setCurrentRow(0);
    }
    else {
        masterMenu = masterMenu->getParent();
        updateMenu(masterMenu->getName(), masterMenu->getMenuItems());
    }

    //Hide the session ui
    hideSessionUI();
}

//up button
void MainWindow::upButton(){
    int nextIndex = activeQListWidget->currentRow() - 1;

    if (nextIndex < 0) {
        nextIndex = activeQListWidget->count() - 1;
    }

    activeQListWidget->setCurrentRow(nextIndex);
}

//down button
void MainWindow::downButton(){
    int nextIndex = activeQListWidget->currentRow() + 1;

    if (nextIndex > activeQListWidget->count() - 1) {
        nextIndex = 0;
    }

    activeQListWidget->setCurrentRow(nextIndex);
}

//selector
void MainWindow::select(){
    int index = activeQListWidget->currentRow();
    if (index < 0) return;

    //Assign breath pacer settings from settings
    if(masterMenu->getName() == "Choose your breath pacer level") {
        if(index == 0){
            currentBreathPacer = 4;
        }else if(index == 1){
            currentBreathPacer = 5;
        }else if(index == 2){
            currentBreathPacer = 10;
        }else if(index == 3){
            currentBreathPacer = 20;
        }
        breathPacerIncrement = currentBreathPacer;
        currentSession->setBreathPacer(breathPacerIncrement);
        backButton();
        return;
    }

    //Logic for when the menu is the delete menu.
    if (masterMenu->getName() == "Clear?") {
        if(index == 0){
            //we want to clear the history here of all the sessions
            record.clear();

            for (int x = 0; x < sessions.size(); x++) {
                delete sessions[x];
            }
            sessions.clear();
            backButton();
            return;
        }else if(index == 1){
            //just return back to the "settings/log" menu
            backButton();
            return;
        }
    }

    //If the menu is a parent and clicking on it should display more menus.
    if (masterMenu->get(index)->getMenuItems().length() > 0) {
        masterMenu = masterMenu->get(index);
        this->updateMenu(masterMenu->getName(), masterMenu->getMenuItems());
    }

    //Enter high coherence session
    else if (masterMenu->get(index)->getName() == "High Coherence") {
        if(!hrContact) {
            backButton();
            return;
        }
        updateMenu("High Coherence Session",{});
        beginSession(sessions.last(), QString("high"));
    }

    //Enter medium coherence session
    else if (masterMenu->get(index)->getName() == "Medium Coherence") {
        if(!hrContact) {
            backButton();
            return;
        }
        updateMenu("Medium Coherence Session",{});
        beginSession(sessions.last(), QString("medium"));
    }

    //Enter low coherence session
    else if (masterMenu->get(index)->getName() == "Low Coherence") {
        if(!hrContact) {
            backButton();
            return;
        }
        updateMenu("Low Coherence Session",{});
        beginSession(sessions.last(), QString("low"));
    }

    //If the button pressed should display the device's recordings.
    else if (masterMenu->get(index)->getName() == "View") {
        masterMenu = masterMenu->get(index);
        updateMenu("RECORDINGS", record);
    }
}

void MainWindow::selectHrContact(){
    hrContact = ui->hrContact->currentData().toBool();

    if (time != -1) {
        if (!isOn) {
            currentSession->getTimer()->stop();

            //save information:
            currentSession->setBreathPacer(currentBreathPacer);
            currentSession->setAverageCoherence(currentCoherence);
            currentSession->setAchievmentScore(currentAchievement);
            currentSession->setTime(time);
            currentSession->setTimeString(timeString);
            record += currentSession->toString();

            //reset variables
            time = -1;
            currentSession->getTimer()->stop();
            currentSession->getTimer()->disconnect();
            ui->hrContact->setCurrentText("False");

            currentCoherence = 0;
            currentBreathPacer = 4;
            breathPacerIncrement = 4;
            breathPacerBar = 0;
            currentAchievement = 0;
            currentHrv = 0;
            averageHrv = 0;
            totalHrv = 0;
            achievmentString = QString("0");
            coherenceString = QString("0");
            timeString = QString("0");
            hideSessionUI();
            backButton();
        }
        else {
            currentSession->getTimer()->start(1000);
        }
    }
}

//charge button reset
void MainWindow::chargeButton(){
    changeBatteryLevel(100);
}

//drain battery algorithm
void MainWindow::drainBattery(){
    //Based on how long a session lasts
    double drainedBatteryLevel = batteryLevel - 1;
    changeBatteryLevel(drainedBatteryLevel);
}

//Starts timer
void MainWindow::initializeTimer(QTimer* t) {
    connect(t, &QTimer::timeout, this, &MainWindow::updateTimer);
    if (hrContact == true) {
        t->start(1000);
    }
}

//Update timer, coherence, achievment scores
void MainWindow::updateTimer() {

    if(currentSession->getTime() < 0){
        currentSession->setTime(0);
    }

    if(time >= 64){

        if(currentSession->getCoherenceString() == QString("High")){
            ui->green->setVisible(true);
        }
        else if(currentSession->getCoherenceString() == QString("Medium")){
            ui->blue->setVisible(true);
        }
        else if(currentSession->getCoherenceString() == QString("Low")){
            ui->red->setVisible(true);
        }
    }else{
         ui->blue->setVisible(false);
         ui->red->setVisible(false);
         ui->green->setVisible(false);
    }

    if(time % 64 == 0 && time > 0){
        ui->beep->setVisible(true);
    }else{
        ui->beep->setVisible(false);
    }

    if (currentSession->getTime() == 0) {
        time += 1;
    }
    else {
        time -= 1;
    }

    //Update coherence
    if(time % 5 == 0){
        coherenceVals(currentSession->getCoherenceString());
    }


    timeString = QString::number(time/60) + ((time%60 < 10) ? + ":0" + QString::number(time%60) : + ":" + QString::number(time%60));
    achievmentString = QString::number(currentAchievement, 'f', 2);
    coherenceString = QString::number(currentCoherence, 'f', 2);

    ui->length->clear();
    ui->length->setText(timeString);
    ui->achievement->clear();
    ui->achievement->setText(achievmentString);
    ui->coherence->clear();
    ui->coherence->setText(coherenceString);

    if (!hrContact) {
        currentSession->setTime(time);
        record += currentSession->toString();

        //Save record
        time = -1;
        currentSession->getTimer()->stop();
        currentSession->getTimer()->disconnect();
        ui->hrContact->setCurrentText("False");

        currentCoherence = 0;
        currentBreathPacer = 4;
        breathPacerIncrement = 4;
        breathPacerBar = 0;
        currentAchievement = 0;
        currentHrv = 0;
        averageHrv = 0;
        totalHrv = 0;
        achievmentString = QString("0");
        coherenceString = QString("0");
        timeString = QString("0");
        hideSessionUI();
        backButton();
    }
    drainBattery();
    changeBreathPacerLevel();
    updateGraph(); //currently updates every second, could be every 5 seconds so the graph does not update every second
}

//battery levels ui
void MainWindow::changeBatteryLevel(double newLevel) {
    if (newLevel >= 0.0 && newLevel <= 100.0) {
        if (newLevel == 0.0 && isOn == true) {
            powerButton();
            batteryLevel = 0;
        }else{
            batteryLevel = newLevel;
        }

        int newLevelInt = int(newLevel);
        ui->batteryBar->setValue(newLevelInt);

        QString highBatteryHealth = "QProgressBar { selection-background-color: rgb(78, 154, 6); background-color: rgb(0, 0, 0); }";
        QString mediumBatteryHealth = "QProgressBar { selection-background-color: rgb(196, 160, 0); background-color: rgb(0, 0, 0); }";
        QString lowBatteryHealth = "QProgressBar { selection-background-color: rgb(164, 0, 0); background-color: rgb(0, 0, 0); }";

        if (newLevelInt >= 50) {
            ui->batteryBar->setStyleSheet(highBatteryHealth);
        }
        else if (newLevelInt >= 20) {
            ui->batteryBar->setStyleSheet(mediumBatteryHealth);
        }
        else {
            ui->batteryBar->setStyleSheet(lowBatteryHealth);
        }
    }
}

void MainWindow::changeBreathPacerLevel(){
    if(breathPacerBar < 0 || breathPacerBar >= 100){
        breathPacerIncrement *= -1;
    }
    breathPacerBar += breathPacerIncrement;
    ui->breathPacer->setValue(breathPacerBar);
}

void MainWindow::beginSession(Session* session, QString coherence){
    if(!hrContact){
        backButton();
        return;
    }

    //Set current session and coherence
    currentSession = session;
    if(coherence == QString("high")){
        currentSession->setCoherenceString(QString("High"));
    }else if(coherence == QString("medium")){
        currentSession->setCoherenceString(QString("Medium"));
    }else if(coherence == QString("low")){
        currentSession->setCoherenceString(QString("Low"));
    }

    //Show UI
    showSessionUI();

    session->setTime(0);
    session->setBreathPacer(currentBreathPacer);
    breathPacerIncrement = currentBreathPacer;
    breathPacerBar = 0;
    ui->breathPacer->setValue(breathPacerBar);
    time = session->getTime();
    timeString = QString::number(time/60) + ":00";
    ui->length->setText(timeString);
    ui->achievement->setText(QString::number(0));
    ui->coherence->setText(QString::number(0));

    //Initialize the timer
    initializeTimer(session->getTimer());

    //Set up new session
    Session *newSession = new Session();
    sessions.append(newSession);

    //Block the right and left buttons
    ui->right->blockSignals(false);
    ui->left->blockSignals(false);
}

void MainWindow::calculateAverageHrv(){
    averageHrv = totalHrv / time;
    currentSession->setAverageHrv(QString::number(averageHrv));
}

void MainWindow::calculateCurrentHrv(){
    currentHrv = getRandomNumber(51, 99);
    totalHrv += currentHrv;
}

