#include "mainwindow.h"
#include "settings.h"
#include "attack.h"
#include "script.h"
#include "profile.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QStackedWidget>
#include <QFile>
#include <QTextStream>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    m_stackedWidget = new QStackedWidget(this);
    mainLayout->addWidget(m_stackedWidget);

    createHomePage();
    createPages();

    // Chargement et application du style
    QFile styleFile(":/resources/stylesheet/homePage.qss");
    if (styleFile.open(QFile::ReadOnly | QFile::Text))
    {
        QTextStream styleStream(&styleFile);
        this->setStyleSheet(styleStream.readAll());
        styleFile.close();
    }
    else
    {
        qWarning() << "Impossible de charger le fichier de style dans MainWindow";
    }

    setWindowTitle("RedBerry - Accueil");
    resize(300, 500);
}

void MainWindow::createHomePage()
{
    QWidget *homePage = new QWidget;
    QVBoxLayout *homeLayout = new QVBoxLayout(homePage);

    // Ajouter un espace extensible en haut
    homeLayout->addStretch();

    QLabel *titleLabel = new QLabel("RedBerry", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    homeLayout->addWidget(titleLabel);

    // Créer un widget pour contenir les boutons
    QWidget *buttonWidget = new QWidget;
    QVBoxLayout *buttonLayout = new QVBoxLayout(buttonWidget);

    QStringList buttonNames = {"Settings", "Attack", "Script", "Profile"};
    for (const QString &name : buttonNames)
    {
        QPushButton *button = createMenuButton(name, name);
        buttonLayout->addWidget(button);
    }

    // Supprimer les marges du layout des boutons
    buttonLayout->setContentsMargins(0, 0, 0, 0);
    buttonLayout->setSpacing(10); // Espace entre les boutons

    // Ajouter le widget des boutons au layout principal
    homeLayout->addWidget(buttonWidget, 0, Qt::AlignCenter);

    // Ajouter un espace extensible en bas
    homeLayout->addStretch();

    m_pages.insert("Home", homePage);
    m_stackedWidget->addWidget(homePage);
}

void MainWindow::createPages()
{
    Settings *settingsPage = new Settings(this);
    m_pages.insert("Settings", settingsPage);
    m_stackedWidget->addWidget(settingsPage);
    connect(settingsPage, &Settings::requestNavigateToPage, this, &MainWindow::navigateToPage);

    Attack *attackPage = new Attack(this);
    m_pages.insert("Attack", attackPage);
    m_stackedWidget->addWidget(attackPage);
    connect(attackPage, &Attack::requestNavigateToPage, this, &MainWindow::navigateToPage);

    Script *scriptPage = new Script(this);
    m_pages.insert("Script", scriptPage);
    m_stackedWidget->addWidget(scriptPage);
    connect(scriptPage, &Script::requestNavigateToPage, this, &MainWindow::navigateToPage);

    Profile *profilePage = new Profile(this);
    m_pages.insert("Profile", profilePage);
    m_stackedWidget->addWidget(profilePage);
    connect(profilePage, &Profile::requestNavigateToPage, this, &MainWindow::navigateToPage);
}

QPushButton* MainWindow::createMenuButton(const QString &text, const QString &pageName)
{
    QPushButton *button = new QPushButton(text, this);
    button->setFixedSize(220, 60); // Taille fixe pour tous les boutons
    button->setCursor(Qt::PointingHandCursor);
    button->setStyleSheet(
        "QPushButton { "
        "   background-color: #3498db; "
        "   color: white; "
        "   border: 2px solid #2980b9; "
        "   border-radius: 10px; "
        "   padding: 5px; "
        "}"
        "QPushButton:hover { background-color: #2980b9; }"
        "QPushButton:pressed { background-color: #2573a7; }"
    );
    
    connect(button, &QPushButton::clicked, this, [this, pageName]() { navigateToPage(pageName); });
    return button;
}

void MainWindow::onMenuButtonClicked()
{
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    if (button)
    {
        QString pageName = button->text();
        navigateToPage(pageName);
    }
}

void MainWindow::navigateToPage(const QString &pageName)
{
    if (m_pages.contains(pageName))
    {
        QWidget *page = m_pages.value(pageName);
        m_stackedWidget->setCurrentWidget(page);
    }
    else
    {
        qDebug() << "Page not found:" << pageName;
    }
}