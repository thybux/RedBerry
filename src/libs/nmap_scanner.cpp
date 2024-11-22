#include "./include/nmap_scanner.h"
#include <QFile>
#include <QDir>
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
    : QWidget(parent)
    , m_nmapProcess(new QProcess(this))
{
    auto mainLayout = new QVBoxLayout(this);

    setupConfigGroup(mainLayout);
    setupOptionsGroup(mainLayout);
    setupOutputDisplay(mainLayout);

    scanButton = new QPushButton(tr("Lancer le scan"), this);
    scanButton->setStyleSheet("color: white; background-color: #222; border: 2px solid #222; border-radius: 12px;");
    mainLayout->addWidget(scanButton);

    setupConnections();
}

void NmapScanner::setupConnections()
{
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

    // Target Input Section
    auto targetLayout = new QVBoxLayout;
    targetLayout->addWidget(new QLabel(tr("Cible (IP/Hostname):")));
    m_targetInput = new QLineEdit(this);
    m_targetInput->setStyleSheet("color: white; background-color: #222; border: 2px solid #222; border-radius: 12px;");
    targetLayout->addWidget(m_targetInput);
    configLayout->addLayout(targetLayout);

    // Scan Type Section
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

    // Port Configuration Section
    auto portLayout = new QVBoxLayout;
    m_portScanCheck = new QCheckBox(tr("Scanner les ports"), this);
    m_portScanCheck->setStyleSheet("color: white;");
    m_portRangeInput = new QLineEdit("1-1000", this);
    m_portRangeInput->setStyleSheet("color: white; background-color: #222; border: 2px solid #222; border-radius: 12px;");
    
    portLayout->addWidget(m_portScanCheck);
    portLayout->addWidget(new QLabel(tr("Plage de ports:")));
    portLayout->addWidget(m_portRangeInput);
    portLayout->setSpacing(10);
    
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
    m_osDetectionCheck = new QCheckBox(tr("Détection d'OS (-O)"), this);
    m_scriptScanCheck = new QCheckBox(tr("Scan de scripts (-sC)"), this);

    QList<QCheckBox*> checkboxes = {m_serviceDetectionCheck, m_osDetectionCheck, m_scriptScanCheck};
    for (auto checkbox : checkboxes) {
        checkbox->setStyleSheet("color: white;");
        optionsLayout->addWidget(checkbox);
    }

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

bool NmapScanner::validateScanParameters() const
{
    if (m_targetInput->text().isEmpty()) {
        QMessageBox::warning(nullptr, tr("Erreur"), tr("Veuillez spécifier une cible"));
        return false;
    }

    if (m_portScanCheck->isChecked() && m_portRangeInput->text().isEmpty()) {
        QMessageBox::warning(nullptr, tr("Erreur"), tr("Veuillez spécifier une plage de ports"));
        return false;
    }

    return true;
}

QString NmapScanner::buildNmapCommand()
{
    QStringList args;
    args << m_targetInput->text();

    // Scan type options
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

    // Port range
    if (m_portScanCheck->isChecked()) {
        args << "-p" << m_portRangeInput->text();
    }

    // Additional options
    if (m_serviceDetectionCheck->isChecked()) args << "-sV";
    if (m_osDetectionCheck->isChecked()) args << "-O";
    if (m_scriptScanCheck->isChecked()) args << "-sC";

    // Toujours ajouter -sV pour la détection de version
    if (!args.contains("-sV")) {
        args << "-sV";
    }

    return "nmap " + args.join(" ");
}

void NmapScanner::saveScanResults(const QString& output)
{
    const QString outputDir = "src/libs/output_nmap_scann";
    QDir().mkpath(outputDir); // Crée le répertoire s'il n'existe pas

    QFile file(outputDir + "/output.txt");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << output;
        file.close();
    } else {
        qWarning() << "Impossible d'ouvrir le fichier pour écrire les résultats du scan:"
                   << file.errorString();
    }
}

void NmapScanner::runScan()
{
    if (!validateScanParameters()) {
        return;
    }

    QString command = buildNmapCommand();
    QStringList args = command.split(' ', Qt::SkipEmptyParts);
    args.removeFirst(); // Enlever "nmap"

    // qDebug() << "Commande Nmap:" << command;

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
    qWarning() << "Erreur de scan Nmap:" << error;
    m_outputDisplay->append(tr("ERREUR: ") + error);
}

void NmapScanner::scanFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    scanButton->setEnabled(true);
    scanButton->setText(tr("Lancer le scan"));
    
    bool success = (exitStatus == QProcess::NormalExit && exitCode == 0);
    QString output = m_outputDisplay->toPlainText();
    
    saveScanResults(output);

    if (!success) {
        qWarning() << "Scan terminé avec des erreurs. Code de sortie:" << exitCode;
    }

    emit scanCompleted(m_targetInput->text(), output, success);
}