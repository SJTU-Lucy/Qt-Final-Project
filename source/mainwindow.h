#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

#include <QWidget>
#include <QPushButton>
#include <QApplication>
#include <QFileDialog>
#include <QDir>
#include <QString>
#include <QStringList>
#include <QDebug>
#include <QMessageBox>
#include <QListWidgetItem>
#include <QSpinBox>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QAreaSeries>

QT_CHARTS_USE_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
public:
    // name of QMenu and QAction is the fuction it is used for
    QMenu *fileMenu;
    QMenu *graphMenu;
    QMenu *analysisMenu;
    QMenu *moreMenu;
    QMenu *HelpMenu;

    QAction *loadAct;
    QAction *mapLoadAct;
    QAction *lineAct;
    QAction *columnAct;
    QAction *roundAct;
    QAction *analysisAct;
    QAction *IncomeAct;
    QAction *chartAct;
    QAction *similarAct;
    QAction *targetAct;
    QAction *timeAct;
    QAction *HelpAct;

    QList<QStringList> data[16];            // data for 15 days
    QList<QStringList> map;                 // map loaded
    QList<QList<int>>  grid[16];            // grid ID for each order in 15 days

    int cnt_total[100];                     // total order number in each grid
    double income_total[100];               // total revenue in each grid
    int Line_count;                         // lines read now
    int total_Line;                         // total line number

    int Start_Date = 1;                     // start date of data you load
    int End_Date = 1;                       // end date of data you load

    int Start_Grid = 0;                     // start grid of your range (for graph)
    int End_Grid = 99;                      // end grid of your range (for graph)

    int NumOrPay = 0;                       // a sign used to decide which to draw 0->chart 1->num 2->pay

    int graphStyle = 0;                     // a sign used to decide which style 0->line 1->bar 2->pie

    int time_start;                         // start grid ID in Time Action user input
    int time_end;                           // end grid ID in Time Action user input

    int target_grid;                        // origin grid ID in Target Action user input


private slots:
    void load();                            // line 79  : slot for LoadData, load your data
    void mapLoad();                         // line 108 : slot for LoadMap, load your map
    void line();                            // line 231 : slot for Line Chart, switch graph to line chart
    void column();                          // line 239 : slot for Bar Chart, switch graph to bar chart
    void round();                           // line 247 : slot for Pie Chart, switch graph to pie chart
    void analysis();                        // line 255 : slot for Count, draw graph for count data
    void income();                          // line 410 : slot for Income, draw graph for income data
    void chart();                           // line 180 : slot for Table, make table
    void similar();                         // line 566 : slot for Similar, start similarThread
    void target();                          // line 610 : slot for Target, initialize range input box
    void time();                            // line 654 : slot for Time, initialize range input box
    void help();                            // line 722 : slot for Help, show start page

    void UpdateLeft(int);                       // line 731: slot for input start date
    void UpdateRight(int);                      // line 737: slot for input end date
    void UpdateStart(int);                      // line 743: slot for input start grid
    void UpdateEnd(int);                        // line 749: slot for input end grid
    void UpdateTarget(int);                     // line 767: slot for input start grid in Target
    void UpdateTimeStart(int);                  // line 755: slot for input start grid in Time
    void UpdateTimeEnd(int);                    // line 761: slot for input end grid in Time
    void Total_Receive(int);                    // line 773: slot for receive total line number from thread
    void Line_Receive(int);                     // line 779: slot for receive current line number from thread
    void Data_Receive(QList<QStringList>,int);  // line 789: slot for receive data emit from myThread
    void Count_Receive(QList<int>);             // line 797: slot for receive total order number from countThread
    void Income_Receive(QList<double>);         // line 812: slot for receive revenue from countThread
    void Grid_Receive(int,QList<QList<int>>);   // line 806: slot for receive grid ID from countThread
    void keep_going();                          // line 821: connect with "FinishLoad" button
    void start_similar(QList<QStringList>);     // line 850: connect with "similar", make chart
    void start_target(QList<int>);              // line 880: connect with "process_target", make graph
    void process_target();                      // line 919: connect with "target", start targetThread
    void start_time(int);                       // line 988: connect with "process_time", show result
    void process_time();                        // line 955: connect witg "time", start timeThread
    void decide();                              // line 1005: decide which action should be done again after updating range
    int  judgeNum(QStringList);                 // line 1025: figure out the start Grid ID for each order
    void close();                               // line 1075: close start page in Help mode
    void changeButtonStatus();                  // line 1049: connect with push on Choose Box

private:
    Ui::MainWindow *ui;
    inline int min(int a,int b);                // line 1083: an inline fuction to calculate the smaller one between two
    inline int max(int a,int b);                // line 1090: an inline fuction to calculate the bigger one between two
    void initMenu();                            // line 1097: initialize the menubar
    void initConnect();                         // line 1140: initialize connect for menubar
    void notLoad();                             // line 1157: warning window for loading not necessary data
    void initCheckBox();                        // line 1165: initialize chooseBox for Table
    void initSlider();                          // line 1186: initialize input widget for date
    void initRange();                           // line 1250: initialize input widget for ID
    void initCount();                           // line 1315: start countThread
};
#endif // MAINWINDOW_H
