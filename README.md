# FINAL PROJECT (HEARTWAVE)

The HeartWave is a simulation device able to measure, analyze and display HRV (Heart Rate Variability) patterns through the use of an advanced heart rate monitor and provide users with real-time biofeedback on their coherence levels. 

For full project, visit: 
[Video] (https://youtu.be/YdOslNPAC2Y)
[Project] (https://github.com/alancmushy/Group-Project-HeartWave-Archive)


## TABLE OF CONTENTS

- Requirements 
- Individual Contributions
- Files
- Troubleshooting

## REQUIREMENTS

- QCustomPlot Module
[QCustomPlot] (https://www.qcustomplot.com)

## INDIVIDUAL CONTRIBUTIONS

Ariba Rajput (101228808)
- Set up Github Repository
- Set up and hosted online meetings
- Designed outline for project
- Designed and implemented program functionality
- Designed UI
- Designed HRV algorithm for system
- Debugged program

Alberto Chavez (100996906)
- Designed outline for project
- Set up and hosted online meetings
- Collaborated in designing and implementing program functionality
- Designed early version of UI
- Integrated customplot library into the program 
- Designed and created README file
- Designed and created UML diagram
- Debugged program

Alan Mushyirahamwe (101152632)
- Designed outline for project
- Recorded demo presentation
- Set up and hosted online meetings
- Designed and created traceability matrix
- Designed and created sequence diagrams
- Designed beeping functionality for system
- Debugged program

Darius Banker (101196575)
- Designed outline for project
- Designed and created all Use Cases
- Set up and hosted online meetings
- Created Architecture to convert all UCM to sequence Diagrams
- Designed and collaboratively created sequence diagrams

## FILES

- mainwindow.cpp
- mainwindow.h
- mainwindow.ui
- menu.cpp
- menu.h
- session.cpp
- session.h
- qcustomplot.cpp
- qcustomplot.h


## TROUBLESHOOTING

If the project does not recognize header files: 

-Help->About Plugins->uncheck the Clang Plugin

If the project does not built properly:

- Build -> Clean All
- Build -> Run qmake
- Build -> Build All

If project does not display images:

- Ensure the image path is correct on mainwindow.cpp file
