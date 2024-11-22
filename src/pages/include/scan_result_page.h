#pragma once

#include <QWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QListWidget>

class ScanResultPage : public QWidget {
    Q_OBJECT
public:
    explicit ScanResultPage(QWidget *parent = nullptr);
    void setResults(const QString& target, const QString& output, bool success);
    void runSearchsploit(const QString& serviceName);
    void runScanComplete(const QStringList& serviceList);
    bool eventFilter(QObject* obj, QEvent* event) override;
signals:
    void requestNavigateToPage(const QString& pageName);

private:
    QTextEdit *m_resultDisplay;
    QPushButton *m_backButton;
    QListWidget *m_serviceList;
    QPushButton *m_scanCompletButton;
};