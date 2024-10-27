#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QStackedWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    virtual ~MainWindow();

private slots:
    void onNavigateButtonClicked();
    // void onButtonClicked();

private:
    QStackedWidget *m_stackedWidget;
    QLabel *m_label;
    QPushButton *m_button;
    QPushButton *m_navigateButton;
};
#endif // MAINWINDOW_H
