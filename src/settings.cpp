#include "settings.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

Settings::Settings(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    QLabel *label = new QLabel("Contenu de la page Settings", this);
    layout->addWidget(label);

    QPushButton *backButton = new QPushButton("Retour à l'accueil", this);
    layout->addWidget(backButton);

    connect(backButton, &QPushButton::clicked, [this]() {
        emit requestNavigateToPage("Home");
    });
}
