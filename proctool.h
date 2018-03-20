#ifndef PROCTOOL_H
#define PROCTOOL_H
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QDateTime>
#include <QTimer>
#include <QVector>
#include <QDir>
#include <QMenu>
#include <QAction>
#include <unistd.h>
#include <stdio.h>

class ProcTool
{
public:
    ProcTool();
    static QString getHostName();
    static QString getRunTime();
    static QString getStartTime();
    static QString getSysVersion();
    static QString getCpuInfo();
    static QStringList getProcInfo(int pid);
    static void getCpuTime(int num, int& total, int& idle);
    static QString getMemSize();
    static double getMemUse();
    static QVector<int> getAllPID();
    static QString getDetail(int pid);
    static double getSwapUse();
};

#endif // PROCTOOL_H
