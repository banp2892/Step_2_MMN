import sys
from PyQt6.QtWidgets import (
    QApplication, QMainWindow, QWidget, QVBoxLayout,
    QHBoxLayout, QLabel, QPushButton, QLineEdit, QGroupBox
)
import subprocess

def start_calculation(exe_name, parameters):
    subprocess.run([exe_name] + parameters)














class SolverApp(QMainWindow):
    def __init__(self):
        super().__init__()

        self.setWindowTitle("ММН")
        self.resize(800, 600)

        # создаем главное окно подложки???
        central_widget = QWidget()
        self.setCentralWidget(central_widget)
        self.setup_ui(central_widget)


    def setup_ui(self):
        params_layout = QGridLayout()
        params_layout.addWidget(QLabel("Число разбиений по x:"), 0, 0)
        params_layout.addWidget(self.n, 0, 1)
        pass


















if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = SolverApp()
    window.show()
    sys.exit(app.exec())

test_params = ["0", "3", "0", "1", "200", "200", "1e-7", "500000", "1", "1e-7", "500000"]
test_exe = "Ступень_2_ММН.exe"

# Вызов функции
start_calculation(test_exe, test_params)