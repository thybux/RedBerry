from PySide6.QtWidgets import QMainWindow

class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Simple Window")  # Titre de la fenêtre
        self.setGeometry(300, 300, 800, 600)  # Position et taille (x, y, largeur, hauteur)
