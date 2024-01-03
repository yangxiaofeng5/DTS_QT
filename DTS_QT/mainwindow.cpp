#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QDebug>

bool autoScroll = false;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle(tr("COFS-DTS"));

    serialport = new QSerialPort;

    RealtimeDataShow(ui->customPlot);

    ui->customPlot->graph(0)->rescaleValueAxis();
    ui->customPlot->graph(1)->rescaleValueAxis(true);
}


MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::RealtimeDataShow(QCustomPlot *customPlot)
{
    QButtonGroup* pButtonGroup = new QButtonGroup(this);
    pButtonGroup->addButton(ui->checkBox_X,1);
    pButtonGroup->addButton(ui->checkBox_Y,2);
    pButtonGroup->addButton(ui->checkBox_XY,3);

    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%h:%m:%s");
    customPlot->xAxis->setTicker(timeTicker);

    // 实时更新槽函数
    customPlot->addGraph(); // blue line
    customPlot->graph(0)->setPen(QPen(QColor(40, 110, 255)));

    customPlot->addGraph(); // red line
    customPlot->graph(1)->setPen(QPen(QColor(255, 110, 40)));

    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);    //允许可缩放，移动
    QList < QCPAxis *>  xis;
    xis.append(customPlot->xAxis);
    customPlot->axisRect()->setRangeZoomAxes(xis);

    // 轴随动
    connect(customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->xAxis2, SLOT(setRange(QCPRange)));
    connect(customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->yAxis2, SLOT(setRange(QCPRange)));

    connect(&dataTimer, SIGNAL(timeout()), this, SLOT(realtimeDataSlot()));

    //轴缩放槽函数
    connect(ui->checkBox_X, SIGNAL(stateChanged(int)), this, SLOT(checkbox_change()));
    connect(ui->checkBox_Y, SIGNAL(stateChanged(int)), this, SLOT(checkbox_change()));
    connect(ui->checkBox_XY, SIGNAL(stateChanged(int)), this, SLOT(checkbox_change()));

    //打开框选设置
   // ui->customPlot->selectionRect()->setPen(QPen(Qt::black,1,Qt::DashLine));//设置选框的样式：虚线
    //ui->customPlot->selectionRect()->setBrush(QBrush(QColor(0,0,100,50)));//设置选框的样式：半透明浅蓝
   // ui->customPlot->setSelectionRectMode(QCP::SelectionRectMode::srmZoom);
}


void MainWindow::realtimeDataSlot()
{
    static QTime timeStart = QTime::currentTime();
    //计算新的数据点
    double key = (double)(QDateTime::currentMSecsSinceEpoch()) / 1000.0; // time elapsed since start of demo, in seconds
    static double lastPointKey = 0;
    if (key-lastPointKey > 0.002) // at most add point every 2 ms
    {
        // add data to lines:
        ui->customPlot->graph(0)->addData(key, qSin(key)+std::rand()/(double)RAND_MAX*100*qSin(key/0.3843));
        ui->customPlot->graph(1)->addData(key, qCos(key)+std::rand()/(double)RAND_MAX*400*qSin(key/0.4364));
        // rescale value (vertical) axis to fit the current data:
        //ui->customPlot->graph(0)->rescaleValueAxis();
        //ui->customPlot->graph(1)->rescaleValueAxis(true);
        lastPointKey = key;
    }


    //设置x轴范围
    if(autoScroll)
    {
        ui->customPlot->xAxis->setRange(key - ui->customPlot->xAxis->range().size(), key);
    }

    //画图
    ui->customPlot->replot();

    // calculate frames per second:
    static double lastFpsKey;
    static int frameCount;
    ++frameCount;
    if (key-lastFpsKey > 2) // average fps over 2 seconds
    {
        ui->statusbar->showMessage(
            QString("%1 FPS, Total Data points: %2")
                .arg(frameCount/(key-lastFpsKey), 0, 'f', 0)
                .arg(ui->customPlot->graph(0)->data()->size()+ui->customPlot->graph(1)->data()->size())
            , 0);
        lastFpsKey = key;
        frameCount = 0;
    }
}

void MainWindow::checkbox_change()
{
    // 缩放轴
    QList < QCPAxis *>  axis_x,axis_y,axis_xy;
    axis_x.append(ui->customPlot->xAxis);
    axis_y.append(ui->customPlot->yAxis);
    axis_xy.append(ui->customPlot->xAxis);
    axis_xy.append(ui->customPlot->yAxis);
    if(ui->checkBox_X->checkState())
    {
        ui->customPlot->axisRect()->setRangeZoomAxes(axis_x);
    }
    if(ui->checkBox_Y->checkState())
    {
        ui->customPlot->axisRect()->setRangeZoomAxes(axis_y);
    }
    if(ui->checkBox_XY->checkState())
    {
        ui->customPlot->axisRect()->setRangeZoomAxes(axis_xy);
    }

}

void MainWindow::onMouseMove(QMouseEvent *event)
{

    QCustomPlot* customPlot = qobject_cast<QCustomPlot*>(sender());
    double x = customPlot->xAxis->pixelToCoord(event->pos().x());
    double y = customPlot->yAxis->pixelToCoord(event->pos().y());
    textItem->setText(QString("(%1, %2)").arg(x).arg(y));
    textItem->position->setCoords(QPointF(x, y));
    textItem->setFont(QFont(font().family(), 10));

      qDebug("aaaaa");

}


//按键功能设置
void MainWindow::on_startButton_clicked()
{
    qDebug("1");
    dataTimer.start(0); //1s更新一次
    ui->stopButton->setEnabled(true);
    ui->startButton->setEnabled(false);
}


void MainWindow::on_stopButton_clicked()
{
    qDebug("2");
    dataTimer.stop(); //1s更新一次
    ui->stopButton->setEnabled(false);
    ui->startButton->setEnabled(true);
}


void MainWindow::on_actionHelp_triggered()
{
    qDebug("no more help!");
}


void MainWindow::on_onlineButton_clicked()
{
    qDebug("3");
    ui->onlineButton->setEnabled(false);
    ui->offlineButton->setEnabled(true);
}


void MainWindow::on_offlineButton_clicked()
{
    qDebug("4");
    ui->onlineButton->setEnabled(true);
    ui->offlineButton->setEnabled(false);
}

void MainWindow::on_saveButton_clicked()
{

}

void MainWindow::on_checkBox_full_stateChanged(int arg1)
{
    if(2 == arg1)
    {
        ui->customPlot->xAxis->rescale(true);
        ui->customPlot->yAxis->rescale(true);
    }
}


void MainWindow::on_checkBox_axis_stateChanged(int arg1)
{
    // 显示鼠标坐标
    textItem = new QCPItemText(ui->customPlot);
    if(2 == arg1)
    {
        connect(ui->customPlot, &QCustomPlot::mouseMove, this, &MainWindow::onMouseMove);
    }
    else
    {
        disconnect(ui->customPlot, &QCustomPlot::mouseMove, this, &MainWindow::onMouseMove);
    }
    ui->customPlot->replot();
}


void MainWindow::on_checkBox_scroll_stateChanged(int arg1)
{
    if(2 == arg1)
    {
        autoScroll = true;
    }
    else
    {
        autoScroll = false;
    }
}

