#include "./include/scan_result_page.h"
#include "../libs/include/nmap_parser.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QProcess>
#include <QRegularExpression>
#include <QFile>
#include <QDesktopServices>
#include <QUrl>
#include <QCheckBox>
#include <QDebug>
#include <QEvent>


ScanResultPage::ScanResultPage(QWidget *parent) : QWidget(parent) {
  auto mainLayout = new QHBoxLayout(this);

  auto serviceLayout = new QVBoxLayout();
  m_serviceList = new QListWidget(this);

  m_serviceList->setStyleSheet("QListWidget {"
                               "    background-color: transparent;"
                               "    border: none;"
                               "}"
                               "QListWidget::item {"
                               "    background-color: transparent;"
                               "    border: none;"
                               "}");
  serviceLayout->addWidget(new QLabel(tr("Services détectés"), this));
  serviceLayout->addWidget(m_serviceList);

  auto resultLayout = new QVBoxLayout();

  auto buttonLayout = new QHBoxLayout();
  m_resultDisplay = new QTextEdit(this);
  m_resultDisplay->setReadOnly(true);

  m_scanCompletButton = new QPushButton(tr("Scan de tout les services"), this);

  connect(m_scanCompletButton, &QPushButton::clicked, [this]() {
    // Récupérer tous les services cochés de la liste
    QStringList services;
    for (int i = 0; i < m_serviceList->count(); i++) {
        QListWidgetItem* item = m_serviceList->item(i);
        QWidget* widget = m_serviceList->itemWidget(item);
        
        // Chercher la checkbox dans le widget
        if (QCheckBox* checkbox = widget->findChild<QCheckBox*>()) {
            // Si la checkbox est cochée
            if (checkbox->isChecked()) {
                // Chercher le label associé
                if (QLabel* label = widget->findChild<QLabel*>()) {
                    services << label->text();
                }
            }
        }
    }
    
    if (services.isEmpty()) {
        m_resultDisplay->append(tr("\n⚠️ Aucun service sélectionné"));
        return;
    }
    
    runScanComplete(services);
  });
  m_backButton = new QPushButton(tr("Retour au scanner"), this);
  resultLayout->addWidget(new QLabel(tr("Résultats des scripts"), this));
  resultLayout->addWidget(m_resultDisplay);
  buttonLayout->addWidget(m_scanCompletButton);
  buttonLayout->addWidget(m_backButton);
  resultLayout->addLayout(buttonLayout);

  mainLayout->addLayout(serviceLayout, 1);

  mainLayout->addLayout(resultLayout, 3);

  connect(m_backButton, &QPushButton::clicked,
          [this]() { emit requestNavigateToPage("Pentest"); });
}



void ScanResultPage::runScanComplete(const QStringList& serviceList) {
    qDebug() << "Scan complet pour les services:" << serviceList;
    
    QProcess* pythonProcess = new QProcess(this);
    
    QStringList args;
    args << "./python/exploitDb.py";
    args << serviceList;
    
    connect(pythonProcess, &QProcess::readyReadStandardOutput, [this, pythonProcess]() {
        QString output = QString::fromLocal8Bit(pythonProcess->readAllStandardOutput());
        m_resultDisplay->append(output);
    });
    
    connect(pythonProcess, &QProcess::readyReadStandardError, [this, pythonProcess]() {
        QString error = QString::fromLocal8Bit(pythonProcess->readAllStandardError());
        m_resultDisplay->append("\n❌ Erreur Python :\n" + error);
    });
    
    // Lancer le script Python
    pythonProcess->start("python3", args);
}


