#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>

class QPushButton;
class QStackedWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

private:
    QPushButton* createMenuButton(const QString &text, const QString &pageName);
    void createHomePage();
    void createPages();
    void navigateToPage(const QString &pageName);

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void onMenuButtonClicked();

private:
    QStackedWidget *m_stackedWidget;
    QMap<QString, QWidget*> m_pages;
    QPixmap m_background;
};

#endif // MAINWINDOW_H