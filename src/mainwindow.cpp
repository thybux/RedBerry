#include "./mainwindow.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QWidget>
#include <QStackedWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    m_stackedWidget = new QStackedWidget(this);
    mainLayout->addWidget(m_stackedWidget);

    // Créez la première page
    QWidget *page1 = new QWidget;
    QVBoxLayout *layout1 = new QVBoxLayout(page1);
    layout1->addWidget(new QLabel("Page 1", page1));
    m_navigateButton = new QPushButton("Aller à la page 2", page1);
    layout1->addWidget(m_navigateButton);

    // Créez la deuxième page
    QWidget *page2 = new QWidget;
    QVBoxLayout *layout2 = new QVBoxLayout(page2);
    layout2->addWidget(new QLabel("Page 2", page2));
    QPushButton *backButton = new QPushButton("Retour à la page 1", page2);
    layout2->addWidget(backButton);

    // Ajoutez les pages au QStackedWidget
    m_stackedWidget->addWidget(page1);
    m_stackedWidget->addWidget(page2);

    // Connectez les boutons
    connect(m_navigateButton, &QPushButton::clicked, this, &MainWindow::onNavigateButtonClicked);
    connect(backButton, &QPushButton::clicked, [this]() { m_stackedWidget->setCurrentIndex(0); });

    setWindowTitle("Navigation entre pages");
    resize(400, 300);
}

void MainWindow::onNavigateButtonClicked()
{
    m_stackedWidget->setCurrentIndex(1);
}

MainWindow::~MainWindow()
{
}

