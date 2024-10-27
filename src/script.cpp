#include "script.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

Script::Script(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    QLabel *label = new QLabel("Contenu de la page Script", this);
    layout->addWidget(label);

    QPushButton *backButton = new QPushButton("Retour à l'accueil", this);
    layout->addWidget(backButton);

    connect(backButton, &QPushButton::clicked, [this]() {
        emit requestNavigateToPage("Home");
    });
}
