import sys
from PySide6.QtWidgets import QApplication
from ui.main_window import MainWindow

if __name__ == "__main__":
    app = QApplication(sys.argv)  # Création de l'application
    window = MainWindow()      # Instanciation de la fenêtre principale
    window.show()                 # Affichage de la fenêtre
    sys.exit(app.exec())          # Boucle d'exécution de l'application
