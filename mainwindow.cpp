#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    QString title = ProcTool::getHostName();
    ui->setupUi(this);
    setWindowTitle("Linux Task Manager  - " + title);
    setWindowFlags(Qt::WindowCloseButtonHint|Qt::WindowMinimizeButtonHint);
    setFixedSize(534,549);
    ui->lhn->setText(ProcTool::getHostName());
    ui->lbt->setText(ProcTool::getStartTime());
    ui->lpi->setText(ProcTool::getCpuInfo());
    ui->lsv->setText(ProcTool::getSysVersion());
    ui->lrt->setText(ProcTool::getRunTime());
    ui->lm->setText(ProcTool::getMemSize());

    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->horizontalHeader()->setVisible(true);
    ui->tableWidget->verticalHeader()->setVisible(false);
    ui->tableWidget->setShowGrid(false);
    ui->tableWidget->verticalHeader()->setDefaultSectionSize(18);
    ui->tableWidget->setStyleSheet("QHeaderView::section {background-color:white;color: black;padding-left: 4px;border: 1px solid #FFFAF0;}");
    ui->tableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableWidget,SIGNAL(customContextMenuRequested(QPoint)),SLOT(customMenuRequested(QPoint)));

    QStringList HStrList;
    HStrList.push_back(QString("PID"));
    HStrList.push_back(QString("Name"));
    HStrList.push_back(QString("PPID"));
    HStrList.push_back(QString("PGID"));
    HStrList.push_back(QString("Mem(KiB)"));
    HStrList.push_back(QString("Priority"));
    ui->tableWidget->setColumnCount(6);
    ui->tableWidget->setHorizontalHeaderLabels(HStrList);
    ui->tableWidget->setColumnWidth(0,ui->tableWidget->width()/9);
    ui->tableWidget->setColumnWidth(1,ui->tableWidget->width()/4);
    ui->tableWidget->setColumnWidth(2,ui->tableWidget->width()/7);
    ui->tableWidget->setColumnWidth(3,ui->tableWidget->width()/7);
    ui->tableWidget->setColumnWidth(4,ui->tableWidget->width()/7);
    ui->tableWidget->horizontalHeader()->setSortIndicatorShown(false);
    ui->tableWidget->horizontalHeader()->setHighlightSections(false);
    ui->tableWidget->setSortingEnabled(false);
    //connect(ui->tableWidget->horizontalHeader(), SIGNAL(sectionClicked(int)), ui->tableWidget, SLOT(sortByColumn(int)));
    lstatu = new QLabel;
    bstatu = new QPushButton("More Menu");

    QMenu* menu = new QMenu(this);
    QAction* ac = new QAction(tr("Run"),this);
    QAction* ac2 = new QAction(tr("Kill - Search"),this);
    QAction* ac3 = new QAction(tr("Detail - Search"),this);
    QList<QAction*> aclist;
    aclist << ac << ac2 << ac3;
    menu->addActions(aclist);
    connect(ac,SIGNAL(triggered(bool)),this,SLOT(mRun()));
    connect(ac2,SIGNAL(triggered(bool)),this,SLOT(mKill()));
    connect(ac3,SIGNAL(triggered(bool)),this,SLOT(mDetail()));
    bstatu->setMenu(menu);

    ui->statusBar->addWidget(lstatu);
    ui->statusBar->addWidget(bstatu);
    //connect(ui->statusBar,SIGNAL(C))

    QHBoxLayout tab2;
    tab2.setMargin(0);
    chart = new QChart();
    chart->setTitle(tr("CPU"));
    chart2 = new QChart();
    chart2->setTitle(tr("MEM"));
    series = new QSplineSeries(chart);
    series2 = new QSplineSeries(chart2);
    series3 = new QSplineSeries(chart2);

    QChartView* chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setLineWidth(1);
    chartView->setGeometry(0,0,ui->tabWidget->width(),250);
    chartView->setContentsMargins(0,0,0,0);
    QChartView* chartView2 = new QChartView(chart2);
    chartView2->setRenderHint(QPainter::Antialiasing);
    chartView2->setLineWidth(1);
    chartView2->setGeometry(0,250,ui->tabWidget->width(),200);
    chartView2->setContentsMargins(0,0,0,0);

    chart->addSeries(series);
    chart->createDefaultAxes();
    chart->axisX()->setRange(0, max_Y);
    chart->axisY()->setRange(0, 100);
    chart->axisX()->setLabelsVisible(false);
    chart->axisY()->setGridLineVisible(false);
    chart->legend()->hide();
    QMargins margin(0,0,0,0);
    chart->setMargins(margin);
    QFont font;
    font.setPixelSize(11);
    chart->axisY()->setTitleFont(font);
    chart->setTitleFont(font);

    series2->setName("Memory");
    series3->setName("Swap");
    chart2->addSeries(series2);
    chart2->addSeries(series3);
    chart2->createDefaultAxes();
    chart2->axisX()->setRange(0, max_Y);
    chart2->axisY()->setRange(0, 1);
    chart2->axisX()->setLabelsVisible(false);
    chart2->axisY()->setGridLineVisible(false);
    chart2->setMargins(margin);
    chart2->axisY()->setTitleFont(font);
    chart2->setTitleFont(font);

    tab2.addWidget(chartView);
    tab2.addWidget(chartView2);
    ui->tab_2->setLayout(&tab2);

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(1000);

    QTimer *timer2 = new QTimer(this);
    connect(timer2, SIGNAL(timeout()), this, SLOT(chartDraw()));
    timer2->start(600);
}

