import sys
from PyQt6.QtWidgets import (
    QApplication, QMainWindow, QWidget, QVBoxLayout,
    QHBoxLayout, QLabel, QPushButton, QLineEdit, QGroupBox
)
from PyQt6 import QtWidgets, uic
import subprocess

def start_calculation(exe_name, parameters):
    subprocess.run([exe_name] + parameters)


class MyWindow(QtWidgets.QDialog):
    def __init__(self):
        super(MyWindow, self).__init__()
        uic.loadUi('design.ui', self)


        self.zadacha.currentIndexChanged.connect(self.update_info)


        self.update_info()

        self.label_button_class_of_task.setText("Выбрать решаемую задачу")
        static_text = (
            "Ступень 2. Метод минимальных невязок. Вариант 8. <br>"
            "Выполнил: Черных Севастьян Владимирович<br>"
            "------------------------------------------------------------------<br>"
            "<b>Уравнение:</b><br>"
            "Δ u(x, y) = – f(x, y), &nbsp; x ∈ [a, b], y ∈ [c, d]<br><br>"
            "<b>Границы области:</b><br>"
            "a = 0.0, &nbsp; b = 3.0, &nbsp; "
            "c = 0.0, &nbsp; d = 1.0"
        )


        self.label_postanoka_zadachi.setWordWrap(True)
        self.label_postanoka_zadachi.setText(static_text)


    def update_info(self):
        # Получаем выбранную задачу
        zadacha_type = self.zadacha.currentText().strip()

        if zadacha_type == "Тестовая":
            text = (
                "<b>Тестовая задача:</b><br>"
                "<i>f(x, y)</i> = -2·[(<i>y</i><sup>4</sup> + 4<i>x</i><sup>2</sup><i>y</i><sup>2</sup>)·cos(2<i>xy</i><sup>2</sup>) + <i>x</i>·sin(2<i>xy</i><sup>2</sup>)]<br>"
                "<i>u</i>(<i>a</i>, <i>y</i>) = 0<br>"
                "<i>u</i>(<i>b</i>, <i>y</i>) = sin<sup>2</sup>(3<i>y</i><sup>2</sup>)<br>"
                "<i>u</i>(<i>x</i>, <i>c</i>) = 0<br>"
                "<i>u</i>(<i>x</i>, <i>d</i>) = sin<sup>2</sup>(<i>x</i>)"
            )
        else:
            text = (
                "<b>Основная задача:</b><br>"
                "<i>f(x, y)</i> = cosh(<i>x - y</i>)<br>"
                "<i>u</i>(<i>a</i>, <i>y</i>) = sin<sup>2</sup>(π<i>y</i>)<br>"
                "<i>u</i>(<i>b</i>, <i>y</i>) = 0<br>"
                "<i>u</i>(<i>x</i>, <i>c</i>) = cosh(<i>x</i><sup>2</sup> - 3<i>x</i>) - 1<br>"
                "<i>u</i>(<i>x</i>, <i>d</i>) = 0"
            )


        self.label_zadacha.setWordWrap(True)
        self.label_zadacha.setText(text)


if __name__ == "__main__":
    app = QtWidgets.QApplication(sys.argv)
    window = MyWindow()
    window.show()

    # Код после exec() выполнится только после ЗАКРЫТИЯ окна
    sys.exit(app.exec())