#ifndef SETTINGS_H
#define SETTINGS_H

#include <QWidget>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QGroupBox>

class Settings : public QWidget
{
    Q_OBJECT

public:
    explicit Settings(QWidget *parent = nullptr);

signals:
    void requestNavigateToPage(const QString &pageName);

private:
    QGroupBox* createNetworkSection();
    QGroupBox* createProxySection();
    QGroupBox* createScanningSection();
    QGroupBox* createPayloadSection();
    QGroupBox* createLoggingSection();

private:
    // Network Settings
    QLineEdit *m_interfaceEdit;
    QSpinBox *m_timeoutSpinBox;
    QComboBox *m_protocolCombo;
    
    // Proxy Settings
    QLineEdit *m_proxyHostEdit;
    QSpinBox *m_proxyPortSpinBox;
    QLineEdit *m_proxyUserEdit;
    QLineEdit *m_proxyPassEdit;
    
    // Scanning Settings
    QSpinBox *m_threadCountSpinBox;
    QSpinBox *m_portRangeStartSpinBox;
    QSpinBox *m_portRangeEndSpinBox;
    QComboBox *m_scanTypeCombo;
    
    // Payload Settings
    QLineEdit *m_payloadPathEdit;
    QComboBox *m_encodingCombo;
    QSpinBox *m_payloadSizeSpinBox;
    
    // Logging Settings
    QLineEdit *m_logPathEdit;
    QComboBox *m_logLevelCombo;
};

#endif // SETTINGS_H