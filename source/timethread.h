#ifndef TIMETHREAD_H
#define TIMETHREAD_H

#include <QThread>
#include <QList>
#include <QStringList>
#include <QString>

// sub thread used in Time Action

class TimeThread  : public QThread
{

     Q_OBJECT

public:
    QList<QStringList> data[16];            // order information received from MainWindow
    QList<QList<int>>  grid[16];            // grid information for each order
    QList<QStringList> map;                 // map information received from MainWindow
    int Start;                              // Start_Date in MainWindow
    int End;                                // End_Date in MainWindow
    int countLine = 0;                      // current lines read
    int totalLine = 0;                      // total line number

    int totalTime = 0;                      // sum of order time that satisfy requirement
    int totalNumber = 0;                    // number of orders that satisfy requirement

    int S_grid;                             // start grid input in MainWindow
    int E_grid;                             // End grid input in MainWindow

signals:
    void changeNum(int);                    // transmit lines having been read
    void transTotal(int);                   // transmit total line number
    void transRate(int);                    // transmit rate of progress
    void transAverage(int);                 // transmit average time

private:
    void run() override;
};

#endif // TIMETHREAD_H
