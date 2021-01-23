#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QThread>
#include <QList>
#include <QStringList>
#include <QString>

// sub thread used to load data

class MyThread : public QThread
{
    Q_OBJECT

public:
    QList<QStringList> Result;              // Data read in this thread
    int countLine = 0;                      // current lines read
    int totalLine = 0;                      // total line number
    QString fileName;                       // filename of the file chosen in MainWindow
    int num;                                // date of the file detected

signals:
    void changeList(QList<QStringList>,int);// transmit Data received
    void changeNum(int);                    // transmit lines having been read
    void transTotal(int);                   // transmit total line number
    void transRate(int);                    // transmit rate of progress

private:
    void run() override;
};

#endif // MYTHREAD_H
