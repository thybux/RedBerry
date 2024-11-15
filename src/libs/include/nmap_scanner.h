#ifndef NMAP_SCANNER_H
#define NMAP_SCANNER_H

#include <QWidget>
#include <QLineEdit>
#include <QTextEdit>
#include <QCheckBox>
#include <QProcess>
#include <QComboBox>
#include <QPushButton>
#include <QVBoxLayout>

class NmapScanner : public QWidget
{
    Q_OBJECT

public:
    explicit NmapScanner(QWidget *parent = nullptr);

private slots:
    void runScan();
    void handleScanOutput();
    void handleScanError();
    void scanFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void setupOptionsGroup(QVBoxLayout* mainLayout);
    void setupOutputDisplay(QVBoxLayout* mainLayout);
    void setupConfigGroup(QVBoxLayout* mainLayout);

signals:
    void scanCompleted(const QString& target, const QString& output, bool success);

private:
    // Interface elements
    QPushButton* scanButton;
    QLineEdit *m_targetInput;
    QComboBox *m_scanTypeCombo;
    QCheckBox *m_portScanCheck;
    QLineEdit *m_portRangeInput;
    QCheckBox *m_serviceDetectionCheck;
    QCheckBox *m_osDetectionCheck;
    QCheckBox *m_scriptScanCheck;
    QTextEdit *m_outputDisplay;
    QCheckBox *m_servicePortCheck;
    
    // Process handling
    QProcess *m_nmapProcess;
    QString buildNmapCommand();
};

#endif // NMAP_SCANNER_H