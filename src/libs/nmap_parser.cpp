#include "./include/nmap_parser.h"
#include <QRegularExpression>

QList<QPair<QString, QString>> NmapParser::parseNmapOutput(const QString& output) {
    QList<QPair<QString, QString>> results;

    QRegularExpression re(R"((\d+)/tcp\s+open\s+(\S+))");
    QRegularExpressionMatchIterator i = re.globalMatch(output);

    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        QString port = match.captured(1);
        QString service = match.captured(2);
        results.append(qMakePair(port, service));
    }

    return results;
}