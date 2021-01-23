#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mythread.h"
#include "countthread.h"
#include "similarthread.h"
#include "targetthread.h"
#include "timethread.h"
#include <QFileDialog>
#include <QMenuBar>
#include <QApplication>
#include <QFileDialog>
#include <QDir>
#include <QtMath>
#include <QString>
#include <QStringList>
#include <QDebug>
#include <QMessageBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QPieSeries>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarSeries>
#include <QBarCategoryAxis>
#include <QValueAxis>
#include <QtCharts/QAreaSeries>
#include <QMetaType>

QT_CHARTS_USE_NAMESPACE

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // register for variable name
    qRegisterMetaType<QList<QStringList>>("QList<QStringList>");
    qRegisterMetaType<QList<int>>("QList<int>");
    qRegisterMetaType<QList<double>>("QList<double>");
    qRegisterMetaType<QList<QList<int>>>("QList<QList<int>>");

    // set up page look
    ui->Explain_two->hide();
    ui->Lines->hide();
    ui->BoxWidget->hide();
    ui->ChooseWidget->hide();
    ui->RangeWidget->hide();
    ui->InputWidget->hide();
    ui->TableWidget->hide();
    ui->GraphWidget->hide();
    ui->FinishLoad->setText("Finish Loading");
    ui->Total->hide();
    ui->myProgress->hide();
    ui->Explain->hide();
    ui->Time->hide();
    ui->Avg_time->hide();
    ui->Help->show();
    ui->Close->hide();
    this->setWindowTitle("Final Project developed by Lucy");

    initMenu();
    initConnect();

    // trigger "keep_going" when press "Finish Load"
    connect(ui->FinishLoad, SIGNAL(pressed()), this, SLOT(keep_going()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

//////////////////////////definition of slots//////////////////////////////

// slot for LoadData, load your data
void MainWindow::load()
{
    ui->myProgress->show();
    ui->myProgress->setValue(0);
    ui->Lines->show();
    ui->Explain_two->show();

    MyThread *myThread = new MyThread;
    // initialize thread
    QFileDialog dialog;
    QString fileName = dialog.getOpenFileName(0, "Load Data", "/home", "Exce(*.csv)");
    myThread->fileName = fileName;

    Line_count = 0;
    total_Line = 0;

    connect(myThread, SIGNAL(changeNum(int)),                this,          SLOT(Line_Receive(int)));
    connect(myThread, SIGNAL(changeList(QList<QStringList>,int))
                      ,this,          SLOT(Data_Receive(QList<QStringList>,int)));
    connect(myThread, SIGNAL(transTotal(int)),               this,          SLOT(Total_Receive(int)));
    connect(myThread, SIGNAL(changeNum(int)),                ui->Lines,     SLOT(setNum(int)));
    connect(myThread, SIGNAL(transRate(int)),                ui->myProgress,SLOT(setValue(int)));

    ui->myProgress->setRange(0,100);

    myThread->start();
}

// slot for LoadMap, load your map
void MainWindow::mapLoad()
{
    ui->myProgress->setRange(0,100);
    ui->myProgress->setValue(0);
    ui->myProgress->show();
    ui->Lines->show();
    ui->Explain_two->show();

    QFileDialog dialog;
    QString fileName = dialog.getOpenFileName(0, "Load Map", "/home", "Exce(*.csv)");
    QFile file(fileName);
    // if the file isn't map, give a warning
    if(fileName[fileName.size()-5]!='e')
    {

        QMessageBox msgBox;
        msgBox.setText("Please Load The Right Data!");
        msgBox.exec();
        ui->Lines->setNum(-1);
        ui->myProgress->setValue(100);
        return;
    }

    Line_count = 0;
    total_Line = 0;

    // similar to myThread.run()
    if(file.open(QIODevice::ReadOnly))
    {
        QTextStream stream(&file);
        QString line;
        forever
        {
            line = stream.readLine();
            if(line.isEmpty()){
                break;
            }
            total_Line++;
        }
        file.close();
    }

    if(file.open(QIODevice::ReadOnly))
    {
        QTextStream stream(&file);
        QString line = stream.readLine();
        QStringList row;
        forever
        {
            line = stream.readLine();
            if(line.isEmpty()){
                break;
            }
            QStringList row;
            for(const QString& cell: line.split(",")){ //c++11
                row.append(cell.trimmed());
            }
            map.append(row);
            Line_count++;
            if(Line_count%3==0)
            {
                ui->Lines->setNum(Line_count);
                ui->myProgress->setValue(Line_count*100/total_Line);
            }
        }
        file.close();
        ui->Lines->setNum(total_Line);
        ui->myProgress->setValue(100);
    }
}

// slot for Table, make table
void MainWindow::chart()
{
    NumOrPay = 0;
    ui->InputWidget->hide();
    ui->RangeWidget->hide();
    ui->BoxWidget->show();
    ui->Avg_time->hide();
    ui->Time->hide();
    ui->GraphWidget->hide();
    ui->TableWidget->clear();
    ui->TableWidget->show();
    ui->Total->hide();
    ui->Explain->hide();

    if(data[Start_Date].isEmpty())
    {
        notLoad();
        return;
    }

    int line = 0;
    int column = data[Start_Date][0].size();

    for(int m = Start_Date;m<=End_Date;++m)
    {
        if(data[m].isEmpty()) continue;
        line += data[m].size() - 1;
    }

    ui->TableWidget->setRowCount(line-1);                               // set up row number
    ui->TableWidget->setColumnCount(column);                            // set up column number
    ui->TableWidget->setHorizontalHeaderLabels(data[Start_Date][0]);    // initialize header of the chart

    int row = 0;

    for(int m = Start_Date;m<=End_Date;++m)
    {
        for(int i=1;i<data[m].size();++i)
        {
            for(int j=0;j<column;++j)
            {
                ui->TableWidget->setItem(row,j,new QTableWidgetItem(data[m][i][j]));
            }
            row++;
        }
    }

    ui->BoxWidget->show();
}

// slot for Line Chart, switch graph to line chart
void MainWindow::line()
{
    graphStyle = 0;
    if(NumOrPay==1) analysis();
    else if(NumOrPay==2) income();
}

// slot for Bar Chart, switch graph to bar chart
void MainWindow::column()
{
    graphStyle = 1;
    if(NumOrPay==1) analysis();
    else if(NumOrPay==2) income();
}

// slot for Pie Chart, switch graph to pie chart
void MainWindow::round()
{
    graphStyle = 2;
    if(NumOrPay==1) analysis();
    else if(NumOrPay==2) income();
}

// slot for Count, draw graph for count data
void MainWindow::analysis()
{
    NumOrPay = 1;
    ui->InputWidget->hide();
    ui->BoxWidget->hide();
    ui->RangeWidget->show();
    ui->Time->hide();
    ui->Avg_time->hide();
    ui->TableWidget->hide();

    if(data[Start_Date].isEmpty())
    {
        notLoad();
        return;
    }

    if(graphStyle==0)
    {
        QLineSeries *series = new QLineSeries();
        series->setPen(QPen(Qt::blue,3,Qt::SolidLine)); //设置画笔

        if(End_Grid-Start_Grid<=5)
        {
            for(int k=Start_Grid;k<End_Grid;++k)
            {
                double tmp = k;
                *series << QPointF(k,cnt_total[k]);
                if(cnt_total[k+1]>cnt_total[k])
                {
                    *series << QPointF(tmp+0.5,(2*cnt_total[k]+cnt_total[k+1])/3);
                }
                else
                {
                    *series << QPointF(tmp+0.5,(cnt_total[k]+2*cnt_total[k+1])/3);
                }
            }
            *series << QPointF(End_Grid,cnt_total[End_Grid]);
        }
        else for(int k=Start_Grid;k<=End_Grid;++k)
        {
            *series << QPointF(k,cnt_total[k]);
        }
        series->setVisible(true);
        //点标签是否可视
        series->setPointLabelsVisible(true);
        //点标签颜色
        series->setPointLabelsColor(QColor(0,0,0));
        //点标签字体
        series->setPointLabelsFont(QFont("微软雅黑",8));
        //设置点标签显示格式
        series->setPointLabelsFormat("(@xPoint,@yPoint)");
        //设置点标签是否可视
        series->setPointsVisible(false);
        //是否切割边缘点标签，默认为true
        series->setPointLabelsClipping(false);
        QChart *chart = new QChart();
        chart->addSeries(series);
        chart->setTitle("Line Chart of Total Order");
        chart->setTitleBrush(QBrush(QColor(0,0,255)));//设置标题Brush
        chart->setTitleFont(QFont("微软雅黑"));//设置标题字体
        chart->createDefaultAxes();   //设置默认的坐标系，如果不设置程序是运行不了的
        ui->GraphWidget->setRenderHint(QPainter::Antialiasing);  //抗锯齿渲染
        ui->GraphWidget->setChart(chart);
    }
    else if(graphStyle == 1)
    {
        QChart* m_chart = new QChart();

        QBarSet *set = new QBarSet("Grid Id");

        for(int k=Start_Grid;k<=End_Grid;++k)
        {
            *set << cnt_total[k];
        }

        set->setLabelColor(QColor(0,0,255));       //设置条形数据颜色

        QBarSeries *series = new QBarSeries();
        series->append(set);

        series->setVisible(true);
        series->setLabelsVisible(true);

        m_chart->setTheme(QChart::ChartThemeLight);//设置白色主题
        m_chart->setDropShadowEnabled(true);//背景阴影
        m_chart->addSeries(series);//添加系列到QChart上

        m_chart->setTitleBrush(QBrush(QColor(0,0,255)));//设置标题Brush
        m_chart->setTitleFont(QFont("微软雅黑"));//设置标题字体
        m_chart->setTitle("Bar Chart of Regional Total Order");

        //创建X轴和Y轴
        QBarCategoryAxis *axisX = new QBarCategoryAxis;
        for(int i=Start_Grid;i<=End_Grid;++i)
        {
            axisX->append(QString::number(i));
        }
        axisX->setLabelsColor(QColor(7,28,96));

        QValueAxis *axisY = new QValueAxis;
        axisY->setRange(0,-1);   //改为setRange(0,1);则表示坐标为动态计算大小的
        axisY->setTitleText("订单总数");
        axisY->setLabelFormat("%d");

        m_chart->setAxisX(axisX,series);
        m_chart->setAxisY(axisY,series);

        ui->GraphWidget->setChart(m_chart);
    }
    else
    {
        QChart* m_chart = new QChart();
        QPieSeries *series = new QPieSeries();

        for(int i=Start_Grid;i<=End_Grid;++i)
        {
            series->append(QString::number(i),cnt_total[i]);
        }

        series->setLabelsVisible(true);
        series->setUseOpenGL(true);
        for(int i = 0;i <= End_Grid - Start_Grid;++i)
        {
            series->slices().at(i)->setColor(QColor((31*i)%255,(31*i+85)%255,(31*i+170)%255)); //设置颜色
            series->slices().at(i)->setLabelColor(QColor((31*i)%255,(31*i+85)%255,(31*i+170)%255));
        }

        m_chart->setTheme(QChart::ChartThemeLight);//设置白色主题
        m_chart->setDropShadowEnabled(true);//背景阴影
        m_chart->addSeries(series);//添加系列到QChart上

        m_chart->setTitleBrush(QBrush(QColor(0,0,255)));//设置标题Brush
        m_chart->setTitleFont(QFont("微软雅黑"));//设置标题字体
        m_chart->setTitle("Bar Chart of Regional Total Order");

        ui->GraphWidget->setChart(m_chart);
    }

    int total = 0;

    for(int i=Start_Grid;i<=End_Grid;++i)
    {
        total += cnt_total[i];
    }


    ui->Total->setNum(total);
    ui->Total->show();
    ui->Explain->setText("The Total Number is :");
    ui->Explain->show();

    ui->GraphWidget->show();
}

// slot for Income, draw graph for income data
void MainWindow::income()
{
    NumOrPay = 2;
    ui->InputWidget->hide();
    ui->BoxWidget->hide();
    ui->RangeWidget->show();
    ui->Time->hide();
    ui->Avg_time->hide();
    ui->TableWidget->hide();

    if(data[Start_Date].isEmpty())
    {
        notLoad();
        return;
    }

    if(graphStyle==0)
    {
        QLineSeries *series = new QLineSeries();
        series->setPen(QPen(Qt::blue,3,Qt::SolidLine)); //设置画笔
        if(End_Grid-Start_Grid<=5)
        {
            for(int k=Start_Grid;k<End_Grid;++k)
            {
                double tmp = k;
                *series << QPointF(k,income_total[k]);
                if(income_total[k+1]>income_total[k])
                {
                    *series << QPointF(tmp+0.5,(2*income_total[k]+income_total[k+1])/3);
                }
                else
                {
                    *series << QPointF(tmp+0.5,(income_total[k]+2*income_total[k+1])/3);
                }
            }
            *series << QPointF(End_Grid,income_total[End_Grid]);
        }
        else for(int k=Start_Grid;k<=End_Grid;++k)
        {
            *series << QPointF(k,income_total[k]);
        }
        series->setVisible(true);
        //点标签是否可视
        series->setPointLabelsVisible(true);
        //点标签颜色
        series->setPointLabelsColor(QColor(0,0,0));
        //点标签字体
        series->setPointLabelsFont(QFont("微软雅黑",8));
        //设置点标签显示格式
        series->setPointLabelsFormat("(@xPoint,@yPoint)");
        //设置点标签是否可视
        series->setPointsVisible(false);
        //是否切割边缘点标签，默认为true
        series->setPointLabelsClipping(false);
        QChart *chart = new QChart();
        chart->addSeries(series);
        chart->setTitle("Line Chart of Regional Gross Revenue");
        chart->setTitleBrush(QBrush(QColor(0,0,255)));//设置标题Brush
        chart->setTitleFont(QFont("微软雅黑"));//设置标题字体
        chart->createDefaultAxes();   //设置默认的坐标系，如果不设置程序是运行不了的
        ui->GraphWidget->setRenderHint(QPainter::Antialiasing);  //抗锯齿渲染
        ui->GraphWidget->setChart(chart);
    }
    else if(graphStyle == 1)
    {
        QChart* m_chart = new QChart();

        //创建3个条线数据
        QBarSet *set = new QBarSet("Grid Id");

        for(int k=Start_Grid;k<=End_Grid;++k)
        {
            *set << income_total[k];
        }

        set->setLabelColor(QColor(0,0,255));       //设置条形数据颜色

        QBarSeries *series = new QBarSeries();
        series->append(set);

        series->setVisible(true);
        series->setLabelsVisible(true);

        m_chart->setTheme(QChart::ChartThemeLight);//设置白色主题
        m_chart->setDropShadowEnabled(true);//背景阴影
        m_chart->addSeries(series);//添加系列到QChart上

        m_chart->setTitleBrush(QBrush(QColor(0,0,255)));//设置标题Brush
        m_chart->setTitleFont(QFont("微软雅黑"));//设置标题字体
        m_chart->setTitle("Bar Chart of Regional Gross Revenue");

        //创建X轴和Y轴
        QBarCategoryAxis *axisX = new QBarCategoryAxis;
        for(int i=Start_Grid;i<=End_Grid;++i)
        {
            axisX->append(QString::number(i));
        }

        axisX->setLabelsColor(QColor(7,28,96));

        QValueAxis *axisY = new QValueAxis;
        axisY->setRange(0,-1);   //改为setRange(0,1);则表示坐标为动态计算大小的
        axisY->setTitleText("Regional Gross Revenue");
        axisY->setLabelFormat("%d");

        m_chart->setAxisX(axisX,series);
        m_chart->setAxisY(axisY,series);

        ui->GraphWidget->setChart(m_chart);
    }
    else
    {
        QChart* m_chart = new QChart();
        QPieSeries *series = new QPieSeries();

        for(int i=Start_Grid;i<=End_Grid;++i)
        {
            series->append(QString::number(i),income_total[i]);
        }

        series->setLabelsVisible(true);
        series->setUseOpenGL(true);
        for(int i = 0;i <= End_Grid - Start_Grid;++i)
        {
            series->slices().at(i)->setColor(QColor((31*i)%255,(31*i+85)%255,(31*i+170)%255)); //设置颜色
            series->slices().at(i)->setLabelColor(QColor((31*i)%255,(31*i+85)%255,(31*i+170)%255));
        }

        m_chart->setTheme(QChart::ChartThemeLight);//设置白色主题
        m_chart->setDropShadowEnabled(true);//背景阴影
        m_chart->addSeries(series);//添加系列到QChart上

        m_chart->setTitleBrush(QBrush(QColor(0,0,255)));//设置标题Brush
        m_chart->setTitleFont(QFont("微软雅黑"));//设置标题字体
        m_chart->setTitle("Pie Chart of Regional Gross Revenue");

        ui->GraphWidget->setChart(m_chart);
    }

    double total = 0;

    for(int i=Start_Grid;i<=End_Grid;++i)
    {
        total += income_total[i];
    }

    ui->Total->setText(QString::number(total));
    ui->Total->show();
    ui->Explain->setText("The Total Revenue is :");
    ui->Explain->show();

    ui->GraphWidget->show();
}

// slot for Similar, start similarThread
void MainWindow::similar()
{
    ui->InputWidget->hide();
    ui->Avg_time->hide();
    ui->Time->hide();

    SimilarThread *similarThread = new SimilarThread;

    QStringList tmp;

    int row = ui->TableWidget->currentRow();

    for(int i=0;i<data[Start_Date][0].size();++i)
    {
        tmp.append(ui->TableWidget->item(row,i)->text());
    }

    Line_count = 0;
    total_Line = 0;

    connect(similarThread, SIGNAL(changeNum(int)),                  this,           SLOT(Line_Receive(int)));
    connect(similarThread, SIGNAL(transTotal(int)),                 this,           SLOT(Total_Receive(int)));
    connect(similarThread, SIGNAL(changeNum(int)),                  ui->Lines,      SLOT(setNum(int)));
    connect(similarThread, SIGNAL(transRate(int)),                  ui->myProgress, SLOT(setValue(int)));
    connect(similarThread, SIGNAL(transResult(QList<QStringList>)), this,           SLOT(start_similar(QList<QStringList>)));

    ui->myProgress->setRange(0,100);
    ui->myProgress->setValue(0);

    similarThread->Start = Start_Date;
    similarThread->End   = End_Date;
    similarThread->map   = map;


    similarThread->target = tmp;

    for(int i=1;i<=15;++i)
    {
        similarThread->data[i] = data[i];
    }

    similarThread->start();
}

// slot for Target, initialize range input box
void MainWindow::target()
{
    ui->InputWidget->show();
    ui->InputWidget->clear();
    ui->Avg_time->hide();
    ui->Time->hide();

    QSlider *startSlider    = new QSlider(this);
    QSpinBox *startSpin     = new QSpinBox(this);
    QPushButton *Update     = new QPushButton(this);

    QListWidgetItem *startItem      = new QListWidgetItem();
    QListWidgetItem *startBox       = new QListWidgetItem();
    QListWidgetItem *UpdateItem     = new QListWidgetItem();

    ui->InputWidget->addItem(startBox);
    ui->InputWidget->addItem(startItem);
    ui->InputWidget->addItem(UpdateItem);

    ui->InputWidget->setItemWidget(UpdateItem,Update);
    ui->InputWidget->setItemWidget(startBox,startSpin);
    ui->InputWidget->setItemWidget(startItem,startSlider);

    startSlider->setTickPosition(QSlider::TicksBelow);

    startSlider->setOrientation(Qt::Horizontal);  // 水平方向
    startSlider->setMinimum(0);  // 最小值
    startSlider->setMaximum(99);  // 最大值
    startSlider->setSingleStep(1);  // 步长

    startSpin->setMinimum(0);  // 最小值
    startSpin->setMaximum(99);  // 最大值
    startSpin->setSingleStep(1);  // 步长

    Update->setText("Update");

    connect(startSpin,      SIGNAL(valueChanged(int)), startSlider, SLOT(setValue(int)));
    connect(startSlider,    SIGNAL(valueChanged(int)), startSpin,   SLOT(setValue(int)));
    connect(startSlider,    SIGNAL(valueChanged(int)), this,        SLOT(UpdateTarget(int)));

    connect(Update, SIGNAL(pressed()), this, SLOT(process_target()));
}

// slot for Time, initialize range input box
void MainWindow::time()
{
    ui->InputWidget->show();
    ui->InputWidget->clear();
    ui->Avg_time->show();
    ui->Time->show();

    QSlider *startSlider    = new QSlider(this);
    QSlider *endSlider      = new QSlider(this);
    QSpinBox *startSpin     = new QSpinBox(this);
    QSpinBox *endSpin       = new QSpinBox(this);
    QPushButton *Update     = new QPushButton(this);

    QListWidgetItem *startItem      = new QListWidgetItem();
    QListWidgetItem *endItem        = new QListWidgetItem();
    QListWidgetItem *startBox       = new QListWidgetItem();
    QListWidgetItem *endBox         = new QListWidgetItem();
    QListWidgetItem *UpdateItem     = new QListWidgetItem();

    ui->InputWidget->addItem(startBox);
    ui->InputWidget->addItem(startItem);
    ui->InputWidget->addItem(endBox);
    ui->InputWidget->addItem(endItem);
    ui->InputWidget->addItem(UpdateItem);

    ui->InputWidget->setItemWidget(UpdateItem,Update);
    ui->InputWidget->setItemWidget(startBox,startSpin);
    ui->InputWidget->setItemWidget(endBox,endSpin);
    ui->InputWidget->setItemWidget(startItem,startSlider);
    ui->InputWidget->setItemWidget(endItem,endSlider);

    startSlider->setTickPosition(QSlider::TicksBelow);
    endSlider->setTickPosition(QSlider::TicksBelow);

    startSlider->setOrientation(Qt::Horizontal);  // 水平方向
    startSlider->setMinimum(0);  // 最小值
    startSlider->setMaximum(99);  // 最大值
    startSlider->setSingleStep(1);  // 步长

    endSlider->setOrientation(Qt::Horizontal);  // 水平方向
    endSlider->setMinimum(0);  // 最小值
    endSlider->setMaximum(99);  // 最大值
    endSlider->setSingleStep(1);  // 步长

    startSpin->setMinimum(0);  // 最小值
    startSpin->setMaximum(99);  // 最大值
    startSpin->setSingleStep(1);  // 步长

    endSpin->setMinimum(0);  // 最小值
    endSpin->setMaximum(99);  // 最大值
    endSpin->setSingleStep(1);  // 步长

    endSlider->setValue(99);
    endSpin->setValue(99);

    Update->setText("Update");

    connect(startSpin,      SIGNAL(valueChanged(int)), startSlider, SLOT(setValue(int)));
    connect(startSlider,    SIGNAL(valueChanged(int)), startSpin,   SLOT(setValue(int)));
    connect(endSpin,        SIGNAL(valueChanged(int)), endSlider,   SLOT(setValue(int)));
    connect(endSlider,      SIGNAL(valueChanged(int)), endSpin,     SLOT(setValue(int)));
    connect(startSlider,    SIGNAL(valueChanged(int)), this,        SLOT(UpdateTimeStart(int)));
    connect(endSlider,      SIGNAL(valueChanged(int)), this,        SLOT(UpdateTimeEnd(int)));

    connect(Update, SIGNAL(pressed()), this, SLOT(process_time()));
}

// slot for Help, show start page
void MainWindow::help()
{
    ui->Help->show();
    ui->Close->show();

    connect(ui->Close,SIGNAL(pressed()),this,SLOT(close()));
}

// slot for input start date
void MainWindow::UpdateLeft(int val)
{
    Start_Date = val;
}

// slot for input end date
void MainWindow::UpdateRight(int val)
{
    End_Date = val;
}

// slot for input start grid
void MainWindow::UpdateStart(int val)
{
    Start_Grid = val;
}

// slot for input end grid
void MainWindow::UpdateEnd(int val)
{
    End_Grid = val;
}

// slot for input start grid in Time
void MainWindow::UpdateTimeStart(int val)
{
    time_start = val;
}

// slot for input end grid in Time
void MainWindow::UpdateTimeEnd(int val)
{
    time_end = val;
}

// slot for input start grid in Target
void MainWindow::UpdateTarget(int val)
{
    target_grid = val;
}

// slot for receive total line number from thread
void MainWindow::Total_Receive(int a)
{
    total_Line = a;
}

// slot for receive current line number from thread
void MainWindow::Line_Receive(int a)
{
    Line_count = a;
}

// slot for receive data emit from myThread
void MainWindow::Data_Receive(QList<QStringList> tmp, int num)
{
    if(num==-1)
    {
        QMessageBox msgBox;
        msgBox.setText("Please Load The Right Data!");
        msgBox.exec();
    }
    else data[num] = tmp;
}

// slot for receive total order number from countThread
void MainWindow::Count_Receive(QList<int> val)
{
    for(int i=0;i<100;++i)
    {
        cnt_total[i] = val[i];
    }
}

// slot for receive grid ID from countThread
void MainWindow::Grid_Receive(int num, QList<QList<int>> val)
{
    grid[num] = val;
}

// slot for receive revenue from countThread
void MainWindow::Income_Receive(QList<double> val)
{
    for(int i=0;i<100;++i)
    {
        income_total[i] = val[i];
    }
}

// connect with "FinishLoad" button
void MainWindow::keep_going()
{
    int count = 0;
    for(int i=0;i<16;++i)
    {
        count += data[i].size();
    }
    if(count == 0||map.size()==0)
    {
        notLoad();
    }
    else
    {
        for(int i=0;i<16;++i)
        {
            if(!data[i].isEmpty())
            {
                Start_Date = i;
                break;
            }
        }
        ui->FinishLoad->hide();
        initCheckBox();
        initSlider();
        initRange();
    }
}

// connect with "similar", make chart
void MainWindow::start_similar(QList<QStringList> val)
{
    ui->GraphWidget->hide();
    ui->TableWidget->clear();
    ui->TableWidget->hide();
    ui->Time->hide();
    ui->Avg_time->hide();
    ui->Explain->hide();
    ui->Total->hide();

    int line = val.size();
    int column = data[Start_Date][0].size();

    ui->TableWidget->setRowCount(line-1);
    ui->TableWidget->setColumnCount(column);
    ui->TableWidget->setHorizontalHeaderLabels(data[Start_Date][0]);

    for(int i=0;i<line;++i)
    {
        for(int j=0;j<column;++j)
        {
            ui->TableWidget->setItem(i,j,new QTableWidgetItem(val[i][j]));
        }
    }

    ui->TableWidget->show();

}

// connect with "process_target", make graph
void MainWindow::start_target(QList<int> val)
{
    ui->TableWidget->hide();
    ui->GraphWidget->hide();
    ui->Time->hide();
    ui->Avg_time->hide();
    ui->Explain->hide();
    ui->Total->hide();

    QChart* m_chart = new QChart();
    QPieSeries *series = new QPieSeries();

    for(int i=0;i<100;++i)
    {
        series->append(QString::number(i),val[i]);
    }

    series->setLabelsVisible(true);
    series->setUseOpenGL(true);
    for(int i = 0;i < 100;++i)
    {
        series->slices().at(i)->setColor(QColor((31*i)%255,(31*i+85)%255,(31*i+170)%255));
        series->slices().at(i)->setLabelColor(QColor((31*i)%255,(31*i+85)%255,(31*i+170)%255));
    }

    m_chart->setTheme(QChart::ChartThemeLight);
    m_chart->setDropShadowEnabled(true);
    m_chart->addSeries(series);

    m_chart->setTitleBrush(QBrush(QColor(0,0,255)));
    m_chart->setTitleFont(QFont("微软雅黑"));
    m_chart->setTitle("Possible Targets");

    ui->GraphWidget->setChart(m_chart);

    ui->GraphWidget->show();
}

// connect with "target", start targetThread
void MainWindow::process_target()
{
    TargetThread *targetThread = new TargetThread;

    Line_count = 0;
    total_Line = 0;

    connect(targetThread, SIGNAL(changeNum(int)),          this,           SLOT(Line_Receive(int)));
    connect(targetThread, SIGNAL(transTotal(int)),         this,           SLOT(Total_Receive(int)));
    connect(targetThread, SIGNAL(changeNum(int)),          ui->Lines,      SLOT(setNum(int)));
    connect(targetThread, SIGNAL(transRate(int)),          ui->myProgress, SLOT(setValue(int)));
    connect(targetThread, SIGNAL(transResult(QList<int>)), this,           SLOT(start_target(QList<int>)));

    ui->myProgress->setRange(0,100);
    ui->myProgress->setValue(0);

    targetThread->Start = Start_Date;
    targetThread->End   = End_Date;
    targetThread->map   = map;

    for(int i=Start_Date;i<=End_Date;++i)
    {
        targetThread->grid[i] = grid[i];
        targetThread->data[i] = data[i];
    }

    targetThread->Grid = target_grid;

    for(int i=0;i<100;++i)
    {
        targetThread->Result.append(0);
    }
    targetThread->start();
}

// connect witg "time", start timeThread
void MainWindow::process_time()
{
    TimeThread *timeThread = new TimeThread;

    Line_count = 0;
    total_Line = 0;

    connect(timeThread, SIGNAL(changeNum(int)),          this,           SLOT(Line_Receive(int)));
    connect(timeThread, SIGNAL(transTotal(int)),         this,           SLOT(Total_Receive(int)));
    connect(timeThread, SIGNAL(changeNum(int)),          ui->Lines,      SLOT(setNum(int)));
    connect(timeThread, SIGNAL(transRate(int)),          ui->myProgress, SLOT(setValue(int)));
    connect(timeThread, SIGNAL(transAverage(int)),       this,           SLOT(start_time(int)));

    ui->myProgress->setRange(0,100);
    ui->myProgress->setValue(0);

    timeThread->Start = Start_Date;
    timeThread->End   = End_Date;
    timeThread->map   = map;

    timeThread->S_grid = time_start;
    timeThread->E_grid = time_end;

    for(int i=1;i<=15;++i)
    {
        timeThread->data[i] = data[i];
        timeThread->grid[i] = grid[i];
    }

    timeThread->start();
}

// connect with "process_time", show result
void MainWindow::start_time(int average)
{
    ui->Time->show();
    ui->Avg_time->show();
    ui->Explain->hide();
    ui->Total->hide();
    if(average==-1)
    {
        ui->Time->setText("no samples found");
    }
    else
    {
        ui->Time->setNum(average);
    }
}

// decide which action should be done again after updating range
void MainWindow::decide()
{
    ui->Help->hide();
    ui->ChooseWidget->hide();
    if(NumOrPay==0)
    {
        chart();
        initCount();
    }
    else if(NumOrPay==1)
    {
        analysis();
    }
    else
    {
        income();
    }
}

// figure out the start Grid ID for each order
int MainWindow::judgeNum(QStringList list)
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

// connect with push on Choose Box
void MainWindow::changeButtonStatus()
{
    //关键一步是利用sender()
    QCheckBox *button = (QCheckBox * )(sender());
    QString target = button->text();
    int index;
    for(int i=0;i<data[Start_Date][0].size();++i)
    {
        if(data[Start_Date][0][i]==target)
        {
            index = i;
            break;
        }
        else continue;
    }
    if(button->checkState()==Qt::Checked)
    {
        ui->TableWidget->setColumnHidden(index,false);
    }
    else if(button->checkState()==Qt::Unchecked)
    {
        ui->TableWidget->setColumnHidden(index,true);
    }
}

 // close start page in Help mode
void MainWindow::close()
{
    ui->Close->hide();
    ui->Help->hide();
}

//////////////////////////private functions//////////////////////////////
// an inline fuction to calculate the smaller one between two
inline int MainWindow::min(int a,int b)
{
    if(a<=b) return a;
    else return b;
}

 // an inline fuction to calculate the bigger one between two
inline int MainWindow::max(int a, int b)
{
    if(a>=b) return a;
    else return b;
}

// initialize the menubar
void MainWindow::initMenu()
{
    loadAct     = new QAction(tr("&LoadData"),      this);
    mapLoadAct  = new QAction(tr("&LoadMap"),       this);
    lineAct     = new QAction(tr("&LineChart"),     this);
    columnAct   = new QAction(tr("&BarChart"),      this);
    roundAct    = new QAction(tr("&PieChart"),      this);
    analysisAct = new QAction(tr("&Count"),         this);
    IncomeAct   = new QAction(tr("&Income"),        this);
    chartAct    = new QAction(tr("&Table"),         this);
    similarAct  = new QAction(tr("&Similar"),       this);
    targetAct   = new QAction(tr("&Target"),        this);
    timeAct     = new QAction(tr("&Time"),          this);
    HelpAct     = new QAction(tr("&Help"),          this);

    fileMenu        = new QMenu(tr("&File"),        this);
    graphMenu       = new QMenu(tr("&Choose"),      this);
    analysisMenu    = new QMenu(tr("&Analysis"),    this);
    moreMenu        = new QMenu(tr("&More"),        this);
    HelpMenu        = new QMenu(tr("&Help"),        this);

    ui->menubar->addMenu(fileMenu);
    ui->menubar->addMenu(graphMenu);
    ui->menubar->addMenu(analysisMenu);
    ui->menubar->addMenu(moreMenu);
    ui->menubar->addMenu(HelpMenu);


    fileMenu    -> addAction(loadAct);
    fileMenu    -> addAction(mapLoadAct);
    graphMenu   -> addAction(lineAct);
    graphMenu   -> addAction(columnAct);
    graphMenu   -> addAction(roundAct);
    analysisMenu-> addAction(analysisAct);
    analysisMenu-> addAction(IncomeAct);
    analysisMenu-> addAction(chartAct);
    moreMenu    -> addAction(similarAct);
    moreMenu    -> addAction(targetAct);
    moreMenu    -> addAction(timeAct);
    HelpMenu    -> addAction(HelpAct);
}

// initialize connect for menubar
void MainWindow::initConnect()
{
    connect(loadAct,        SIGNAL(triggered()),this,SLOT(load()));
    connect(mapLoadAct,     SIGNAL(triggered()),this,SLOT(mapLoad()));
    connect(lineAct,        SIGNAL(triggered()),this,SLOT(line()));
    connect(columnAct,      SIGNAL(triggered()),this,SLOT(column()));
    connect(roundAct,       SIGNAL(triggered()),this,SLOT(round()));
    connect(analysisAct,    SIGNAL(triggered()),this,SLOT(analysis()));
    connect(IncomeAct,      SIGNAL(triggered()),this,SLOT(income()));
    connect(chartAct,       SIGNAL(triggered()),this,SLOT(chart()));
    connect(similarAct,     SIGNAL(triggered()),this,SLOT(similar()));
    connect(targetAct,      SIGNAL(triggered()),this,SLOT(target()));
    connect(timeAct,        SIGNAL(triggered()),this,SLOT(time()));
    connect(HelpAct,        SIGNAL(triggered()),this,SLOT(help()));
}

// warning window for loading not necessary data
void MainWindow::notLoad()
{
    QMessageBox msgBox;
    msgBox.setText("Please load data first!");
    msgBox.exec();
}

// initialize chooseBox for Table
void MainWindow::initCheckBox()
{
    int n = data[Start_Date][0].size();

    QCheckBox* Box[n];
    QListWidgetItem* Item[n];

    for(int i=0;i<n;++i)
    {
        Box[i]  = new QCheckBox(data[Start_Date][0][i],this);
        Box[i]->setCheckable(true);
        Item[i] = new QListWidgetItem();
        ui->BoxWidget -> addItem(Item[i]);
        ui->BoxWidget -> setItemWidget(Item[i],Box[i]);
        Box[i]->setCheckState(Qt::Checked);
        connect(Box[i],SIGNAL(clicked()),this,SLOT(changeButtonStatus()));
    }
    ui->BoxWidget->hide();
}

// initialize input widget for date
void MainWindow::initSlider()
{
    QSlider *leftSlider  = new QSlider(this);
    QSlider *rightSlider = new QSlider(this);
    QSpinBox *LeftSpin   = new QSpinBox(this);
    QSpinBox *RightSpin  = new QSpinBox(this);
    QPushButton *Finish = new QPushButton(this);  

    QListWidgetItem *leftItem   = new QListWidgetItem();
    QListWidgetItem *rightItem  = new QListWidgetItem();
    QListWidgetItem *leftBox    = new QListWidgetItem();
    QListWidgetItem *rightBox   = new QListWidgetItem();
    QListWidgetItem *ButtonItem = new QListWidgetItem();

    ui->ChooseWidget->addItem(leftBox);
    ui->ChooseWidget->addItem(leftItem);
    ui->ChooseWidget->addItem(rightBox);
    ui->ChooseWidget->addItem(rightItem);
    ui->ChooseWidget->addItem(ButtonItem);

    ui->ChooseWidget->setItemWidget(ButtonItem,Finish);
    ui->ChooseWidget->setItemWidget(leftBox,LeftSpin);
    ui->ChooseWidget->setItemWidget(rightBox,RightSpin);
    ui->ChooseWidget->setItemWidget(leftItem,leftSlider);
    ui->ChooseWidget->setItemWidget(rightItem,rightSlider);

    leftSlider->setTickPosition(QSlider::TicksBelow);
    rightSlider->setTickPosition(QSlider::TicksBelow);

    leftSlider->setOrientation(Qt::Horizontal);  // 水平方向
    leftSlider->setMinimum(1);  // 最小值
    leftSlider->setMaximum(15);  // 最大值
    leftSlider->setSingleStep(1);  // 步长

    rightSlider->setOrientation(Qt::Horizontal);  // 水平方向
    rightSlider->setMinimum(1);  // 最小值
    rightSlider->setMaximum(15);  // 最大值
    rightSlider->setSingleStep(1);  // 步长

    // 微调框

    LeftSpin->setMinimum(1);  // 最小值
    LeftSpin->setMaximum(15);  // 最大值
    LeftSpin->setSingleStep(1);  // 步长

    RightSpin->setMinimum(1);  // 最小值
    RightSpin->setMaximum(15);  // 最大值
    RightSpin->setSingleStep(1);  // 步长

    Finish->setText("OK");

    connect(LeftSpin,       SIGNAL(valueChanged(int)), leftSlider,  SLOT(setValue(int)));
    connect(leftSlider,     SIGNAL(valueChanged(int)), LeftSpin,    SLOT(setValue(int)));
    connect(RightSpin,      SIGNAL(valueChanged(int)), rightSlider, SLOT(setValue(int)));
    connect(rightSlider,    SIGNAL(valueChanged(int)), RightSpin,   SLOT(setValue(int)));
    connect(leftSlider,     SIGNAL(valueChanged(int)), this,        SLOT(UpdateLeft(int)));
    connect(rightSlider,    SIGNAL(valueChanged(int)), this,        SLOT(UpdateRight(int)));

    connect(Finish, SIGNAL(pressed()), this, SLOT(decide()));

    ui->ChooseWidget->show();
}

// initialize input widget for ID
void MainWindow::initRange()
{
    QSlider *startSlider    = new QSlider(this);
    QSlider *endSlider      = new QSlider(this);
    QSpinBox *startSpin     = new QSpinBox(this);
    QSpinBox *endSpin       = new QSpinBox(this);
    QPushButton *Update     = new QPushButton(this);

    QListWidgetItem *startItem      = new QListWidgetItem();
    QListWidgetItem *endItem        = new QListWidgetItem();
    QListWidgetItem *startBox       = new QListWidgetItem();
    QListWidgetItem *endBox         = new QListWidgetItem();
    QListWidgetItem *UpdateItem     = new QListWidgetItem();

    ui->RangeWidget->addItem(startBox);
    ui->RangeWidget->addItem(startItem);
    ui->RangeWidget->addItem(endBox);
    ui->RangeWidget->addItem(endItem);
    ui->RangeWidget->addItem(UpdateItem);

    ui->RangeWidget->setItemWidget(UpdateItem,Update);
    ui->RangeWidget->setItemWidget(startBox,startSpin);
    ui->RangeWidget->setItemWidget(endBox,endSpin);
    ui->RangeWidget->setItemWidget(startItem,startSlider);
    ui->RangeWidget->setItemWidget(endItem,endSlider);

    startSlider->setTickPosition(QSlider::TicksBelow);
    endSlider->setTickPosition(QSlider::TicksBelow);

    startSlider->setOrientation(Qt::Horizontal);  // 水平方向
    startSlider->setMinimum(0);  // 最小值
    startSlider->setMaximum(99);  // 最大值
    startSlider->setSingleStep(1);  // 步长

    endSlider->setOrientation(Qt::Horizontal);  // 水平方向
    endSlider->setMinimum(0);  // 最小值
    endSlider->setMaximum(99);  // 最大值
    endSlider->setSingleStep(1);  // 步长

    startSpin->setMinimum(0);  // 最小值
    startSpin->setMaximum(99);  // 最大值
    startSpin->setSingleStep(1);  // 步长

    endSpin->setMinimum(0);  // 最小值
    endSpin->setMaximum(99);  // 最大值
    endSpin->setSingleStep(1);  // 步长

    endSlider->setValue(99);
    endSpin->setValue(99);

    Update->setText("Update");

    connect(startSpin,      SIGNAL(valueChanged(int)), startSlider, SLOT(setValue(int)));
    connect(startSlider,    SIGNAL(valueChanged(int)), startSpin,   SLOT(setValue(int)));
    connect(endSpin,        SIGNAL(valueChanged(int)), endSlider,   SLOT(setValue(int)));
    connect(endSlider,      SIGNAL(valueChanged(int)), endSpin,     SLOT(setValue(int)));
    connect(startSlider,    SIGNAL(valueChanged(int)), this,        SLOT(UpdateStart(int)));
    connect(endSlider,      SIGNAL(valueChanged(int)), this,        SLOT(UpdateEnd(int)));

    connect(Update, SIGNAL(pressed()), this, SLOT(decide()));

    ui->RangeWidget->hide();
}

// start countThread
void MainWindow::initCount()
{
    if(map.isEmpty())
    {
        QMessageBox msgBox;
        msgBox.setText("Please load map!");
        msgBox.exec();
        mapLoad();
    }

    for(int i=0;i<100;++i)
    {
        cnt_total[i] = 0;
        income_total[i] = 0;
    }

    CountThread *countThread = new CountThread;

    Line_count = 0;
    total_Line = 0;

    connect(countThread, SIGNAL(changeNum(int)),              this,           SLOT(Line_Receive(int)));
    connect(countThread, SIGNAL(transTotal(int)),             this,           SLOT(Total_Receive(int)));
    connect(countThread, SIGNAL(changeNum(int)),              ui->Lines,      SLOT(setNum(int)));
    connect(countThread, SIGNAL(transRate(int)),              ui->myProgress, SLOT(setValue(int)));
    connect(countThread, SIGNAL(transCount(QList<int>)),      this,           SLOT(Count_Receive(QList<int>)));
    connect(countThread, SIGNAL(transIncome(QList<double>)),  this,           SLOT(Income_Receive(QList<double>)));
    connect(countThread, SIGNAL(transGrid(int,QList<QList<int>>)),
                                                              this,           SLOT(Grid_Receive(int,QList<QList<int>>)));
    ui->myProgress->setRange(0,100);

    countThread->Start = Start_Date;
    countThread->End   = End_Date;
    countThread->map   = map;
    for(int i=0;i<=100;++i)
    {
        countThread->count.append(0);
        countThread->income.append(0);
    }
    for(int i=1;i<=15;++i)
    {
        countThread->data[i] = data[i];
    }
    countThread->start();
}
