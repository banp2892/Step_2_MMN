import sys
from PyQt6.QtWidgets import (
    QApplication, QMainWindow, QWidget, QVBoxLayout,
    QHBoxLayout, QLabel, QPushButton, QLineEdit, QGroupBox
)
from PyQt6 import QtWidgets, uic
import subprocess

def start_exe(exe_name, parameters):
    subprocess.run([exe_name] + parameters)


class MyWindow(QtWidgets.QDialog):
    def __init__(self):
        super(MyWindow, self).__init__()
        uic.loadUi('design.ui', self)

        self.read_parametrs()


        self.zadacha.currentIndexChanged.connect(self.update_info)
        self.pushButto_start_calculate.clicked.connect(self.start_calculation)

        self.update_info()
        self.set_text_to_postanovka()
        self.validator()
        self.label_spravka_fill()

    def start_calculation(self):
        self.read_parametrs()

        zadacha_type = self.zadacha.currentText().strip()
        base_bounds = ["0.0", "3.0", "0.0", "1.0"]

        if zadacha_type == 'Тестовая':
            params = base_bounds + [
                self.n, self.m, self.e_max_1, self.n_max_1, "0"
            ]
        else:
            params = base_bounds + [
                self.n, self.m, self.e_max_1, self.n_max_1, "1",
                self.e_max_2, self.n_max_2
            ]
        start_exe("Ступень_2_ММН.exe", params)

    def read_parametrs(self):
        self.n = self.lineEdit_n.text()
        self.m = self.lineEdit_m.text()
        self.e_max_1 = self.lineEdit_e_max_1.text()
        self.n_max_1 = self.lineEdit_n_max_1.text()

        self.e_max_2 = self.lineEdit_e_max_2.text()
        self.n_max_2 = self.lineEdit_n_max_2.text()

    def set_text_to_postanovka(self):
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
    def validator(self):
        from PyQt6.QtGui import QIntValidator, QDoubleValidator
        from PyQt6.QtCore import QLocale

        # Валидатор для целых положительных чисел (n, m, n_max)
        int_validator = QIntValidator(2, 1000000, self)
        self.lineEdit_n.setValidator(int_validator)
        self.lineEdit_m.setValidator(int_validator)
        self.lineEdit_n_max_1.setValidator(int_validator)

        # Валидатор для точности (E_max)
        # Разрешаем ввод в научном формате (1e-17)
        eps_validator = QDoubleValidator(0.0, 1.0, 18, self)
        eps_validator.setNotation(QDoubleValidator.Notation.ScientificNotation)
        # Устанавливаем английскую локаль, чтобы разделителем всегда была точка, а не запятая
        eps_validator.setLocale(QLocale(QLocale.Language.English, QLocale.Country.UnitedStates))

        self.lineEdit_e_max_1.setValidator(eps_validator)

    def label_spravka_fill(self):
        # Данные для вставки (замени на реальные переменные из твоего расчета)
        zadacha_type = self.zadacha.currentText().strip()
        if zadacha_type=="Тестовая":


            text = (
                "<b>СПРАВКА ПО РЕШЕНИЮ ТЕСТОВОЙ ЗАДАЧИ</b><br>"
                "------------------------------------------------------------------<br>"
                f"<b>Параметры сетки:</b> n = {self.n}, m = {self.m}<br>"
                "<b>Метод:</b> Минимальных невязок (ММН)<br>"
                f"<b>Критерии остановки:</b> ε<sub>мет</sub> = {self.e_max_1}, N<sub>max</sub> = {self.n_max_1}<br>"
                "------------------------------------------------------------------<br>"
                f"На решение СЛАУ затрачено итераций: <b>N = {n_fact}</b><br>"
                f"Достигнутая точность метода: <b>ε(N) = {eps_fact}</b><br>"
                f"Невязка СЛАУ: <b>||R<sup>(N)</sup>||<sub>max</sub> = {residual}</b> (норма 'max')<br>"
                "------------------------------------------------------------------<br>"
                f"Контрольная погрешность: ε ≤ {err_test}<br>"
                f"Фактическая погрешность: <b>ε<sub>1</sub> = {err_fact}</b><br>"
                f"Макс. отклонение в узле: <b>(x = {x_max}; y = {y_max})</b><br>"
                "------------------------------------------------------------------<br>"
                "<b>Начальное приближение:</b> Нулевое (или интерполяция)<br>"
                f"Начальная невязка: <b>||R<sup>(0)</sup>||<sub>max</sub> = {r_0}</b>"
            )

        self.label_spravka.setWordWrap(True)
        self.label_spravka.setText(text)

if __name__ == "__main__":
    app = QtWidgets.QApplication(sys.argv)
    window = MyWindow()
    window.show()

    # Код после exec() выполнится только после ЗАКРЫТИЯ окна
    sys.exit(app.exec())