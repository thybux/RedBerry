#include "./include/nmap_parser.h"
#include <QRegularExpression>
#include <QDebug>

NmapScanInfo NmapParser::parseNmapOutput(const QString& output) {
    NmapScanInfo info;
    
    parseScanInfo(output, info);
    parsePortInfo(output, info);
    parseOsInfo(output, info);
    parseErrors(output, info);
    
    return info;
}

void NmapParser::parseScanInfo(const QString& output, NmapScanInfo& info) {
    // Capture le type de scan et la cible
    QRegularExpression scanStartRe(R"(Starting Nmap (.+) \( http://nmap\.org \))");
    QRegularExpression targetRe(R"(Nmap scan report for (.+))");
    
    auto scanMatch = scanStartRe.match(output);
    if (scanMatch.hasMatch()) {
        info.scanType = scanMatch.captured(1);
    }
    
    auto targetMatch = targetRe.match(output);
    if (targetMatch.hasMatch()) {
        info.target = targetMatch.captured(1);
    }
    
    // Capture les temps de début et fin
    QRegularExpression timeRe(R"(Nmap done at (.+))");
    auto timeMatch = timeRe.match(output);
    if (timeMatch.hasMatch()) {
        info.endTime = timeMatch.captured(1);
    }
}

void NmapParser::parsePortInfo(const QString& output, NmapScanInfo& info) {
    // Parser les ports et services
    QRegularExpression portRe(R"((\d+)/tcp\s+open\s+(\S+)(?:\s+(.+))?)");
    auto matches = portRe.globalMatch(output);
    
    while (matches.hasNext()) {
        
        auto match = matches.next();
        QString port = match.captured(1);
        QString service = match.captured(2);
        QString version = match.captured(3).trimmed();
        QString serviceInfo = service;

        if (!version.isEmpty()) {
            serviceInfo += " (" + version + ")";
        }
        
        info.ports.append(qMakePair(port, serviceInfo));
        info.openPorts++;
    }
}

void NmapParser::parseOsInfo(const QString& output, NmapScanInfo& info) {
    // Parser les informations sur l'OS
    QRegularExpression osRe(R"(OS details: (.+))");
    auto osMatch = osRe.match(output);
    if (osMatch.hasMatch()) {
        info.osInfo = osMatch.captured(1);
    }
}

void NmapParser::parseErrors(const QString& output, NmapScanInfo& info) {
    // Parser les warnings
    QRegularExpression warningRe(R"(Warning: (.+))");
    auto warningMatches = warningRe.globalMatch(output);
    while (warningMatches.hasNext()) {
        auto match = warningMatches.next();
        info.warnings.append(match.captured(1));
    }
    
    // Parser les erreurs
    QRegularExpression errorRe(R"(ERROR: (.+))");
    auto errorMatches = errorRe.globalMatch(output);
    while (errorMatches.hasNext()) {
        auto match = errorMatches.next();
        info.errors.append(match.captured(1));
    }
}