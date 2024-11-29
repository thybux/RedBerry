from PySide6.QtWidgets import QMainWindow, QWidget, QToolButton, QVBoxLayout, QHBoxLayout, QStackedWidget, QPushButton
from PySide6.QtCore import Qt, QSize
from PySide6.QtGui import QFont, QIcon
from .pages.settings import Settings
from .pages.pentest import Pentest
from .pages.history import History
from functools import partial
from pathlib import Path
import os

class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("RedBerry")
        self.setGeometry(300, 300, 800, 600)

        current_dir = Path(__file__).parent.parent.parent
        self.assets_path = current_dir / 'assets'
        
        background_image = str(self.assets_path / 'background_1.png')
        self.setStyleSheet(f"""
            QMainWindow {{
                background-image: url({background_image});
                background-position: center;
                background-repeat: no-repeat;
                background-color: #1a1a1a;
            }}
            QWidget#centralWidget {{
                background: transparent;
            }}
        """)
        
        self.central_widget = QWidget()
        self.central_widget.setObjectName("centralWidget")
        self.setCentralWidget(self.central_widget)
        
        self.layout = QVBoxLayout(self.central_widget)
        
        self.stack = QStackedWidget()
        self.stack.setStyleSheet("""
            QStackedWidget {
                background: transparent;
            }
        """)
        self.pages = {
            'Pentest': {'widget': Pentest(), 'index': 0},
            'Settings': {'widget': Settings(), 'index': 1},
            'History': {'widget': History(), 'index': 2}
        }
        for page_name, page_info in self.pages.items():
            self.stack.addWidget(page_info['widget'])
        self.create_menu()
        self.layout.addWidget(self.stack)

    def navigate_to(self, page_name):
        """
        Fonction de navigation vers une page spécifique
        """
        if page_name in self.pages:
            self.stack.setCurrentIndex(self.pages[page_name]['index'])

    def create_menu(self):
        menu_widget = QWidget()
        menu_layout = QHBoxLayout(menu_widget)
        menu_layout.setSpacing(50)
        menu_layout.setAlignment(Qt.AlignCenter)
        
        button_configs = [
            {'name': 'Pentest', 'text': 'Pentest', 'icon': 'Pentest.png'},
            {'name': 'Settings', 'text': 'Settings', 'icon': 'Settings.png'},
            {'name': 'History', 'text': 'History', 'icon': 'History.png'}
        ]   
        
        for config in button_configs:
            btn = QToolButton()
            icon_path = self.assets_path / config['icon']
            if icon_path.exists():
                btn.setIcon(QIcon(str(icon_path)))
                btn.setIconSize(QSize(80, 80))
            else:
                print(f"Image not found: {icon_path}")
            
            btn.setStyleSheet("""
                QToolButton {
                    color: white;
                    border: none;
                    border-radius: 10px;
                    font-size: 16px;
                    font-weight: bold;
                    margin: 10px;
                    min-width: 120px;
                    min-height: 120px;
                    text-align: center;
                    padding-top: 20px;
                }
                QToolButton:hover {
                    background-color: rgba(52, 73, 94, 0.7);
                }
                QToolButton:pressed {
                    background-color: rgba(44, 62, 80, 0.7);
                }
            """)
            btn.setFont(QFont("Arial", 14))
            btn.setText(config['text'])
            btn.setToolButtonStyle(Qt.ToolButtonTextUnderIcon)
            btn.setStyleSheet(btn.styleSheet() + """
                QToolButton {
                    qproperty-toolButtonStyle: ToolButtonTextUnderIcon;
                    qproperty-iconSize: 80px;
                    padding-bottom: 15px;
                }
            """)
            btn.clicked.connect(partial(self.navigate_to, config['name']))
            menu_layout.addWidget(btn)
        
        self.stack.addWidget(menu_widget)
        self.stack.setCurrentWidget(menu_widget)
        
        for page_info in self.pages.values():
            back_btn = QPushButton("Retour au menu")
            back_btn.setStyleSheet("""
                QPushButton {
                    background-color: #3498db;
                    color: white;
                    border: none;
                    padding: 20px 40px;
                    font-size: 14px;
                    font-weight: bold;
                    border-radius: 10px;
                }
                QPushButton:hover {
                    background-color: #2980b9;
                }
            """)
            back_btn.clicked.connect(lambda _: self.stack.setCurrentWidget(menu_widget))
            if hasattr(page_info['widget'], 'layout'):
                page_info['widget'].layout().addWidget(back_btn)
