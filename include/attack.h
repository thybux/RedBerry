#ifndef ATTACK_H
#define ATTACK_H

#include <QWidget>

class Attack : public QWidget
{
    Q_OBJECT
public:
    explicit Attack(QWidget *parent = nullptr);

signals:
    void requestNavigateToPage(const QString &pageName);
};

#endif // ATTACK_H
