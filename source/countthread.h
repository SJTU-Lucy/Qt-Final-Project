#ifndef COUNTTHREAD_H
#define COUNTTHREAD_H

#include <QThread>
#include <QList>
#include <QStringList>
#include <QString>

// sub thread used to pre-process data

class CountThread : public QThread
{

     Q_OBJECT

public:
    QList<int> count;                       // total order number in each grid
    QList<double> income;                   // total income in each grid
    QList<QStringList> data[16];            // order information received from MainWindow
    QList<QList<int>>  grid;                // grid information for each order
    QList<QStringList> map;                 // map information received from MainWindow
    int Start;                              // Start_Date in MainWindow
    int End;                                // End_Date in MainWindow
    int countLine = 0;                      // current lines read
    int totalLine = 0;                      // total line number

signals:
    void changeNum(int);                    // transmit lines having been read
    void transTotal(int);                   // transmit total line number
    void transRate(int);                    // transmit rate of progress
    void transCount(QList<int>);            // transmit total order number in each grid
    void transIncome(QList<double>);        // transmit total income in each grid
    void transGrid(int,QList<QList<int>>);  // transmit grid information for each order

private:
    void run() override;
    int  judgeNum(QStringList);              // calculate the starting grid number
    int  judgeArrive(QStringList);           // caltulate the ending grid number
};

#endif // COUNTTHREAD_H
