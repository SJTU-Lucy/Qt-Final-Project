#include "worker.h"
#include "ui_mainwindow.h"
#include "mainwindow.h"
#include <QFileDialog>
#include <QMenuBar>
#include <QApplication>
#include <QFileDialog>
#include <QDir>
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
#include <QtCharts/QAreaSeries>

Worker::Worker()
{
    m_thread.start();
    this->moveToThread(&m_thread);
}

void Worker::sieve()
{
}