void ScanResultPage::setResults(const QString &target, const QString &output, bool success) {
    m_resultDisplay->clear();
    m_resultDisplay->setStyleSheet("color: white; background-color: #222;");
    m_serviceList->clear();

    m_resultDisplay->append(tr("Résultats du scan pour: %1\n").arg(target));
    m_resultDisplay->append(tr("Statut: %1\n\n").arg(success ? "Succès" : "Échec"));

    // Parser la sortie
    NmapScanInfo scanInfo = NmapParser::parseNmapOutput(output);

    // Afficher les erreurs s'il y en a
    if (!scanInfo.errors.isEmpty()) {
        m_resultDisplay->append("\nErreurs détectées:");
        for (const auto& error : scanInfo.errors) {
            m_resultDisplay->append("- " + error);
        }
        m_resultDisplay->append("");
    }

   for (const auto &portInfo : scanInfo.ports) {
    QString fullServiceName = portInfo.second;
    QString servicePort = portInfo.first;

    // Liste des services pour ce port
    QStringList services;
    
    if (fullServiceName.contains("/")) {
        // Si multiple services, les séparer
        services = fullServiceName.split("/");
    } else {
        // Sinon, juste un service
        services << fullServiceName;
    }

    for (const QString& service : services) {
        QString serviceName = service.split("(").first().trimmed();
        
        // Créer le widget container
        QWidget* serviceWidget = new QWidget(this);
        QHBoxLayout* serviceLayout = new QHBoxLayout(serviceWidget);
        
        // Créer la checkbox
        QCheckBox* checkbox = new QCheckBox(this);
        checkbox->setStyleSheet(
            "QCheckBox {"
            "    spacing: 5px;"
            "}"
            "QCheckBox::indicator {"
            "    width: 18px;"
            "    height: 18px;"
            "    border: 2px solid white;"
            "    border-radius: 5px;"
            "    background: transparent;"
            "}"
            "QCheckBox::indicator:checked {"
            "    background-color: #1e90ff;"
            "}"
        );
        
        // Créer le label
        QLabel* serviceLabel = new QLabel(serviceName, this);
        serviceLabel->setStyleSheet(
            "QLabel {"
            "    color: white;"
            "    padding: 10px 15px;"
            "    border-radius: 12px;"
            "    border: 2px solid white;"
            "    background-color: transparent;"
            "    font-size: 14px;"
            "}"
        );
        
        serviceLayout->addWidget(checkbox);
        serviceLayout->addWidget(serviceLabel, 1);
        serviceLayout->setContentsMargins(5, 5, 5, 5);
        serviceLayout->setSpacing(10);
        
        // Stocker les informations dans les propriétés du widget
        serviceWidget->setProperty("serviceName", serviceName);
        serviceWidget->setProperty("servicePort", servicePort);
        serviceWidget->setProperty("service", service);
        
        // Installer l'event filter
        serviceWidget->installEventFilter(this);
        
        QListWidgetItem* item = new QListWidgetItem(m_serviceList);
        item->setSizeHint(serviceWidget->sizeHint());
        m_serviceList->setItemWidget(item, serviceWidget);
    }

    // Afficher les informations sur l'OS si disponibles
    if (!scanInfo.osInfo.isEmpty()) {
        m_resultDisplay->append(tr("\nSystème d'exploitation détecté:"));
        m_resultDisplay->append(scanInfo.osInfo + "\n");
    }

    m_serviceList->setSpacing(10);
}
}

bool ScanResultPage::eventFilter(QObject* obj, QEvent* event) {
    if (event->type() == QEvent::MouseButtonPress) {
        QWidget* widget = qobject_cast<QWidget*>(obj);
        if (widget) {
            QString serviceName = widget->property("serviceName").toString();
            QString servicePort = widget->property("servicePort").toString();
            QString service = widget->property("service").toString();
            
            m_resultDisplay->clear();
            m_resultDisplay->append(tr("Service: %1").arg(serviceName));
            m_resultDisplay->append(tr("Port: %1").arg(servicePort));
            m_resultDisplay->append(tr("Version: %1").arg(service));

            runSearchsploit(serviceName);
            return true;
        }
    }
    return QWidget::eventFilter(obj, event);
}


