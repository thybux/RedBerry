#include "attack.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

Attack::Attack(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    QLabel *label = new QLabel("Contenu de la page Attack", this);
    layout->addWidget(label);

    QPushButton *backButton = new QPushButton("Retour à l'accueil", this);
    layout->addWidget(backButton);

    connect(backButton, &QPushButton::clicked, [this]() {
        emit requestNavigateToPage("Home");
    });
}