MainWindow::~MainWindow()
{
    delete ui;
}

double MainWindow::calculate(int num)
{
    static int totalp[8] = {0};
    static int ildep[8] = {0};
    int total,ilde;
    ProcTool::getCpuTime(num,total,ilde);
    if(total == totalp[num])return 0;
    double r = 1 - ((double)(ilde - ildep[num]))/(total - totalp[num]);
    totalp[num] = total;
    ildep[num] = ilde;
    return r*100;
}

void MainWindow::update()
{
    ui->lrt->setText(ProcTool::getRunTime());
    QVector<int> PID = ProcTool::getAllPID();
    ui->tableWidget->clearContents();
//    for(int k = 0; k < ui->tableWidget->rowCount(); ++k){
//        QString process = "";
//        if(process.size()==0)continue;
//        for(int i = 0; i < 6; ++i){
//            ui->tableWidget->setItem(k, i, new QTableWidgetItem(process));
//        }
//    }
    ui->tableWidget->setRowCount(PID.size());
    for(int k = 0; k < PID.size(); ++k){
        QStringList process = ProcTool::getProcInfo(PID.at(k));
        if(process.size()==0)continue;
        for(int i = 0; i < 6; ++i){
            if(i == 0){
                QTableWidgetItem* item = new QTableWidgetItem();
                item->setData(2,QString(process[i]).toInt());
                ui->tableWidget->setItem(k, i,item);
                ui->tableWidget->item(k,i)->setTextAlignment(Qt::AlignCenter);
            }
            else ui->tableWidget->setItem(k, i, new QTableWidgetItem(process[i]));
            ui->tableWidget->item(k,i)->setTextAlignment(Qt::AlignCenter);
        }
    }
    ui->tableWidget->sortByColumn(0,Qt::AscendingOrder);
    lstatu->setText("Local Time: " + QTime::currentTime().toString() + "  CPU: " + QString::number(calculate(0),'f',2) + "%" + "   Mem: " + QString::number(ProcTool::getMemUse(),'f',2) + "%" + "                          ");
}

void MainWindow::customMenuRequested(QPoint pos)
{
    QMenu* menu = new QMenu(this);
    QAction* ac = new QAction(tr("Kill"),this);
    QAction* ac2 = new QAction(tr("Detail"),this);
    QList<QAction*> aclist;
    aclist << ac << ac2;
    ac->setData(ui->tableWidget->selectedItems().at(0)->text().toInt());
    ac2->setData(ui->tableWidget->selectedItems().at(0)->text().toInt());
    menu->addActions(aclist);
    menu->popup(ui->tableWidget->viewport()->mapToGlobal(pos));
    connect(ac,SIGNAL(triggered(bool)),this,SLOT(killpid()));
    connect(ac2,SIGNAL(triggered(bool)),this,SLOT(showDetail()));
}


void MainWindow::killpid()
{
    QAction* action = qobject_cast<QAction*> (sender());
    if (action == 0) return;
    QVariant spid = action->data();
    int pid = spid.toInt();
    kill(pid,SIGKILL);
}

