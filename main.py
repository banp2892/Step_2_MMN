import sys
from PyQt6.QtWidgets import QApplication, QMainWindow, QWidget, QVBoxLayout, QLabel, QPushButton, QLineEdit


class MyWindow(QMainWindow):
    def __init__(self):
        super().__init__()

        # 1. Настройка самого окна
        self.setWindowTitle("Вариант 8. Шаг 2. ММН. Черных Севастьян Владимирович")
        self.resize(400, 500)

        # 2. Создаем "центральный виджет" — это пустой холст внутри окна
        self.central_widget = QWidget()
        self.setCentralWidget(self.central_widget)

        # 3. Создаем "Layout" (слой) — он отвечает за то, как элементы будут стоять в ряд или в столбик
        self.layout = QVBoxLayout(self.central_widget)  # V — Vertical (столбик)

        # 4. Добавляем элементы
        self.add_elements()

    def add_elements(self):
        # Здесь мы будем создавать кнопки и надписи
        pass


if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = MyWindow()
    window.show()
    sys.exit(app.exec())