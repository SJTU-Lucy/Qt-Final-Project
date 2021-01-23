#include "similarthread.h"

/*
public:
    QList<QStringList> data[16];
    QList<QStringList> map;
    QStringList target;          //need to solve!

    int Start;
    int End;
    int countLine = 0;
    int totalLine = 0;
    int index = -1;
    int similarity = INT_MAX;


signals:
    void changeNum(int);
    void transTotal(int);
    void transRate(int);
    void transResult(QStringList);
*/


void SimilarThread::run()
{
    tar_len = target[2].toInt() - target[1].toInt();
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

            int tmp = figure(data[k][i]);
            if(tmp <= 20 && tmp > 0)
            {
                Result.append(data[k][i]);
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
    if(!Result.isEmpty())
    {
        emit transResult(Result);
    }
    else
    {
        QList<QStringList> New;
        emit transResult(New);
    }
}

int SimilarThread::figure(QStringList tmp)
{
    int tmp_len   = tmp[2].toInt() - tmp[1].toInt();
    double tmp_lng = tmp[3].toDouble();
    double tmp_lat = tmp[4].toDouble();

    int res = 0;

    res += abs(tmp_lng - target[3].toDouble()) * 1000;
    res += abs(tmp_lat - target[4].toDouble()) * 1000;
    res += abs(tmp_len - tar_len) * 2;

    return res;
}
