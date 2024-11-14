#ifndef LOGGER_H
#define LOGGER_H

#include <QString>
#include <QObject>
#include <QFile>
#include <QDateTime>
#include <QMap>

class Logger : public QObject
{
    Q_OBJECT

public:
    struct NmapScanResult {
        QString timestamp;
        QString target;
        QString scanType;
        QString command;
        struct PortInfo {
            QString protocol;
            QString state;
            QString service;
            QString version;
        };
        QMap<int, PortInfo> ports;
        QStringList hostDetails;  // OS, MAC address, etc.
        bool success;
    };

    static Logger& getInstance();

    void logNmapScan(const QString& target, 
                     const QString& scanType,
                     const QString& command,
                     const QString& rawOutput,
                     bool success);

    QString getLogFilePath() const { return m_logFilePath; }
    QString readAllLogs() const;
    void clearLogs();

signals:
    void logsUpdated();

private:
    explicit Logger(QObject *parent = nullptr);
    ~Logger() = default;
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    void initLogFile();
    void writeToLog(const QString& content);
    NmapScanResult parseNmapOutput(const QString& rawOutput);
    QString formatScanResult(const NmapScanResult& result);
    QString formatPortInfo(int port, const NmapScanResult::PortInfo& info);

    QString m_logFilePath;
    QFile m_logFile;
};

#endif // LOGGER_H