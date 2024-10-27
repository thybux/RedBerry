#ifndef SCRIPT_H
#define SCRIPT_H

#include <QWidget>

class Script : public QWidget
{
    Q_OBJECT
public:
    explicit Script(QWidget *parent = nullptr);

signals:
    void requestNavigateToPage(const QString &pageName);
};

#endif // SCRIPT_H
