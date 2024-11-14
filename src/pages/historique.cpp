#include "./include/historique.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>

Historique::Historique(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QLabel *titleLabel = new QLabel("Historique", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 24px; color: white; margin: 20px;");
    mainLayout->addWidget(titleLabel);

    // Ajoutez ici le contenu de votre page historique

    QPushButton *backButton = new QPushButton("Retour à l'accueil", this);
    backButton->setFixedWidth(200);
    mainLayout->addWidget(backButton, 0, Qt::AlignCenter);

    connect(backButton, &QPushButton::clicked, [this]() {
        emit requestNavigateToPage("Home");
    });
}