#ifndef SIMILARTHREAD_H
#define SIMILARTHREAD_H

#include <QThread>
#include <QList>
#include <QStringList>
#include <QString>

// sub thread used in Similar Action


class SimilarThread  : public QThread
{

     Q_OBJECT

public:
    QList<QStringList> data[16];            // order information received from MainWindow
    QList<QStringList> map;                 // map information received from MainWindow
    QStringList target;                     // order user choose to find similar ones

    QList<QStringList> Result;              // list of similar orders

    int Start;                              // Start_Date in MainWindow
    int End;                                // End_Date in MainWindow
    int countLine = 0;                      // current lines read
    int totalLine = 0;                      // total line number

    int tar_len;                            // time spent of target order

signals:
    void changeNum(int);                    // transmit lines having been read
    void transTotal(int);                   // transmit total line number
    void transRate(int);                    // transmit rate of progress
    void transResult(QList<QStringList>);   // transmit list of similar orders

private:
    void run() override;
    int  figure(QStringList);               // figure out the similarity of two orders

};

#endif // SIMILARTHREAD_H
