#ifndef HISTORIQUE_H
#define HISTORIQUE_H

#include <QWidget>

class Historique : public QWidget
{
    Q_OBJECT
public:
    explicit Historique(QWidget *parent = nullptr);

signals:
    void requestNavigateToPage(const QString &pageName);
};

#endif // HISTORIQUE_H
