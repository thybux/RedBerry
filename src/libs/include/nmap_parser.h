#pragma once

#include <QString>
#include <QList>
#include <QPair>

class NmapParser {
public:
    static QList<QPair<QString, QString>> parseNmapOutput(const QString& output);
};