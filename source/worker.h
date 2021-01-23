#ifndef WORKER_H
#define WORKER_H

#include <QWidget>
#include <QPushButton>
#include <QApplication>
#include <QFileDialog>
#include <QDir>
#include <QString>
#include <QStringList>
#include <QListWidgetItem>
#include <QSpinBox>
#include <QThread>

class Worker :public QObject
{
public:
    Worker();

protected slots:
    void sieve();

private:
    QThread m_thread;
};

#endif // WORKER_H
