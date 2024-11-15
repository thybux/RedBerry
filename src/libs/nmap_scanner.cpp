#include "./include/nmap_scanner.h"
#include <QFile>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QProcess>
#include <QComboBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QTextEdit>
#include <QDebug>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QTextEdit>

NmapScanner::NmapScanner(QWidget *parent)
    : QWidget(parent)
    , m_nmapProcess(new QProcess(this))
{

    m_targetInput = new QLineEdit(this);
    m_scanTypeCombo = new QComboBox(this);
    m_portScanCheck = new QCheckBox(tr("Scanner les ports"), this);
    m_portRangeInput = new QLineEdit("1-1000", this);
    m_serviceDetectionCheck = new QCheckBox(tr("Détection de services (-sV)"), this);
    m_osDetectionCheck = new QCheckBox(tr("Détection d'OS (-O)"), this);
    m_scriptScanCheck = new QCheckBox(tr("Scan de scripts (-sC)"), this);

    auto mainLayout = new QVBoxLayout(this);

    auto configGroup = new QGroupBox(tr("Configuration du scan"));
    auto configLayout = new QVBoxLayout;

    QWidget *targetContainer = new QWidget;
    QVBoxLayout *targetLayout = new QVBoxLayout(targetContainer);
    targetLayout->addWidget(new QLabel(tr("Cible (IP/Hostname):")));
    targetLayout->addWidget(m_targetInput);
    configLayout->addWidget(targetContainer);

    QWidget *scanTypeContainer = new QWidget;
    QHBoxLayout *scanTypeLayout = new QHBoxLayout(scanTypeContainer);
    scanTypeLayout->addWidget(new QLabel(tr("Type de scan:")));
    m_scanTypeCombo->addItems({
        tr("Scan rapide (-T4 -F)"),
        tr("Scan complet"),
        tr("Scan furtif (-sS)"),
        tr("Scan UDP (-sU)"),
        tr("Scan complet avec tous les scripts (-sC)")
    });
    scanTypeLayout->addWidget(m_scanTypeCombo);
    configLayout->addWidget(scanTypeContainer);

    QWidget *portContainer = new QWidget;
    QHBoxLayout *portLayout = new QHBoxLayout(portContainer);

    QHBoxLayout *portLayout2 = new QHBoxLayout;
    portLayout2->addWidget(new QLabel(tr("Plage de ports:")));


    portLayout->addWidget(m_portScanCheck);
    portLayout->addWidget(m_portRangeInput);
    configLayout->addWidget(portContainer);

    configGroup->setLayout(configLayout);
    mainLayout->addWidget(configGroup);

    auto optionsGroup = new QGroupBox(tr("Options additionnelles"));
    auto optionsLayout = new QHBoxLayout;
    optionsLayout->addWidget(m_serviceDetectionCheck);
    optionsLayout->addWidget(m_osDetectionCheck);
    optionsLayout->addWidget(m_scriptScanCheck);
    optionsGroup->setLayout(optionsLayout);
    mainLayout->addWidget(optionsGroup);

    auto scanButton = new QPushButton(tr("Lancer le scan"), this);
    mainLayout->addWidget(scanButton);

    m_outputDisplay = new QTextEdit(this);
    m_outputDisplay->setReadOnly(true);
    m_outputDisplay->hide();

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
    bool success = (exitStatus == QProcess::NormalExit && exitCode == 0);
    QString output = m_outputDisplay->toPlainText();
    
    // Écrire la sortie dans un fichier .txt
    QFile file("src/libs/output_nmap_scann/output.txt");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << output;
        file.close();
    } else {
        qWarning() << "Impossible d'ouvrir le fichier pour écrire les résultats du scan.";
    }

    // emettre le signal pour changer de page
    emit scanCompleted(m_targetInput->text(), output, success);
}