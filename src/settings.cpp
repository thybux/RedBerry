#include "settings.h"
#include <QVBoxLayout>
#include <QGroupBox>
#include <QFormLayout>
#include <QLabel>

Settings::Settings(QWidget *parent)
    : QWidget(parent)
{
    auto mainLayout = new QVBoxLayout(this);
    auto contentLayout = new QVBoxLayout();

    contentLayout->addWidget(createNetworkSection());
    contentLayout->addWidget(createProxySection());
    contentLayout->addWidget(createScanningSection());
    contentLayout->addWidget(createPayloadSection());
    contentLayout->addWidget(createLoggingSection());

    mainLayout->addLayout(contentLayout);
    setLayout(mainLayout);
}

QGroupBox* Settings::createNetworkSection()
{
    auto groupBox = new QGroupBox(tr("Network Settings"));
    auto layout = new QFormLayout;

    m_interfaceEdit = new QLineEdit(this);
    m_timeoutSpinBox = new QSpinBox(this);
    m_protocolCombo = new QComboBox(this);

    layout->addRow(tr("Interface:"), m_interfaceEdit);
    layout->addRow(tr("Timeout (s):"), m_timeoutSpinBox);
    layout->addRow(tr("Protocol:"), m_protocolCombo);

    groupBox->setLayout(layout);
    return groupBox;
}

QGroupBox* Settings::createProxySection()
{
    auto groupBox = new QGroupBox(tr("Proxy Settings"));
    auto layout = new QFormLayout;

    m_proxyHostEdit = new QLineEdit(this);
    m_proxyPortSpinBox = new QSpinBox(this);
    m_proxyUserEdit = new QLineEdit(this);
    m_proxyPassEdit = new QLineEdit(this);
    m_proxyPassEdit->setEchoMode(QLineEdit::Password);

    layout->addRow(tr("Host:"), m_proxyHostEdit);
    layout->addRow(tr("Port:"), m_proxyPortSpinBox);
    layout->addRow(tr("Username:"), m_proxyUserEdit);
    layout->addRow(tr("Password:"), m_proxyPassEdit);

    groupBox->setLayout(layout);
    return groupBox;
}

QGroupBox* Settings::createScanningSection()
{
    auto groupBox = new QGroupBox(tr("Scanning Settings"));
    auto layout = new QFormLayout;

    m_threadCountSpinBox = new QSpinBox(this);
    m_portRangeStartSpinBox = new QSpinBox(this);
    m_portRangeEndSpinBox = new QSpinBox(this);
    m_scanTypeCombo = new QComboBox(this);

    layout->addRow(tr("Thread Count:"), m_threadCountSpinBox);
    layout->addRow(tr("Port Range Start:"), m_portRangeStartSpinBox);
    layout->addRow(tr("Port Range End:"), m_portRangeEndSpinBox);
    layout->addRow(tr("Scan Type:"), m_scanTypeCombo);

    groupBox->setLayout(layout);
    return groupBox;
}

QGroupBox* Settings::createPayloadSection()
{
    auto groupBox = new QGroupBox(tr("Payload Settings"));
    auto layout = new QFormLayout;

    m_payloadPathEdit = new QLineEdit(this);
    m_encodingCombo = new QComboBox(this);
    m_payloadSizeSpinBox = new QSpinBox(this);

    layout->addRow(tr("Payload Path:"), m_payloadPathEdit);
    layout->addRow(tr("Encoding:"), m_encodingCombo);
    layout->addRow(tr("Payload Size:"), m_payloadSizeSpinBox);

    groupBox->setLayout(layout);
    return groupBox;
}

QGroupBox* Settings::createLoggingSection()
{
    auto groupBox = new QGroupBox(tr("Logging Settings"));
    auto layout = new QFormLayout;

    m_logPathEdit = new QLineEdit(this);
    m_logLevelCombo = new QComboBox(this);

    layout->addRow(tr("Log Path:"), m_logPathEdit);
    layout->addRow(tr("Log Level:"), m_logLevelCombo);

    groupBox->setLayout(layout);
    return groupBox;
}