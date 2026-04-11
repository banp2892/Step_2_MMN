import sys

from PyQt6.QtCore import QObject, pyqtSignal, QThread
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
        try:
            print("--- Запуск процесса ---")
            self.read_parametrs()

            # Блокируем кнопку, чтобы не нажать дважды
            self.pushButto_start_calculate.setEnabled(False)

            zadacha_type = self.zadacha.currentText().strip()
            base_bounds = ["0.0", "3.0", "0.0", "1.0"]

            if zadacha_type == 'Тестовая':
                params = base_bounds + [self.n, self.m, self.e_max_1, self.n_max_1, "0"]
            else:
                params = base_bounds + [self.n, self.m, self.e_max_1, self.n_max_1, "1", self.e_max_2, self.n_max_2]

            cmd = ["calculate.exe"] + params
            print(f"Команда: {cmd}")

            self.thread = QThread()
            self.worker = Worker(cmd)
            self.worker.moveToThread(self.thread)

            # Сигналы
            self.thread.started.connect(self.worker.run)
            self.worker.text_for_console.connect(self.update_console_label)

            # Важно: Сначала включаем кнопку обратно, потом закрываем поток
            self.worker.finished.connect(lambda: self.pushButto_start_calculate.setEnabled(True))
            self.worker.finished.connect(self.thread.quit)
            self.worker.finished.connect(self.worker.deleteLater)
            self.thread.finished.connect(self.thread.deleteLater)
            self.thread.finished.connect(self.read_results)

            print("Поток подготовлен, запускаю...")
            self.thread.start()

        except Exception as e:
            print(f"Ошибка при инициализации потока: {e}")
            self.pushButto_start_calculate.setEnabled(True)


    def read_parametrs(self):
        self.n = self.lineEdit_n.text()
        self.m = self.lineEdit_m.text()
        self.e_max_1 = self.lineEdit_e_max_1.text()
        self.n_max_1 = self.lineEdit_n_max_1.text()

        self.e_max_2 = self.lineEdit_e_max_2.text()
        self.n_max_2 = self.lineEdit_n_max_2.text()

    def read_results(self):
        try:
            with open("stats.txt", "r") as f:
                lines = f.readlines()
                if not lines:
                    return

                last_line = lines[-1].strip()
                data = last_line.split()

                # --- Общие параметры (0-6) ---
                self.res_task_type = int(data[0])
                self.res_n = data[1]
                self.res_m = data[2]
                self.res_iter = data[3]  # N1
                self.res_eps_n = data[4]  # eps1
                self.res_r_n = data[5]  # R1
                self.res_r_0 = data[6]  # R0_1

                if self.res_task_type == 0:
                    # ТЕСТОВАЯ ЗАДАЧА
                    self.res_error = data[7]
                    self.res_x_max = data[8]
                    self.res_y_max = data[9]
                    self.res_time_1 = data[10]
                else:
                    # ОСНОВНАЯ ЗАДАЧА
                    self.res_iter_2 = data[7]  # N2
                    self.res_eps_n_2 = data[8]  # eps2
                    self.res_r_n_2 = data[9]  # R2
                    self.res_r_0_2 = data[10]  # R0_2
                    self.res_error = data[11]  # Оценка по половинному шагу
                    self.res_x_max = data[12]
                    self.res_y_max = data[13]
                    self.res_time_1 = data[14]
                    self.res_time_2 = data[15]

            # Обновляем справку
            self.label_spravka_fill()

        except FileNotFoundError:
            print("stats.txt еще не создан")
        except Exception as e:
            print(f"Ошибка парсинга: {e}")

    def update_console_label(self, text):
        self.label_console.setText("Вычисление: " + str(text))

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
        # Проверяем, есть ли у нас результаты (были ли они созданы в read_results)
        if not hasattr(self, 'res_iter'):
            self.label_spravka.setText("Ожидание результатов расчета...")
            return

        zadacha_type = self.zadacha.currentText().strip()

        if zadacha_type == "Тестовая":
            # Константа из условия
            err_test_limit = "0.5·10⁻⁶"

            text = (
                "<b>СПРАВКА ПО РЕШЕНИЮ ТЕСТОВОЙ ЗАДАЧИ</b><br>"
                "------------------------------------------------------------------<br>"
                f"<b>Сетка:</b> n = {self.res_n}, m = {self.res_m}<br>"
                f"<b>Критерии:</b> ε<sub>мет</sub> = {self.e_max_1}, N<sub>max</sub> = {self.n_max_1}<br>"
                "------------------------------------------------------------------<br>"
                f"Итераций затрачено: <b>N = {self.res_iter}</b><br>"
                f"Время расчета: <b>t = {float(self.res_time_1):.4f} сек.</b><br>"
                f"Точность метода: <b>ε(N) = {self.res_eps_n}</b><br>"
                f"Невязка СЛАУ: <b>||R<sup>(N)</sup>|| = {self.res_r_n}</b><br>"
                "------------------------------------------------------------------<br>"
                f"Контрольная погрешность: ε ≤ {err_test_limit}<br>"
                f"<b>Фактическая погрешность: ε<sub>1</sub> = {self.res_error}</b><br>"
                f"Макс. отклонение в узле: <b>(x={self.res_x_max}; y={self.res_y_max})</b><br>"
                "------------------------------------------------------------------<br>"
                f"Начальная невязка: ||R<sup>(0)</sup>|| = {self.res_r_0}"
            )
        else:
        # Для основной задачи считаем суммарное время
            try:
                total_t = float(self.res_time_1) + float(self.res_time_2)
                t1 = float(self.res_time_1)
                t2 = float(self.res_time_2)
            except:
                total_t = t1 = t2 = 0.0

            text = (
                "<b>СПРАВКА ПО РЕШЕНИЮ ОСНОВНОЙ ЗАДАЧИ</b><br>"
                "------------------------------------------------------------------<br>"
                f"<b>Сетка 1:</b> {self.res_n}x{self.res_m} | <b>Сетка 2:</b> {int(self.res_n) * 2}x{int(self.res_m) * 2}<br>"
                "------------------------------------------------------------------<br>"
                f"Итераций (N1/N2): <b>{self.res_iter} / {self.res_iter_2}</b><br>"
                f"Время (t1/t2): <b>{t1:.4f} / {t2:.4f} сек.</b><br>"
                f"<b>Общее время: {total_t:.4f} сек.</b><br>"
                "------------------------------------------------------------------<br>"
                f"Невязка R1 (N): {self.res_r_n}<br>"
                f"Невязка R2 (N): {self.res_r_n_2}<br>"
                "------------------------------------------------------------------<br>"
                f"<b>Оценка точности (пол. шаг): ε = {self.res_error}</b><br>"
                f"Макс. разность в узле: <b>(x={self.res_x_max}; y={self.res_y_max})</b><br>"
                "------------------------------------------------------------------<br>"
                f"Начальная невязка R1(0): {self.res_r_0}"
            )

        self.label_spravka.setWordWrap(True)
        self.label_spravka.setText(text)


class Worker(QObject):
    text_for_console = pyqtSignal(str)
    finished = pyqtSignal()

    def __init__(self, command_args):
        super().__init__()
        self.command_args = command_args

    def run(self):
        try:
            print(f"Worker начал работу с: {self.command_args}")
            process = subprocess.Popen(
                self.command_args,
                stdout=subprocess.PIPE,
                stderr=subprocess.STDOUT,
                text=True,
                encoding="cp1251",  # Пробуем Windows-кодировку
                shell=True
            )

            # Проверка, запустился ли процесс
            if process.stdout:
                for line in process.stdout:
                    clean_line = line.strip()
                    if clean_line:
                        #print(f"Консоль EXE: {clean_line}")
                        self.text_for_console.emit(clean_line)

            process.wait()
            #print(f"Процесс завершен с кодом: {process.returncode}")

        except Exception as e:
            print(f"Критическая ошибка в Worker: {e}")
            self.text_for_console.emit(f"Ошибка запуска: {e}")
        finally:
            self.finished.emit()



if __name__ == "__main__":
    app = QtWidgets.QApplication(sys.argv)
    window = MyWindow()
    window.show()

    # Код после exec() выполнится только после ЗАКРЫТИЯ окна
    sys.exit(app.exec())