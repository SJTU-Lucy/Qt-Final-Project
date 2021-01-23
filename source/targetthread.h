#ifndef TARGETTHREAD_H
#define TARGETTHREAD_H

#include <QThread>
#include <QList>
#include <QStringList>
#include <QString>

// sub thread used in Target Action


class TargetThread  : public QThread
{

     Q_OBJECT

public:
    QList<QStringList> data[16];            // order information received from MainWindow
    QList<QList<int>>  grid[16];            // grid information for each order
    QList<QStringList> map;                 // map information received from MainWindow
    QList<int> Result;                      // target destination statistics
    int Start;                              // Start_Date in MainWindow
    int End;                                // End_Date in MainWindow
    int countLine = 0;                      // current lines read
    int totalLine = 0;                      // total line number

    int Grid;                               // start grid input in MainWindow

signals:
    void changeNum(int);                    // transmit lines having been read
    void transTotal(int);                   // transmit total line number
    void transRate(int);                    // transmit rate of progress
    void transResult(QList<int>);           // transmit destination statistics

private:
    void run() override;
};

#endif // TARGETTHREAD_H
