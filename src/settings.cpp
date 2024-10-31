#include "settings.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QNetworkReply>
#include <QTimer>
#include <QComboBox>
#include <QLineEdit>
#include <QMessageBox>
#include <QLabel>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QFormLayout>


Settings::Settings(QWidget *parent) : QWidget(parent)
{
    m_networkManager = new QNetworkAccessManager(this);
    
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);

    // Groupe Langue
    QGroupBox *languageGroup = new QGroupBox("Langue / Language");
    QHBoxLayout *languageLayout = new QHBoxLayout;
    m_languageSelect = new QComboBox;
    m_languageSelect->addItems({"Français", "English"});
    languageLayout->addWidget(m_languageSelect);
    languageGroup->setLayout(languageLayout);
    mainLayout->addWidget(languageGroup);

    // Groupe Compte
    QGroupBox *accountGroup = new QGroupBox("Compte");
    QHBoxLayout *accountLayout = new QHBoxLayout;
    m_accountLabel = new QLabel("THOMAS");
    m_accountLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: white;");
    accountLayout->addWidget(m_accountLabel);
    accountGroup->setLayout(accountLayout);
    mainLayout->addWidget(accountGroup);

    // Groupe État Internet
    QGroupBox *internetGroup = new QGroupBox("État de la connexion");
    QVBoxLayout *internetLayout = new QVBoxLayout;
    m_internetStatusLabel = new QLabel("Vérification...");
    m_internetStatusLabel->setStyleSheet("font-size: 16px;");
    QPushButton *checkButton = new QPushButton("Vérifier la connexion");
    internetLayout->addWidget(m_internetStatusLabel);
    internetLayout->addWidget(checkButton);
    internetGroup->setLayout(internetLayout);
    mainLayout->addWidget(internetGroup);

    // Groupe Configuration WiFi
    QGroupBox *wifiGroup = new QGroupBox("Configuration WiFi");
    QFormLayout *wifiLayout = new QFormLayout;

    // Ajout du scan des réseaux
    QHBoxLayout *scanLayout = new QHBoxLayout;
    m_networkSelect = new QComboBox;
    QPushButton *scanButton = new QPushButton("Scanner");
    
    scanLayout->addWidget(m_networkSelect, 1);
    scanLayout->addWidget(scanButton);
    wifiLayout->addRow("Réseaux disponibles:", scanLayout);

    // Connexion manuelle
    m_ssidInput = new QLineEdit;
    wifiLayout->addRow("SSID manuel:", m_ssidInput);

    m_ssidInput = new QLineEdit;
    wifiLayout->addRow("SSID:", m_ssidInput);

    m_pskInput = new QLineEdit;
    m_pskInput->setEchoMode(QLineEdit::Password);
    wifiLayout->addRow("Mot de passe:", m_pskInput);

    m_keyMgmtSelect = new QComboBox;
    m_keyMgmtSelect->addItems({"WPA-PSK", "WPA-EAP", "None"});
    wifiLayout->addRow("Sécurité:", m_keyMgmtSelect);

    m_scanSsidInput = new QLineEdit;
    m_scanSsidInput->setPlaceholderText("0 ou 1");
    wifiLayout->addRow("Scan SSID:", m_scanSsidInput);

    QPushButton *testWifiButton = new QPushButton("Tester la connexion WiFi");
    wifiLayout->addRow(testWifiButton);

    wifiGroup->setLayout(wifiLayout);
    mainLayout->addWidget(wifiGroup);

    // Bouton retour
    QPushButton *backButton = new QPushButton("Retour à l'accueil");
    backButton->setFixedWidth(200);
    mainLayout->addWidget(backButton, 0, Qt::AlignCenter);

    // Connexions
    connect(checkButton, &QPushButton::clicked, this, &Settings::checkInternetConnection);
    connect(testWifiButton, &QPushButton::clicked, this, &Settings::testWifiConnection);
    connect(backButton, &QPushButton::clicked, [this]() {
        emit requestNavigateToPage("Home");
    });

    // Vérifier la connexion internet au démarrage
    QTimer::singleShot(0, this, &Settings::checkInternetConnection);
}

void Settings::checkInternetConnection()
{
    m_internetStatusLabel->setText("Vérification...");
    m_internetStatusLabel->setStyleSheet("color: orange;");

    QNetworkRequest request(QUrl("https://www.google.com"));
    QNetworkReply *reply = m_networkManager->get(request);

    connect(reply, &QNetworkReply::finished, [this, reply]() {
        updateInternetStatus(reply->error() == QNetworkReply::NoError);
        reply->deleteLater();
    });
}

void Settings::updateInternetStatus(bool isConnected)
{
    if (isConnected) {
        m_internetStatusLabel->setText("Connecté");
        m_internetStatusLabel->setStyleSheet("color: green;");
    } else {
        m_internetStatusLabel->setText("Non connecté");
        m_internetStatusLabel->setStyleSheet("color: red;");
    }
}

void Settings::testWifiConnection()
{
    // Ici, vous pouvez implémenter la logique de test de connexion WiFi
    // Par exemple, en utilisant NetworkManager ou en exécutant des commandes système
    
    QString ssid = m_ssidInput->text();
    QString psk = m_pskInput->text();
    QString keyMgmt = m_keyMgmtSelect->currentText();
    QString scanSsid = m_scanSsidInput->text();

    // Pour l'exemple, on affiche juste une boîte de dialogue
    QMessageBox::information(this, "Test WiFi",
        QString("Test de connexion avec:\nSSID: %1\nSécurité: %2\nScan SSID: %3")
        .arg(ssid)
        .arg(keyMgmt)
        .arg(scanSsid));
}