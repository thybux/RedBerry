import sys
from PySide6.QtWidgets import QApplication
from src.ui.main_window import Ui_MainWindow
from src.controllers.scanner import Scanner

class MainWindow(Ui_MainWindow):
    def __init__(self):
        super().__init__()
        self.scanner = Scanner()
        self.setupUi(self)
        self.scanButton.clicked.connect(self.start_scan)

    def start_scan(self):
        target = self.targetInput.text()
        results = self.scanner.scan(target)
        self.resultsOutput.setText(results)

if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec())