void MainWindow::showDetail()
{
    QAction* action = qobject_cast<QAction*> (sender());
    if (action == 0) return;
    QVariant spid = action->data();
    QString info = ProcTool::getDetail(spid.toInt());
    QMessageBox detailDialog(QMessageBox::NoIcon,"PID: " + spid.toString(),info);
    detailDialog.exec();
}

void MainWindow::mRun()
{
    //QFileDialog::getOpenFileNames(this,tr("Run"),tr("Input:"),QLineEdit::Normal,tr(""),&ok);
    QStringList cmd = QFileDialog::getOpenFileNames(this,tr("Run"));
    if(cmd.size() == 1){
        if(!fork()){
            execv(QString(cmd[0]).toStdString().c_str(),nullptr);
        }
    }
}

void MainWindow::mDetail()
{
    bool ok;
    QString info = QInputDialog::getText(this,tr("Detail"),tr("Input PID or Name:"),QLineEdit::Normal,tr(""),&ok);
    if(ok){
        if(info.toInt()){//PID
            QString text = ProcTool::getDetail(info.toInt());
            if(text.length())QMessageBox::information(this,"PID: " + info,text);
            else QMessageBox::critical(this,"Error","Cannot Find The Process!");
        }
        else {//Name
            int pid = -1;
            for(int i = 0; i < ui->tableWidget->rowCount(); ++i){
                if(ui->tableWidget->item(i,1)->text() == info){
                    pid = ui->tableWidget->item(i,0)->text().toInt();
                    break;
                }
            }
            if(pid == -1)QMessageBox::critical(this,"Error","Cannot Find The Process!");
            else {
                QString text = ProcTool::getDetail(pid);
                if(text.length())QMessageBox::information(this,"PID: " + QString::number(pid),text);
                else QMessageBox::critical(this,"Error","Cannot Find The Process!");
            }
        }
    }
}

void MainWindow::chartDraw()
{
    double r = calculate(0);
    //qDebug() << r;
    QVector<QPointF> OldPoints = series->pointsVector();
    QVector<QPointF> Points;
    if(OldPoints.size() > max_Y)OldPoints.pop_front();
    for (int i = 0; i < OldPoints.size(); ++i) {
        Points.append(QPointF(OldPoints.at(i).x() - 10,OldPoints.at(i).y()));
    }
    Points.append(QPointF(max_Y,r));
    series->replace(Points);

    QVector<QPointF> mOldPoints = series2->pointsVector();
    QVector<QPointF> mPoints;
    if(mOldPoints.size() > max_Y)mOldPoints.pop_front();
    for (int i = 0; i < mOldPoints.size(); ++i) {
        mPoints.append(QPointF(mOldPoints.at(i).x() - 10,mOldPoints.at(i).y()));
    }
    mPoints.append(QPointF(max_Y,ProcTool::getMemUse()/100));
    series2->replace(mPoints);

    QVector<QPointF> sOldPoints = series3->pointsVector();
    QVector<QPointF> sPoints;
    if(sOldPoints.size() > max_Y)sOldPoints.pop_front();
    for (int i = 0; i < sOldPoints.size(); ++i) {
        sPoints.append(QPointF(sOldPoints.at(i).x() - 10,sOldPoints.at(i).y()));
    }
    sPoints.append(QPointF(max_Y,ProcTool::getSwapUse()));
    series3->replace(sPoints);
}

void MainWindow::mKill()
{
    bool ok;
    QString info = QInputDialog::getText(this,tr("Kill"),tr("Input PID or Name:"),QLineEdit::Normal,tr(""),&ok);
    if(ok){
        if(info.toInt()){//PID
            if(kill(info.toInt(),SIGKILL))QMessageBox::critical(this,"Error","Cannot Kill The Process!");
            else QMessageBox::information(this,"Success","Process has been killed!");
        }
        else {//Name
            int pid = -1;
            for(int i = 0; i < ui->tableWidget->rowCount(); ++i){
                if(ui->tableWidget->item(i,1)->text() == info){
                    pid = ui->tableWidget->item(i,0)->text().toInt();
                    break;
                }
            }
            if(pid == -1)QMessageBox::critical(this,"Error","Cannot Find The Process!");
            else {
                if(kill(pid,SIGKILL))QMessageBox::critical(this,"Error","Cannot Kill The Process!");
                else QMessageBox::information(this,"Success","Process has been killed!");
            }
        }
    }
}