void ScanResultPage::runSearchsploit(const QString& serviceName) {

    qDebug() << "Run searchsploit for" << serviceName;
    /*QProcess *searchsploitProcess = new QProcess(this);
    
    connect(searchsploitProcess, &QProcess::readyReadStandardOutput, [this, searchsploitProcess]() {
        QString output = QString::fromLocal8Bit(searchsploitProcess->readAllStandardOutput());
        QStringList lines = output.split('\n', Qt::SkipEmptyParts);
        
        for (const QString& line : lines) {
            QString cleanLine = line;
            cleanLine.remove(QRegularExpression("\033\\[[0-9;]*[a-zA-Z]"));
            
            QStringList parts = cleanLine.split('|');
            if (parts.size() >= 2) {
                QString title = parts[0].trimmed();
                QString relativePath = parts[1].trimmed();
                
                QRegularExpression re("(windows|linux|multiple)/(\\w+)/(\\d+)\\.\\w+$");
                QRegularExpressionMatch match = re.match(relativePath);
                
                if (match.hasMatch()) {
                    QString platform = match.captured(1);
                    QString type = match.captured(2);
                    QString id = match.captured(3);
                    
                    QString fullPath = QString("/opt/homebrew/opt/exploitdb/share/exploitdb/exploits/%1")
                        .arg(relativePath);

                    QString criticity;
                    QString description;

                    QFile exploitFile(fullPath);
                    if (exploitFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
                        QTextStream in(&exploitFile);
                        while (!in.atEnd()) {
                            QString fileLine = in.readLine();
                            if (fileLine.startsWith("# Criticality:")) {
                                criticity = fileLine.section(':', 1).trimmed();
                            }
                            if (fileLine.startsWith("# Description:")) {
                                description = fileLine.section(':', 1).trimmed();
                            }
                        }
                        exploitFile.close();
                    }

                    m_resultDisplay->append(QString("\n📋 Exploit ID: %1").arg(id));
                    m_resultDisplay->append(QString("🔍 Titre: %1").arg(title));
                    m_resultDisplay->append(QString("💻 Plateforme: %1").arg(platform.toUpper()));
                    m_resultDisplay->append(QString("⚠️ Type: %1").arg(type.toUpper()));
                    m_resultDisplay->append(QString("🔥 Criticité: %1").arg(criticity.isEmpty() ? "Inconnue" : criticity));
                    m_resultDisplay->append(QString("📖 Description: %1").arg(description.isEmpty() ? "Non disponible" : description));

                    QPushButton* openButton = new QPushButton(QString("📁 Ouvrir: %1").arg(relativePath), this);
                    openButton->setStyleSheet(
                        "QPushButton {"
                        "    color: #1e90ff;"
                        "    background: transparent;"
                        "    border: none;"
                        "    text-align: left;"
                        "    padding: 5px;"
                        "    text-decoration: underline;"
                        "}"
                        "QPushButton:hover {"
                        "    color: #87CEFA;"
                        "}"
                    );

                    connect(openButton, &QPushButton::clicked, [this, fullPath]() {
                        QFile file(fullPath);
                        if (file.exists()) {
                            QDesktopServices::openUrl(QUrl::fromLocalFile(fullPath));
                        } else {
                            m_resultDisplay->append("\n❌ Fichier non trouvé : " + fullPath);
                            m_resultDisplay->append("Vérifiez que exploitdb est bien installé : brew install exploitdb");
                        }
                    });

                    QWidget* container = new QWidget(this);
                    QHBoxLayout* layout = new QHBoxLayout(container);
                    layout->addWidget(openButton);
                    layout->addStretch();
                    layout->setContentsMargins(0, 0, 0, 0);
                    container->setLayout(layout);

                    QTextCursor cursor = m_resultDisplay->textCursor();
                    cursor.movePosition(QTextCursor::End);
                    m_resultDisplay->setTextCursor(cursor);

                    QTextBlockFormat blockFormat;
                    blockFormat.setTopMargin(5);
                    blockFormat.setBottomMargin(5);
                    cursor.insertBlock(blockFormat);

                    m_resultDisplay->insertHtml("<br>");
                    QTextCursor newCursor = m_resultDisplay->textCursor();
                    newCursor.movePosition(QTextCursor::End);
                    m_resultDisplay->setTextCursor(newCursor);

                    m_resultDisplay->append("----------------------------------------");
                }
            }
        }
    });

    connect(searchsploitProcess, &QProcess::readyReadStandardError, [this, searchsploitProcess]() {
        QString error = QString::fromLocal8Bit(searchsploitProcess->readAllStandardError());
        m_resultDisplay->append("\n❌ Erreur searchsploit :");
        m_resultDisplay->append(error);
    });

    connect(searchsploitProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            [this, searchsploitProcess](int exitCode, QProcess::ExitStatus exitStatus) {
        if (exitCode != 0) {
            m_resultDisplay->append("\n❌ La recherche a échoué. Code de sortie : " + QString::number(exitCode));
        }
        searchsploitProcess->deleteLater();
    });

    QStringList args;
    args << serviceName;
    
    m_resultDisplay->append(tr("\n🔎 Recherche de vulnérabilités pour %1...").arg(serviceName));
    searchsploitProcess->start("searchsploit", args);
    */
}
