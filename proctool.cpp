#include "proctool.h"

ProcTool::ProcTool()
{

}

QString ProcTool::getHostName(){
    QFile file("/proc/sys/kernel/hostname");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream in(&file);
    QString name = in.readLine();
    file.close();
    return name;
}

QString ProcTool::getRunTime()
{
    QFile file("/proc/uptime");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream in(&file);
    QString stime = in.readLine();
    file.close();
    int num = stime.section(' ',0,0).section('.',0,0).toInt();
    int days = (num)/(24*3600);
    int hours = (num)%(24*3600)/3600;
    int minutes = (num)%3600/60;
    int second = (num)%60;
    QString stime_r = QString::number(days) + ':' + QString::number(hours) + ':' + QString::number(minutes) + ':' + QString::number(second);
    //qDebug() << stime_r;
    return stime_r;
}

QString ProcTool::getStartTime()
{
    QFile file("/proc/uptime");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream in(&file);
    QString stime = in.readLine();
    file.close();
    int num = stime.section(' ',0,0).section('.',0,0).toInt();
    QDateTime time = QDateTime::currentDateTime();   //获取当前时间
    int timeT = time.toTime_t();   //将当前时间转为时间戳
    QString stime_r = QDateTime::fromTime_t(timeT - num).toString("yyyy.MM.dd hh:mm:ss");
    qDebug() << stime_r;
    return stime_r;
}

QString ProcTool::getSysVersion()
{
    QFile file("/proc/sys/kernel/ostype");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream in(&file);
    QString osType = in.readLine();
    file.close();

    QFile file2("/proc/sys/kernel/osrelease");
    file2.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream in2(&file2);
    QString osRelease = in2.readLine();
    file2.close();
    qDebug() << osType + "-" + osRelease;
    return osType + "-" + osRelease;
}

QString ProcTool::getCpuInfo()
{
    QFile file("/proc/cpuinfo");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream in(&file);
    for(int i = 0; i < 4; ++i)in.readLine();
    QString info = in.readLine();
    file.close();
    qDebug() << info.section(':',1,1).mid(1);
    return info.section(':',1,1).mid(1);
}

QStringList ProcTool::getProcInfo(int pid)
{
    QStringList InfoList;
    QFile file("/proc/" + QString::number(pid) + "/stat");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))return InfoList;
    QTextStream in(&file);
    QString info = in.readLine();
    file.close();
    InfoList.append(QString::number(pid));
    InfoList.append(info.section(' ',1,1).mid(1,info.section(' ',1,1).length()-2));     //process name
    //qDebug() << info.section(' ',1,1).mid(1,info.section(' ',1,1).length()-2);
    InfoList.append(info.section(' ',3,3));                                           //ppid
    //qDebug() << info.section(' ',3,3);
    InfoList.append(info.section(' ',4,4));                                           //pgid
    //qDebug() << info.section(' ',4,4);

    QFile file2("/proc/" + QString::number(pid) + "/status");
    file2.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream in2(&file2);
    QString info2;
    for(int i = 0; i < 23; ++i)info2 = in2.readLine();
    file2.close();
    if(info2.section(':',0,0) != QString("RssAnon"))InfoList.append(QString("0"));       //memory
    else InfoList.append(info2.section(' ',-2,-2));
    //qDebug() << info2.section(' ',-2,-2);

    InfoList.append(info.section(' ',17,17));                                           //priority
    //qDebug() << info.section(' ',17,17);
    return InfoList;
}

void ProcTool::getCpuTime(int num, int &total, int &idle)
{
    QFile file("/proc/stat");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream in(&file);
    QString info;
    for(int i = 0; i < num + 1; ++i){
        info = in.readLine();
    }
    file.close();
    total = 0;
    for(int i = 1; i < 10; ++i){
        total += info.section(' ',i,i).toInt();
    }
    idle = info.section(' ',-7,-7).toInt();
    //qDebug() << "total:" << total << "| idle:" << idle;
}

QString ProcTool::getMemSize()
{
    QFile file("/proc/meminfo");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream in(&file);
    QString info = in.readLine();
    file.close();
    QString size = QString::number(info.section(' ',-2,-2).toDouble()/1024/1024);
    return size + " GB";
}

double ProcTool::getMemUse()
{
    QFile file("/proc/meminfo");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream in(&file);
    QString info = in.readLine();
    int totalsize = info.section(' ',-2,-2).toInt();
    info = in.readLine();
    info = in.readLine();
    info = in.readLine();
    int buffsize = info.section(' ',-2,-2).toInt();
    info = in.readLine();
    int cachesize = info.section(' ',-2,-2).toInt();
    file.close();

    return 100*((double)(buffsize + cachesize))/totalsize;
}

QVector<int> ProcTool::getAllPID()
{
    QVector<int> PIDs;
    QDir dir("/proc/");
    QFileInfoList list = dir.entryInfoList();
    if(list.length()!=0){
        for (int i = 0; i < list.size(); ++i)
        {
            if(list.at(i).fileName().toInt()){
                PIDs.append(list.at(i).fileName().toInt());
            }
        }
    }
    return PIDs;
}

QString ProcTool::getDetail(int pid)
{
    QString info = "";
    QFile file("/proc/" + QString::number(pid) + "/status");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))return info;
    QTextStream in(&file);
    info = in.readAll();
    file.close();
    return info;
}

double ProcTool::getSwapUse()
{
    QFile file("/proc/meminfo");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream in(&file);
    QString info;
    for(int i = 0; i < 15; ++i)info = in.readLine();
    int totalsize = info.section(' ',-2,-2).toInt();
    info = in.readLine();
    int freesize = info.section(' ',-2,-2).toInt();
    file.close();

    return ((double)(totalsize - freesize))/totalsize;
}
