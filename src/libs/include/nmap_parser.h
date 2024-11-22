#pragma once

#include <QString>
#include <QList>
#include <QPair>
#include <QMap>

struct NmapScanInfo {
    QString scanType;
    QString target;
    QString startTime;
    QString endTime;
    int totalPorts = 0;
    int openPorts = 0;
    QList<QPair<QString, QString>> ports;
    QList<QString> warnings;
    QList<QString> errors;
    QString osInfo;
};

class NmapParser {
public:
    static NmapScanInfo parseNmapOutput(const QString& output);

private:
    static void parsePortInfo(const QString& output, NmapScanInfo& info);
    static void parseOsInfo(const QString& output, NmapScanInfo& info);
    static void parseErrors(const QString& output, NmapScanInfo& info);
    static void parseScanInfo(const QString& output, NmapScanInfo& info);
};