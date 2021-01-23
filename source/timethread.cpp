#include "timethread.h"

#include <QDebug>

void TimeThread::run()
{
    for(int i=Start;i<=End;++i)
    {
        totalLine+=data[i].size();
    }
    emit transTotal(totalLine);

    for(int k=Start;k<=End;++k)
    {
        countLine++;
        for(int i=1;i<data[k].size();++i)
        {
            int s = grid[k][i][0];
            int a = grid[k][i][1];
            if(s == S_grid && a == E_grid)
            {
                totalTime += data[k][i][2].toInt() - data[k][i][1].toInt();
                totalNumber++;
            }
            countLine++;
            if(countLine%7==0)
            {
                emit changeNum(countLine);
                emit transRate(countLine*100/totalLine);
            }
        }
    }
    emit changeNum(countLine);
    emit transRate(100);
    if(totalNumber==0)
    {
        emit transAverage(-1);
    }
    else emit transAverage(totalTime/totalNumber);
}
