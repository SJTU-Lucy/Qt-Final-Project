#include "mythread.h"
#include <QThread>
#include <QFileDialog>
#include <QTextStream>
#include <QDebug>

void MyThread::run()
{
    Result.clear();
    QString ans;
    if(fileName[fileName.size()-5]=='e')
    {
        num = -1;
        ans = "";
    }
    else if(fileName[fileName.size()-6]=='\\')
    {
        ans = fileName.mid(fileName.size()-6,2);
        num = ans.toInt();
    }
    else
    {
        ans = fileName.mid(fileName.size()-5,1);
        num = ans.toInt();
    }

    QFile file(fileName);

    if(ans=="")
    {
        emit transTotal(-1);
        emit changeNum(-1);
        emit transRate(100);
        emit changeList(Result,num);
        return;
    }

    if(file.open(QIODevice::ReadOnly))
    {
        QTextStream stream(&file);
        forever
        {
            QString line = stream.readLine();
            if(line.isEmpty())
            {
                break;
            }
            totalLine++;
        }
        file.close();
    }
    emit transTotal(totalLine);


    if(file.open(QIODevice::ReadOnly))
    {
        QTextStream stream(&file);
        QString line = stream.readLine();
        QStringList row;
        for(const QString& cell: line.split(",")){ //c++11
            row.append(cell.trimmed());
        }
        Result.append(row);
        countLine++;
        forever
        {
            line = stream.readLine();
            if(line.isEmpty()){
                break;
            }
            QStringList row;
            bool flag = false;
            for(const QString& cell: line.split(",")){ //c++11
                if(cell=="grid_id")
                {
                    break;
                    flag = true;
                }
                row.append(cell.trimmed());
            }
            if(!flag) Result.append(row);
            countLine++;
            if(countLine%7==0)
            {
                emit changeNum(countLine);
                emit transRate(countLine*100/totalLine);
            }
        }
        file.close();
    }
    emit changeNum(countLine);
    emit transRate(countLine*100/totalLine);
    emit changeList(Result,num);
}
