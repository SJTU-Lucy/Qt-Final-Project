#include "countthread.h"
#include <QThread>
#include <QFileDialog>
#include <QTextStream>
#include <QDebug>

void CountThread::run()
{
    for(int i=Start;i<=End;++i)
    {
        totalLine+=data[i].size();
    }
    emit transTotal(totalLine);

    for(int k=Start;k<=End;++k)
    {
        grid.clear();
        countLine++;
        QList<int> tmp;
        tmp.append(-1);
        tmp.append(-1);
        grid.append(tmp);
        for(int i=1;i<data[k].size();++i)
        {
            double pay = data[k][i][7].toDouble();
            int num = judgeNum(data[k][i]);
            int arrive = judgeArrive(data[k][i]);
            if(num == -1)
            {
                tmp[0] = -1;
                tmp[1] = -1;
                grid.append(tmp);
            }
            else
            {
                tmp[0] = num;
                tmp[1] = arrive;
                grid.append(tmp);
                count[num]++;
                income[num]+=pay;
            }
            countLine++;
            //qDebug()<<grid[i][0]<<grid[i][1];
            if(countLine%7==0)
            {
                emit changeNum(countLine);
                emit transRate(countLine*100/totalLine);
            }
        }
        emit transGrid(k,grid);
        //qDebug()<<grid.size();
    }
    emit changeNum(countLine);
    emit transRate(100);
    emit transCount(count);
    emit transIncome(income);
}

int CountThread::judgeNum(QStringList list)
{
    // lng : data 3 -> map 2,4 (2<4)
    // lat : data 4 -> map 3,5 (5<3)
    double lng = list[3].toDouble();
    double lat = list[4].toDouble();
    //qDebug()<<lng<<lat;
    for(int i=0;i<100;++i)
    {
        QStringList tmp = map[i];
        double v2 = tmp[2].toDouble();
        double v3 = tmp[3].toDouble();
        double v4 = tmp[4].toDouble();
        double v5 = tmp[5].toDouble();
        if(lng<v2) continue;
        if(lng>v4) continue;
        if(lat<v5) continue;
        if(lat>v3) continue;
        return i;
    }
    return -1;
}

int CountThread::judgeArrive(QStringList list)
{
    // lng : data 5 -> map 2,4 (2<4)
    // lat : data 6 -> map 3,5 (5<3)
    double lng = list[5].toDouble();
    double lat = list[6].toDouble();
    //qDebug()<<lng<<lat;
    for(int i=0;i<100;++i)
    {
        QStringList tmp = map[i];
        double v2 = tmp[2].toDouble();
        double v3 = tmp[3].toDouble();
        double v4 = tmp[4].toDouble();
        double v5 = tmp[5].toDouble();
        if(lng<v2) continue;
        if(lng>v4) continue;
        if(lat<v5) continue;
        if(lat>v3) continue;
        return i;
    }
    return -1;
}

