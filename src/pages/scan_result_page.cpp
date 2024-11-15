#include "./include/scan_result_page.h"
#include "../libs/include/nmap_parser.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>

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
  m_resultDisplay = new QTextEdit(this);
  m_resultDisplay->setReadOnly(true);

  m_backButton = new QPushButton(tr("Retour au scanner"), this);
  resultLayout->addWidget(new QLabel(tr("Résultats des scripts"), this));
  resultLayout->addWidget(m_resultDisplay);
  resultLayout->addWidget(m_backButton);

  mainLayout->addLayout(serviceLayout, 1);

  mainLayout->addLayout(resultLayout, 3);

  connect(m_backButton, &QPushButton::clicked,
          [this]() { emit requestNavigateToPage("Pentest"); });
}

void ScanResultPage::setResults(const QString &target, const QString &output,
                                bool success) {
  m_resultDisplay->clear();
  m_resultDisplay->setStyleSheet("color: white; background-color: #222;");
  m_serviceList->clear();

  m_resultDisplay->append(tr("Résultats du scan pour: %1\n").arg(target));
  m_resultDisplay->append(
      tr("Statut: %1\n\n").arg(success ? "Succès" : "Échec"));

  auto parsedResults = NmapParser::parseNmapOutput(output);
  for (const auto &result : parsedResults) {
    // QString serviceText = tr("Port: %1, Service: %2").arg(result.first,
    // result.second);
    QString serviceName = tr("%1").arg(result.second);
    QString servicePort = tr("%1").arg(result.first);

    QPushButton *serviceButton = new QPushButton(serviceName, this);
    serviceButton->setStyleSheet("QPushButton {"
                                 "    color: white;"
                                 "    text-align: left;"
                                 "    padding: 10px 15px;"
                                 "    border-radius: 12px;"
                                 "    border: 2px solid white;"
                                 "    background-color: transparent;"
                                 "    font-size: 14px;"
                                 "}"
                                 "QPushButton:hover {"
                                 "    border-color: #1e90ff;"
                                 "    background-color: #444;"
                                 "}"
                                 "QPushButton:pressed {"
                                 "    background-color: #555;"
                                 "    border-color: #1e90ff;"
                                 "}"
                                 "QPushButton:focus {"
                                 "    outline: none;"
                                 "    border: 2px solid #1e90ff;"
                                 "}");

    connect(serviceButton, &QPushButton::clicked, [this, serviceName]() {
      m_resultDisplay->append(tr("Vous avez cliqué sur : %1").arg(serviceName));
    });

    QListWidgetItem *item = new QListWidgetItem(m_serviceList);
    item->setSizeHint(serviceButton->sizeHint());
    m_serviceList->setItemWidget(item, serviceButton);
  }
  m_serviceList->setSpacing(10);
}
