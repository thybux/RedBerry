#include "nmap_scanner.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>

NmapScanner::NmapScanner(QWidget *parent)
    : QWidget(parent)
    , m_nmapProcess(new QProcess(this))
{
    auto mainLayout = new QVBoxLayout(this);

    // Configuration Group
    auto configGroup = new QGroupBox(tr("Configuration du scan"));
    auto configLayout = new QFormLayout;

    // Target input
    m_targetInput = new QLineEdit(this);
    configLayout->addRow(tr("Cible (IP/Hostname):"), m_targetInput);

    // Scan type
    m_scanTypeCombo = new QComboBox(this);
    m_scanTypeCombo->addItems({
        tr("Scan rapide (-T4 -F)"),
        tr("Scan complet"),
        tr("Scan furtif (-sS)"),
        tr("Scan UDP (-sU)"),
        tr("Scan complet avec tous les scripts (-sC)")
    });
    configLayout->addRow(tr("Type de scan:"), m_scanTypeCombo);

    // Port configuration
    m_portScanCheck = new QCheckBox(tr("Scanner les ports"), this);
    m_portRangeInput = new QLineEdit("1-1000", this);
    auto portLayout = new QHBoxLayout;
    portLayout->addWidget(m_portScanCheck);
    portLayout->addWidget(new QLabel(tr("Plage de ports:")));
    portLayout->addWidget(m_portRangeInput);
    configLayout->addRow(portLayout);

    // Additional options
    m_serviceDetectionCheck = new QCheckBox(tr("Détection de services (-sV)"), this);
    m_osDetectionCheck = new QCheckBox(tr("Détection d'OS (-O)"), this);
    m_scriptScanCheck = new QCheckBox(tr("Scan de scripts (-sC)"), this);
    
    configLayout->addRow(m_serviceDetectionCheck);
    configLayout->addRow(m_osDetectionCheck);
    configLayout->addRow(m_scriptScanCheck);

    configGroup->setLayout(configLayout);
    mainLayout->addWidget(configGroup);

    // Scan button
    auto scanButton = new QPushButton(tr("Lancer le scan"), this);
    mainLayout->addWidget(scanButton);

    // Output display
    m_outputDisplay = new QTextEdit(this);
    m_outputDisplay->setReadOnly(true);
    mainLayout->addWidget(m_outputDisplay);

    // Connect signals
    connect(scanButton, &QPushButton::clicked, this, &NmapScanner::runScan);
    connect(m_nmapProcess, &QProcess::readyReadStandardOutput, this, &NmapScanner::handleScanOutput);
    connect(m_nmapProcess, &QProcess::readyReadStandardError, this, &NmapScanner::handleScanError);
    connect(m_nmapProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &NmapScanner::scanFinished);
}

QString NmapScanner::buildNmapCommand()
{
    QStringList args;
    args << m_targetInput->text();

    // Scan type
    switch (m_scanTypeCombo->currentIndex()) {
        case 0: args << "-T4" << "-F"; break;
        case 2: args << "-sS"; break;
        case 3: args << "-sU"; break;
        case 4: args << "-sC"; break;
    }

    // Port range
    if (m_portScanCheck->isChecked()) {
        args << "-p" << m_portRangeInput->text();
    }

    // Additional options
    if (m_serviceDetectionCheck->isChecked()) args << "-sV";
    if (m_osDetectionCheck->isChecked()) args << "-O";
    if (m_scriptScanCheck->isChecked()) args << "-sC";

    return "nmap " + args.join(" ");
}

void NmapScanner::runScan()
{
    if (m_targetInput->text().isEmpty()) {
        QMessageBox::warning(this, tr("Erreur"), tr("Veuillez spécifier une cible"));
        return;
    }

    m_outputDisplay->clear();
    m_outputDisplay->append(tr("Démarrage du scan...\n"));
    m_outputDisplay->append(tr("Commande: ") + buildNmapCommand() + "\n");

    m_nmapProcess->start("nmap", QStringList() << "-v" << m_targetInput->text());
}

void NmapScanner::handleScanOutput()
{
    QString output = QString::fromLocal8Bit(m_nmapProcess->readAllStandardOutput());
    m_outputDisplay->append(output);
}

void NmapScanner::handleScanError()
{
    QString error = QString::fromLocal8Bit(m_nmapProcess->readAllStandardError());
    m_outputDisplay->append(tr("ERREUR: ") + error);
}

void NmapScanner::scanFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (exitStatus == QProcess::NormalExit && exitCode == 0) {
        m_outputDisplay->append(tr("\nScan terminé avec succès"));
    } else {
        m_outputDisplay->append(tr("\nLe scan a échoué avec le code: ") + QString::number(exitCode));
    }
}