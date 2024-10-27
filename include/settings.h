#ifndef SETTINGS_H
#define SETTINGS_H

#include <QWidget>

class Settings : public QWidget
{
    Q_OBJECT
public:
    explicit Settings(QWidget *parent = nullptr);

signals:
    void requestNavigateToPage(const QString &pageName);
};

#endif // SETTINGS_H
