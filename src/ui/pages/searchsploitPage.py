from PySide6.QtWidgets import (
    QWidget,
    QVBoxLayout,
    QHBoxLayout,
    QCheckBox,
    QPushButton,
    QTextEdit,
    QScrollArea,
    QWidget,
    QLabel
)
from PySide6.QtCore import Qt
from PySide6.QtGui import QFont

class SearchsploitPage(QWidget):
    def __init__(self, services=None, parent=None):
        super().__init__(parent)
        self.services = services or {}
        self.checkboxes = {}
        self.services_layout = None
        self.results_output = None
        self.init_ui()

    def init_ui(self):
        main_vertical_layout = QVBoxLayout()
        
        content_layout = QHBoxLayout()
        
        left_widget = QWidget()
        left_layout = QVBoxLayout()
        
        services_label = QLabel("Services Actifs:")
        services_label.setStyleSheet("font-weight: bold; font-size: 14px;")
        left_layout.addWidget(services_label)

        scroll = QScrollArea()
        scroll.setWidgetResizable(True)
        scroll_content = QWidget()
        self.services_layout = QVBoxLayout(scroll_content)
        
        self.update_services()

        select_buttons_layout = QHBoxLayout()
        select_all_btn = QPushButton("Tout sélectionner")
        deselect_all_btn = QPushButton("Tout désélectionner")
        select_all_btn.clicked.connect(self.select_all)
        deselect_all_btn.clicked.connect(self.deselect_all)
        select_buttons_layout.addWidget(select_all_btn)
        select_buttons_layout.addWidget(deselect_all_btn)
        
        search_btn = QPushButton("Lancer Searchsploit")
        search_btn.clicked.connect(self.run_searchsploit)
        
        scroll_content.setLayout(self.services_layout)
        scroll.setWidget(scroll_content)
        left_layout.addWidget(scroll)
        left_layout.addLayout(select_buttons_layout)
        left_layout.addWidget(search_btn)
        left_widget.setLayout(left_layout)
        
        left_widget.setMaximumWidth(400)
        
        self.results_output = QTextEdit()
        self.results_output.setReadOnly(True)
        self.results_output.setFont(QFont("Courier"))
        
        content_layout.addWidget(left_widget)
        content_layout.addWidget(self.results_output)
        
        main_vertical_layout.addLayout(content_layout)
        
        self.setLayout(main_vertical_layout)

    def update_services(self):
        for checkbox in self.checkboxes.values():
            checkbox.deleteLater()
        self.checkboxes.clear()

        while self.services_layout and self.services_layout.count():
            item = self.services_layout.takeAt(0)
            if item.widget():
                item.widget().deleteLater()

        for port, service_info in self.services.items():
            service_name = service_info.get('name', 'unknown')
            product = service_info.get('product', '')
            version = service_info.get('version', '')
            
            service_text = f"Port {port}: {service_name}"
            if product:
                service_text += f" - {product}"
            if version:
                service_text += f" ({version})"

            checkbox = QCheckBox(service_text)
            self.checkboxes[port] = checkbox
            self.services_layout.addWidget(checkbox)

    def set_services(self, new_services):
        """Méthode pour mettre à jour les services"""
        self.services = new_services
        self.update_services()

    def select_all(self):
        for checkbox in self.checkboxes.values():
            checkbox.setChecked(True)

    def deselect_all(self):
        for checkbox in self.checkboxes.values():
            checkbox.setChecked(False)

    def run_searchsploit(self):
        selected_services = []
        for port, checkbox in self.checkboxes.items():
            if checkbox.isChecked():
                service_info = self.services[port]
                service_str = f"{service_info.get('name', '')} {service_info.get('product', '')} {service_info.get('version', '')}".strip()
                selected_services.append(service_str)

        if not selected_services:
            self.results_output.setText("Veuillez sélectionner au moins un service.")
            return

        # TODO: Implémenter l'appel à searchsploit
        # Pour l'instant, affiche juste les services sélectionnés
        self.results_output.setText("Services sélectionnés pour searchsploit:\n\n" + "\n".join(selected_services))
