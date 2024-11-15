#include "./mainwindow.h"
#include "./pages/include/scan_result_page.h"
#include "./libs/include/nmap_scanner.h"
#include "./pages/include/settings.h"
#include "./pages/include/pentest.h"
#include "./pages/include/historique.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QPainter>
#include <QLabel>
#include <QPixmap>
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

    QFile styleFile(":/resources/stylesheet/homePage.qss");
    qDebug() << "Tentative de chargement du style depuis:" << styleFile.fileName();
    
    if (!styleFile.exists()) {
        qWarning() << "Le fichier de style n'existe pas!";
        return;
    }
    
    if (styleFile.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream styleStream(&styleFile);
        QString styleSheet = styleStream.readAll();
        this->setStyleSheet(styleSheet);
        styleFile.close();
    } else {
        qWarning() << "Erreur lors de l'ouverture du fichier:" << styleFile.errorString();
    }

    setWindowTitle("RedBerry - Accueil");
    resize(500, 800);
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.drawPixmap(rect(), m_background, m_background.rect());
}


/**
 * @brief Crée la page d'accueil
 */
void MainWindow::createHomePage()
{
    QWidget *homePage = new QWidget;
    QVBoxLayout *homeLayout = new QVBoxLayout(homePage);

    homeLayout->addStretch();

    QLabel *titleLabel = new QLabel("RedBerry", this);
    titleLabel->setStyleSheet("color: white; font-size: 36px; font-weight: bold; margin-bottom: 20px;");
    titleLabel->setAlignment(Qt::AlignCenter);
    homeLayout->addWidget(titleLabel);

    QWidget *buttonWidget = new QWidget;
    QHBoxLayout *buttonLayout = new QHBoxLayout(buttonWidget);

    QStringList buttonNames = {"Settings", "Pentest", "Historique"};
    for (const QString &name : buttonNames)
    {
        QPushButton *button = createMenuButton(name, name);
        buttonLayout->addWidget(button);
    }

    buttonLayout->setContentsMargins(0, 0, 0, 0);
    buttonLayout->setSpacing(10);

    homeLayout->addWidget(buttonWidget, 0, Qt::AlignCenter);

    homeLayout->addStretch();

    m_pages.insert("Home", homePage);
    m_stackedWidget->addWidget(homePage);
}

/**
 * @brief Crée les pages
 * page pour les paramètres, le pentest et l'historique
 */
void MainWindow::createPages()
{
    Settings *settingsPage = new Settings(this);
    m_pages.insert("Settings", settingsPage);
    m_stackedWidget->addWidget(settingsPage);
    connect(settingsPage, &Settings::requestNavigateToPage, this, &MainWindow::navigateToPage);

    Pentest *pentestPage = new Pentest(this);
    m_pages.insert("Pentest", pentestPage);
    m_stackedWidget->addWidget(pentestPage);
    connect(pentestPage, &Pentest::requestNavigateToPage, this, &MainWindow::navigateToPage);

    NmapScanner *nmapScanner = pentestPage->findChild<NmapScanner*>();
    if (nmapScanner) {
        connect(nmapScanner, &NmapScanner::scanCompleted, this, &MainWindow::showScanResults);
    }

    ScanResultPage *resultPage = new ScanResultPage(this);
    m_pages.insert("ScanResults", resultPage);
    m_stackedWidget->addWidget(resultPage);
    connect(resultPage, &ScanResultPage::requestNavigateToPage, this, &MainWindow::navigateToPage);


    Historique *historiquePage = new Historique(this);
    m_pages.insert("Historique", historiquePage);
    m_stackedWidget->addWidget(historiquePage);
    connect(historiquePage, &Historique::requestNavigateToPage, this, &MainWindow::navigateToPage);
}

/**
 * @brief Crée les boutons de la page d'accueil
 */
QPushButton* MainWindow::createMenuButton(const QString &text, const QString &pageName)
{
    QWidget *container = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(container);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(5);
    
    QLabel *iconLabel = new QLabel;
    QString iconPath;
    if (pageName == "Settings") {
        iconPath = ":/resources/img/assets/settings.png";
    } else if (pageName == "Pentest") {
        iconPath = ":/resources/img/assets/pentest.png";
    } else if (pageName == "Historique") {
        iconPath = ":/resources/img/assets/history.png";
    }
    
    iconLabel->setPixmap(QPixmap(iconPath).scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    layout->addWidget(iconLabel, 0, Qt::AlignCenter);
    
    QLabel *textLabel = new QLabel(text);
    textLabel->setStyleSheet("font-size: 20px; font-weight: bold; margin-top: 10px;");
    layout->addWidget(textLabel, 0, Qt::AlignCenter);
    
    QPushButton *button = new QPushButton(this);
    button->setFixedSize(220, 120);
    button->setCursor(Qt::PointingHandCursor);

    
    QVBoxLayout *buttonLayout = new QVBoxLayout(button);
    buttonLayout->setContentsMargins(5, 5, 5, 5);
    buttonLayout->addWidget(container);
    
    button->setStyleSheet(
        "QPushButton { "
        "   background-color: transparent; "
        "   border: none; "
        "   padding: 5px; "
        "}"
        "QPushButton:hover { "
        "   background-color: rgba(255, 255, 255, 30); " 
        "}"
        "QPushButton:pressed { "
        "   background-color: rgba(255, 255, 255, 50); "
        "}"
    );
    
    connect(button, &QPushButton::clicked, this, [this, pageName]() { 
        navigateToPage(pageName); 
    });
    
    return button;
}

/**
 * @brief Gère le clic sur un bouton de la page d'accueil
 */
void MainWindow::onMenuButtonClicked()
{
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    if (button)
    {
        QString pageName = button->text();
        navigateToPage(pageName);
    }
}

/**
 * @brief Navige à la page spécifiée
 */
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

/**
 * @brief Affiche les résultats du scan
 */
void MainWindow::showScanResults(const QString& target, const QString& output, bool success) {
    ScanResultPage *resultPage = qobject_cast<ScanResultPage*>(m_pages.value("ScanResults"));
    if (resultPage) {
        resultPage->setResults(target, output, success);
        m_stackedWidget->setCurrentWidget(resultPage);
    }
}