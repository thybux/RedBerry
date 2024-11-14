#include "include/logger.h"
#include <QStandardPaths>
#include <QDir>
#include <QTextStream>
#include <QDateTime>
#include <QRegularExpression>
#include <QDebug>
    
Logger::Logger(QObject *parent) : QObject(parent)
{
    initLogFile();
}

Logger& Logger::getInstance()
{
    static Logger instance;
    return instance;
}

void Logger::initLogFile()
{
    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(appDataPath);
    m_logFilePath = appDataPath + "/redberry_scans.log";
    m_logFile.setFileName(m_logFilePath);
}

Logger::NmapScanResult Logger::parseNmapOutput(const QString& rawOutput)
{
    NmapScanResult result;
    
    // Expressions régulières pour parser la sortie nmap
    QRegularExpression portRe("(\\d+)/(tcp|udp)\\s+(open|filtered|closed)\\s+(\\S+)?\\s*(.*)");
    QRegularExpression osRe("OS details: (.+)");
    QRegularExpression macRe("MAC Address: ([0-9A-F:]{17}) \\((.+)\\)");
    QRegularExpression latencyRe("Latency: (.+)");

    // Analyser ligne par ligne
    QStringList lines = rawOutput.split('\n');
    for (const QString& line : lines) {
        // Analyser les ports
        auto portMatch = portRe.match(line);
        if (portMatch.hasMatch()) {
            int port = portMatch.captured(1).toInt();
            NmapScanResult::PortInfo portInfo;
            portInfo.protocol = portMatch.captured(2);
            portInfo.state = portMatch.captured(3);
            portInfo.service = portMatch.captured(4);
            portInfo.version = portMatch.captured(5);
            result.ports[port] = portInfo;
            continue;
        }

        // Analyser les détails de l'hôte
        auto osMatch = osRe.match(line);
        if (osMatch.hasMatch()) {
            result.hostDetails.append("Système d'exploitation: " + osMatch.captured(1));
            continue;
        }

        auto macMatch = macRe.match(line);
        if (macMatch.hasMatch()) {
            result.hostDetails.append(QString("Adresse MAC: %1 (%2)")
                .arg(macMatch.captured(1))
                .arg(macMatch.captured(2)));
            continue;
        }

        auto latencyMatch = latencyRe.match(line);
        if (latencyMatch.hasMatch()) {
            result.hostDetails.append("Latence: " + latencyMatch.captured(1));
        }
    }

    return result;
}

QString Logger::formatPortInfo(int port, const NmapScanResult::PortInfo& info)
{
    QString status;
    if (info.state == "open") {
        status = "OUVERT";
    } else if (info.state == "filtered") {
        status = "FILTRÉ";
    } else {
        status = "FERMÉ";
    }

    QString description = QString("Port %1/%2 - %3")
        .arg(port)
        .arg(info.protocol)
        .arg(status);

    if (!info.service.isEmpty()) {
        description += QString(" - Service: %1").arg(info.service);
        if (!info.version.isEmpty()) {
            description += QString(" (%1)").arg(info.version);
        }
    }

    return description;
}

QString Logger::formatScanResult(const NmapScanResult& result)
{
    QString formattedOutput;
    QTextStream out(&formattedOutput);

    out << "=== Résumé du Scan ===\n\n";

    // Informations sur l'hôte
    if (!result.hostDetails.isEmpty()) {
        out << "Informations sur l'hôte:\n";
        for (const QString& detail : result.hostDetails) {
            out << "- " << detail << "\n";
        }
        out << "\n";
    }

    // Ports ouverts
    if (!result.ports.isEmpty()) {
        out << "Ports détectés:\n\n";
        
        // Trier les ports par état
        QMap<QString, QStringList> portsByState;
        for (auto it = result.ports.constBegin(); it != result.ports.constEnd(); ++it) {
            portsByState[it.value().state].append(formatPortInfo(it.key(), it.value()));
        }

        // Afficher d'abord les ports ouverts
        if (portsByState.contains("open")) {
            out << "PORTS OUVERTS:\n";
            for (const QString& portInfo : portsByState["open"]) {
                out << "✓ " << portInfo << "\n";
            }
            out << "\n";
        }

        // Puis les ports filtrés
        if (portsByState.contains("filtered")) {
            out << "PORTS FILTRÉS:\n";
            for (const QString& portInfo : portsByState["filtered"]) {
                out << "? " << portInfo << "\n";
            }
            out << "\n";
        }

        // Et enfin les ports fermés
        if (portsByState.contains("closed")) {
            out << "PORTS FERMÉS:\n";
            for (const QString& portInfo : portsByState["closed"]) {
                out << "✗ " << portInfo << "\n";
            }
        }
    } else {
        out << "Aucun port détecté.\n";
    }

    return formattedOutput;
}

void Logger::logNmapScan(const QString& target, 
                        const QString& scanType,
                        const QString& command,
                        const QString& rawOutput,
                        bool success)
{
    NmapScanResult result = parseNmapOutput(rawOutput);
    result.timestamp = QDateTime::currentDateTime().toString(Qt::ISODate);
    result.target = target;
    result.scanType = scanType;
    result.command = command;
    result.success = success;

    QString formattedOutput = formatScanResult(result);

    QString logEntry = QString("=== Nouveau Scan ===\n"
                             "Date: %1\n"
                             "Cible: %2\n"
                             "Type de scan: %3\n"
                             "Statut: %4\n\n"
                             "%5\n"
                             "Commande utilisée: %6\n"
                             "==================\n\n")
                          .arg(result.timestamp)
                          .arg(result.target)
                          .arg(result.scanType)
                          .arg(success ? "Succès" : "Échec")
                          .arg(formattedOutput)
                          .arg(result.command);

    writeToLog(logEntry);
    emit logsUpdated();
}

void Logger::writeToLog(const QString& content)
{
    if (m_logFile.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&m_logFile);
        out << content;
        m_logFile.close();
    } else {
        qDebug() << "Erreur lors de l'écriture dans le fichier de log:" << m_logFile.errorString();
    }
}

// Ajoutons aussi les méthodes readAllLogs et clearLogs qui sont utiles

QString Logger::readAllLogs() const
{
    QString content;
    QFile file(m_logFilePath);
    
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        content = in.readAll();
        file.close();
    } else {
        qDebug() << "Erreur lors de la lecture du fichier de log:" << file.errorString();
    }
    
    return content;
}

void Logger::clearLogs()
{
    if (m_logFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        m_logFile.close();
        emit logsUpdated();
    } else {
        qDebug() << "Erreur lors de la suppression des logs:" << m_logFile.errorString();
    }
}