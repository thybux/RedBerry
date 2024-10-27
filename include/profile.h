#ifndef PROFILE_H
#define PROFILE_H

#include <QWidget>

class Profile : public QWidget
{
    Q_OBJECT
public:
    explicit Profile(QWidget *parent = nullptr);

signals:
    void requestNavigateToPage(const QString &pageName);
};

#endif // PROFILE_H
