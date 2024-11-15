#include "./include/nmap_scanner.h"
#include <QFile>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QProcess>
#include <QComboBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QTextEdit>
#include <QDebug>

NmapScanner::NmapScanner(QWidget *parent)
    : QWidget(parent), m_nmapProcess(new QProcess(this))
{
    auto mainLayout = new QVBoxLayout(this);

    setupConfigGroup(mainLayout);
    setupOptionsGroup(mainLayout);
    setupOutputDisplay(mainLayout);

    scanButton = new QPushButton(tr("Lancer le scan"), this);
    mainLayout->addWidget(scanButton);

    connect(scanButton, &QPushButton::clicked, this, &NmapScanner::runScan);
    connect(m_nmapProcess, &QProcess::readyReadStandardOutput, this, &NmapScanner::handleScanOutput);
    connect(m_nmapProcess, &QProcess::readyReadStandardError, this, &NmapScanner::handleScanError);
    connect(m_nmapProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &NmapScanner::scanFinished);
}

void NmapScanner::setupConfigGroup(QVBoxLayout *mainLayout)
{
    auto configGroup = new QGroupBox(tr("Configuration du scan"));
    configGroup->setStyleSheet("color: white;");
    auto configLayout = new QVBoxLayout;

    auto targetLayout = new QVBoxLayout;
    targetLayout->addWidget(new QLabel(tr("Cible (IP/Hostname):")));
    m_targetInput = new QLineEdit(this);
    m_targetInput->setStyleSheet("color: white; background-color: #222; border: 2px solid #222; border-radius: 12px;");
    targetLayout->addWidget(m_targetInput);
    configLayout->addLayout(targetLayout);

    auto scanTypeLayout = new QVBoxLayout;
    scanTypeLayout->addWidget(new QLabel(tr("Type de scan:")));
    m_scanTypeCombo = new QComboBox(this);
    m_scanTypeCombo->addItems({
        tr("Scan rapide (-T4 -F)"),
        tr("Scan complet"),
        tr("Scan furtif (-sS)"),
        tr("Scan UDP (-sU)"),
        tr("Scan complet avec tous les scripts (-sC)")
    });
    m_scanTypeCombo->setStyleSheet("color: black;");
    scanTypeLayout->addWidget(m_scanTypeCombo);
    configLayout->addLayout(scanTypeLayout);

    auto portLayout = new QHBoxLayout;
    m_portScanCheck = new QCheckBox(tr("Scanner les ports"), this);
    m_portScanCheck->setStyleSheet("color: white;");
    m_portRangeInput = new QLineEdit("1-1000", this);
    portLayout->addWidget(m_portScanCheck);
    portLayout->addWidget(new QLabel(tr("Plage de ports:")));
    m_portRangeInput->setStyleSheet("color: white; background-color: #222; border: 2px solid #222; border-radius: 12px;");
    portLayout->addWidget(m_portRangeInput);
    configLayout->addLayout(portLayout);

    configGroup->setLayout(configLayout);
    mainLayout->addWidget(configGroup);
}

void NmapScanner::setupOptionsGroup(QVBoxLayout *mainLayout)
{
    auto optionsGroup = new QGroupBox(tr("Options additionnelles"));
    optionsGroup->setStyleSheet("color: white;");
    auto optionsLayout = new QHBoxLayout;
    m_serviceDetectionCheck = new QCheckBox(tr("Détection de services (-sV)"), this);
    m_serviceDetectionCheck->setStyleSheet("color: white;");
    m_osDetectionCheck = new QCheckBox(tr("Détection d'OS (-O)"), this);
    m_osDetectionCheck->setStyleSheet("color: white;");
    m_scriptScanCheck = new QCheckBox(tr("Scan de scripts (-sC)"), this);
    m_scriptScanCheck->setStyleSheet("color: white;");
    optionsLayout->addWidget(m_serviceDetectionCheck);
    optionsLayout->addWidget(m_osDetectionCheck);
    optionsLayout->addWidget(m_scriptScanCheck);
    optionsGroup->setLayout(optionsLayout);
    mainLayout->addWidget(optionsGroup);
}

void NmapScanner::setupOutputDisplay(QVBoxLayout *mainLayout)
{
    m_outputDisplay = new QTextEdit(this);
    m_outputDisplay->setReadOnly(true);
    m_outputDisplay->hide();
    mainLayout->addWidget(m_outputDisplay);
}

QString NmapScanner::buildNmapCommand()
{
    QStringList args;
    args << m_targetInput->text();

    QMap<int, QStringList> scanOptions = {
        {0, {"-T4", "-F"}}, 
        {1, {"-p-"}},     
        {2, {"-sS"}},
        {3, {"-sU"}},
        {4, {"-sC"}}
    };

    if (scanOptions.contains(m_scanTypeCombo->currentIndex())) {
        args << scanOptions[m_scanTypeCombo->currentIndex()];
    }

    if (m_portScanCheck->isChecked()) {
        args << "-p" << m_portRangeInput->text();
    }
    if (m_serviceDetectionCheck->isChecked()) args << "-sV";
    if (m_osDetectionCheck->isChecked()) args << "-O";
    if (m_scriptScanCheck->isChecked()) args << "-sC";

    return "nmap " + args.join(" ");
}

void NmapScanner::runScan()
{
    const QString target = m_targetInput->text();
    if (target.isEmpty()) {
        QMessageBox::warning(this, tr("Erreur"), tr("Veuillez spécifier une cible"));
        return;
    }

    QString command = buildNmapCommand();
    QStringList args = command.split(' ', Qt::SkipEmptyParts);
    args.removeFirst();

    m_outputDisplay->clear();
    m_outputDisplay->append(tr("Démarrage du scan...\n"));
    m_outputDisplay->append(tr("Commande: ") + command + "\n");

    scanButton->setEnabled(false);
    scanButton->setText(tr("Scan en cours..."));
    m_nmapProcess->start("nmap", args);
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
    scanButton->setEnabled(true);
    scanButton->setText(tr("Lancer le scan"));
    bool success = (exitStatus == QProcess::NormalExit && exitCode == 0);
    QString output = m_outputDisplay->toPlainText();
    
    QFile file("src/libs/output_nmap_scann/output.txt");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << output;
        file.close();
    } else {
        qWarning() << "Impossible d'ouvrir le fichier pour écrire les résultats du scan.";
    }

    emit scanCompleted(m_targetInput->text(), output, success);
}
