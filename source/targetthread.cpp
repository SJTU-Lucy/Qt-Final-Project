#include "targetthread.h"
#include <QDebug>

void TargetThread::run()
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
            int num = grid[k][i][0];
            if(num == Grid)
            {
                int des = grid[k][i][1];
                if(des >= 0 && des <= 99) Result[des]++;
            }
            countLine++;
            if(countLine % 7 == 0)
            {
                emit changeNum(countLine);
                emit transRate(countLine * 100 / totalLine);
            }
        }
    }
    emit changeNum(countLine);
    emit transRate(100);
    emit transResult(Result);
}
