#ifndef SETTINGS_H
#define SETTINGS_H

#include <QWidget>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QNetworkAccessManager>

class Settings : public QWidget
{
    Q_OBJECT

public:
    explicit Settings(QWidget *parent = nullptr);

signals:
    void requestNavigateToPage(const QString &pageName);

private slots:
    void checkInternetConnection();
    void testWifiConnection();
    void updateInternetStatus(bool isConnected);

private:
    QComboBox *m_languageSelect;
    QLabel *m_accountLabel;
    QLabel *m_internetStatusLabel;
    
    // WiFi settings
    QLineEdit *m_ssidInput;
    QLineEdit *m_pskInput;
    QComboBox *m_keyMgmtSelect;
    QLineEdit *m_scanSsidInput;
    
    QNetworkAccessManager *m_networkManager;
    void scanAvailableNetworks();
    void handleScanResults(const QString &output);
    QComboBox *m_networkSelect;
};

#endif // SETTINGS_H