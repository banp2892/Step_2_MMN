import sys

import pyvista as pv
from pyvistaqt import QtInteractor
from PyQt6.QtCore import QObject, pyqtSignal, QThread
from PyQt6.QtWidgets import (
    QApplication, QMainWindow, QWidget, QVBoxLayout,
    QHBoxLayout, QLabel, QPushButton, QLineEdit, QGroupBox, QTableWidget, QAbstractItemView, QTableWidgetItem
)
from PyQt6 import QtWidgets, uic
import subprocess
import numpy as np

def start_exe(exe_name, parameters):
    subprocess.run([exe_name] + parameters)
def read_binary(filename,n,m):
    return np.fromfile(filename,dtype=np.float64)


class MyWindow(QtWidgets.QDialog):
    def __init__(self):
        super(MyWindow, self).__init__()
        uic.loadUi('design.ui', self)

        self.opened_plots = []

        self.read_parametrs()


        self.zadacha.currentIndexChanged.connect(self.update_info)
        self.pushButto_start_calculate.clicked.connect(self.start_calculation)
        self.pushButto_table_v.clicked.connect(self.view_v_table)
        self.pushButto_table_u_or_v2.clicked.connect(self.view_v2_table)
        self.pushButto_table_raznost.clicked.connect(self.view_diff_table)

        self.pushButto_graphic_v.clicked.connect(lambda: self.show_graphic('v'))
        self.pushButto_graphic_u_or_v2.clicked.connect(lambda: self.show_graphic('u_v2'))
        self.pushButto_graphic_raznost.clicked.connect(lambda: self.show_graphic('diff'))
        self.pushButto_graphic_v_0.clicked.connect(lambda: self.show_graphic('v_0'))
        self.pushButto_graphic_v2_0.clicked.connect(lambda: self.show_graphic('v2_0'))


        self.update_info()
        self.set_text_to_postanovka()
        self.validator()
        self.label_spravka_fill()



    #table1 = 'v_main_n.bin'
    #table2 = 'v_main_2n_sub.bin'
    #table3 = 'v_main_diff.bin'



    def view_v_table(self):
        n, m = int(self.n), int(self.m)
        zadacha_type = self.zadacha.currentText().strip()

        if zadacha_type == 'Тестовая':
            filename = 'v_test_numeric.bin'  # Исправлено!
            title = "Таблица vN(xi,yj) (Тест)"
        else:
            filename = 'v_main_n.bin'
            title = "Таблица v1(N)(xi,yj) (Основная)"
        data = read_binary(filename, n, m)
        self.v_win = TableView(data, n, m, title)
        self.v_win.show()

    def view_v2_table(self):
        """Отображает u_exact (тест) или v(2N) (основная)"""
        n, m = int(self.n), int(self.m)
        zadacha_type = self.zadacha.currentText().strip()

        if zadacha_type == 'Тестовая':
            filename = 'u_test_exact.bin'
            title = "Таблица u(N)(xi,yj)"
        else:
            filename = 'v_main_2n_sub.bin'
            title = "Таблица v2(N2)(x2i,y2j)"

        data = read_binary(filename, n, m)
        self.v_win = TableView(data, n, m, title)
        self.v_win.show()

    def view_diff_table(self):
        """Отображает разность (v - u) или (v - v2)"""
        n, m = int(self.n), int(self.m)
        zadacha_type = self.zadacha.currentText().strip()

        if zadacha_type == 'Тестовая':
            filename = 'uv_test_diff.bin'
            title = "Таблица v(N)(xi,yj) - u(xi,yj)"
        else:
            filename = 'v_main_diff.bin'
            title = "Таблица v(N)(xi,yj) - v(2N)(xi,yj)"

        data = read_binary(filename, n, m)
        self.v_win = TableView(data, n, m, title)
        self.v_win.show()

    def start_calculation(self):
        try:
            print("--- Запуск процесса ---")
            self.read_parametrs()


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


            self.thread.started.connect(self.worker.run)
            self.worker.text_for_console.connect(self.update_console_label)

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

    def rename_button_table(self):
        zadacha_type = self.zadacha.currentText().strip()
        if zadacha_type=="Тестовая":
            self.pushButto_table_v.setText("Таблица v(N)(xi,yj)")
            self.pushButto_table_u_or_v2.setText("Таблица u(N)(xi,yj)")
            self.pushButto_table_raznost.setText("Таблица v(N)(xi,yj) - u(N)(xi,yj)")
        else:
            self.pushButto_table_v.setText("Таблица v(N)(xi,yj)")
            self.pushButto_table_u_or_v2.setText("Таблица v2(N2)(x2i,y2j)")
            self.pushButto_table_raznost.setText("Таблица v(N)(xi,yj) - v(2N)(x2i,y2j)")

    def rename_button_graphic(self):
        zadacha_type = self.zadacha.currentText().strip()
        if zadacha_type=="Тестовая":
            self.pushButto_graphic_v.setText("График v(N)(xi,yj)")
            self.pushButto_graphic_u_or_v2.setText("График u(N)(xi,yj)")
            self.pushButto_graphic_raznost.setText("График v(N)(xi,yj) - u(N)(xi,yj)")
        else:
            self.pushButto_graphic_v.setText("График v(N)(xi,yj)")
            self.pushButto_graphic_u_or_v2.setText("График v2(N2)(x2i,y2j)")
            self.pushButto_graphic_raznost.setText("График v(N)(xi,yj) - v(2N)(x2i,y2j)")

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

                self.pushButto_table_v.setEnabled(True)
                self.pushButto_table_u_or_v2.setEnabled(True)
                self.pushButto_table_raznost.setEnabled(True)

                self.pushButto_graphic_raznost.setEnabled(True)
                self.pushButto_graphic_u_or_v2.setEnabled(True)
                self.pushButto_graphic_v.setEnabled(True)
                self.pushButto_graphic_v_0.setEnabled(True)



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
            is_main_task = (self.res_task_type != 0)
            self.pushButto_graphic_v2_0.setEnabled(is_main_task)

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
        self.rename_button_table()
        self.rename_button_graphic()

        self.pushButto_table_v.setEnabled(False)
        self.pushButto_table_u_or_v2.setEnabled(False)
        self.pushButto_table_raznost.setEnabled(False)
        self.pushButto_graphic_raznost.setEnabled(False)
        self.pushButto_graphic_u_or_v2.setEnabled(False)
        self.pushButto_graphic_v.setEnabled(False)
        self.pushButto_graphic_v2_0.setEnabled(False)
        self.pushButto_graphic_v_0.setEnabled(False)

    def show_graphic(self, mode):
        try:
            n, m = int(self.n), int(self.m)
            zadacha_type = self.zadacha.currentText().strip()
            is_test = (zadacha_type == 'Тестовая')

            # По умолчанию размер (n+1)x(m+1)
            grid_n, grid_m = n + 1, m + 1

            if mode == 'v':
                filename = 'v_test_numeric.bin' if is_test else 'v_main_n.bin'
                title = "График v(N)"
            elif mode == 'u_v2':
                filename = 'u_test_exact.bin' if is_test else 'v_main_2n_sub.bin'
                title = "График u(N)" if is_test else "График v(2N)"
            elif mode == 'diff':
                filename = 'uv_test_diff.bin' if is_test else 'v_main_diff.bin'
                title = "График разности"
            elif mode == 'v_0':
                filename = 'v0_test_numeric.bin' if is_test else 'v1_0_main_numeric.bin'
                title = "График v(0)(xi,yj)"
            elif mode == 'v2_0':
                filename = 'v2_0_main_numeric.bin'
                title = "График v2(0)(x2i,y2j)"
                # Если в С++ ты сделал (n+1)x(m+1) для v2_0, оставляй по умолчанию.
                # Если там осталось n*m, то раскомментируй строку ниже:
                # grid_n, grid_m = n, m

            data = read_binary(filename, n, m)

            if data.size == 0:
                print(f"Файл {filename} пуст!")
                return

            # Делаем reshape с учетом выбранных размеров
            grid = data.reshape((grid_m, grid_n))

            # Создаем новое окно и сохраняем его в список
            new_plot = SurfaceWindow(grid, title, 0.0, 3.0, 0.0, 1.0, title)
            self.opened_plots.append(new_plot)  # Не дает окну закрыться
            new_plot.show()

            # Чистим список от закрытых окон (чтобы не ели память)
            self.opened_plots = [w for w in self.opened_plots if w.isVisible()]

        except Exception as e:
            print(f"Ошибка при отрисовке: {e}")

    def validator(self):
        from PyQt6.QtGui import QIntValidator, QDoubleValidator
        from PyQt6.QtCore import QLocale

        # Валидатор для целых положительных чисел (n, m, n_max)
        int_validator = QIntValidator(2, 1000000, self)
        self.lineEdit_n.setValidator(int_validator)
        self.lineEdit_m.setValidator(int_validator)
        self.lineEdit_n_max_1.setValidator(int_validator)


        eps_validator = QDoubleValidator(0.0, 1.0, 18, self)
        eps_validator.setNotation(QDoubleValidator.Notation.ScientificNotation)

        eps_validator.setLocale(QLocale(QLocale.Language.English, QLocale.Country.UnitedStates))

        self.lineEdit_e_max_1.setValidator(eps_validator)

    def label_spravka_fill(self):
        if not hasattr(self, 'res_iter'):
            self.label_spravka.setText("Ожидание результатов расчета...")
            return

        zadacha_type = self.zadacha.currentText().strip()

        if zadacha_type == "Тестовая":
            # Лимит погрешности из методички
            err_test_limit = "0.5·10⁻⁶"

            try:
                t1 = float(self.res_time_1)
            except:
                t1 = 0.0

            text = (
                f"Метод: <b>Минимальных невязок (ММН)</b>. <br>"
                "<b>СПРАВКА ПО РЕШЕНИЮ ТЕСТОВОЙ ЗАДАЧИ</b><br>"
                "------------------------------------------------------------------<br>"
                f"Использована сетка с числом разбиений: <b>n = {self.res_n}</b>, <b>m = {self.res_m}</b>. <br>"
                f"Критерии остановки: <b>ε<sub>мет</sub> = {self.e_max_1}</b>, <b>N<sub>max</sub> = {self.n_max_1}</b>. <br>"
                f"Затрачено итераций: <b>N = {self.res_iter}</b>. <br>"
                f"Достигнута точность итерационного метода: <b>ε(N) = {self.res_eps_n}</b>. <br>"
                f"Невязка СЛАУ (норма Чебышёва): <b>||R(N)|| = {self.res_r_n}</b>. <br>"
                f"Начальное приближение: <b>нулевое</b>. <br>"
                f"Начальная невязка: <b>||R(0)|| = {self.res_r_0}</b>. <br>"
                f"Время расчета: <b>{t1:.4f} сек.</b><br>"
                "------------------------------------------------------------------<br>"
                "<b>ИТОГОВЫЙ КОНТРОЛЬ ТОЧНОСТИ</b><br>"
                "------------------------------------------------------------------<br>"
                f"Требуемая точность: <b>ε = {err_test_limit}</b>. <br>"
                f"Фактическая погрешность: <b>ε1 = {self.res_error}</b>. <br>"
                f"Макс. отклонение в узле: <b>x = {self.res_x_max}</b>; <b>y = {self.res_y_max}</b>. <br>"
                f"<b>Общее время расчета: {t1:.4f} сек.</b>"
            )
        else:
            # Расчет времени
            try:
                t1 = float(self.res_time_1)
                t2 = float(self.res_time_2)
                total_time = t1 + t2
            except:
                t1 = t2 = total_time = 0.0

            text = (
                f"Метод: <b>Минимальных невязок (ММН)</b>. <br>"
                "<b>СПРАВКА ПО РЕШЕНИЮ ОСНОВНОЙ ЗАДАЧИ (ОСНОВНАЯ СЕТКА)</b><br>"
                "------------------------------------------------------------------<br>"
                f"Использована сетка с числом разбиений: <b>n = {self.res_n}</b>, <b>m = {self.res_m}</b>. <br>"
                f"Критерии остановки: <b>ε<sub>мет</sub> = {self.e_max_1}</b>, <b>N<sub>max</sub> = {self.n_max_1}</b>. <br>"
                f"Затрачено итераций: <b>N = {self.res_iter}</b>. <br>"
                f"Достигнута точность итерационного метода: <b>ε(N) = {self.res_eps_n}</b>. <br>"
                f"Невязка СЛАУ (норма Чебышёва): <b>||R(N)|| = {self.res_r_n}</b>. <br>"
                f"Начальное приближение: <b>нулевое</b>. <br>"
                f"Начальная невязка: <b>||R(0)|| = {self.res_r_0}</b>. <br>"
                f"Время расчета: <b>{t1:.4f} сек.</b><br>"
                "------------------------------------------------------------------<br>"
                "<b>СПРАВКА ПО РЕШЕНИЮ (СЕТКА С ПОЛОВИННЫМ ШАГОМ)</b><br>"
                "------------------------------------------------------------------<br>"
                
                f"Использована сетка с числом разбиений: <b>n = {int(self.res_n) * 2}</b>, <b>m = {int(self.res_m) * 2}</b>. <br>"
                f"Критерии остановки: <b>ε<sub>мет-2</sub> = {self.e_max_2}</b>, <b>N<sub>max-2</sub> = {self.n_max_2}</b>. <br>"
                f"Затрачено итераций: <b>N2 = {self.res_iter_2}</b>. <br>"
                f"Достигнута точность итерационного метода: <b>ε(N2) = {self.res_eps_n_2}</b>. <br>"
                f"Невязка СЛАУ (норма Чебышёва): <b>||R(N2)|| = {self.res_r_n_2}</b>. <br>"
                f"Начальное приближение: <b>нулевое</b>. <br>"
                f"Начальная невязка: <b>||R(0)|| = {self.res_r_0_2}</b>. <br>"
                f"Время расчета: <b>{t2:.4f} сек.</b><br>"
                "------------------------------------------------------------------<br>"
                "<b>ИТОГОВЫЙ КОНТРОЛЬ ТОЧНОСТИ</b><br>"
                "------------------------------------------------------------------<br>"
                f"Требуемая точность: <b>ε = 0.5·10⁻⁶</b>. <br>"
                f"Полученная точность (разность численных решений в общих узлах): <b>ε2 = {self.res_error}</b>. <br>"
                f"Макс. отклонение в узле: <b>x = {self.res_x_max}</b>; <b>y = {self.res_y_max}</b>. <br>"
                f"<b>Общее время расчетов: {total_time:.4f} сек.</b>"
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


class TableView(QWidget):
    def __init__(self, data, n,m,title,parent=None):
        super().__init__(parent)
        self.setWindowTitle(title)
        self.setMinimumSize(800, 600)

        layout = QVBoxLayout(self)
        layout.setContentsMargins(0, 0, 0, 0)  # Убираем рамки вокруг таблицы
        layout.setSpacing(0)


        rows = m+1
        cols = n + 1

        step_x = (0 + 3)/(n)
        step_y = (0 + 1)/(m)

        self.table = QTableWidget(rows + 1, cols + 1)



        layout.addWidget(self.table)
        for i in range(n+1):
            x_val = 0.0 + step_x * i
            self.table.setItem(0,i+1,QTableWidgetItem(f"{x_val:.3f}"))

        for j in range(m,-1,-1):
            y_val = 1.0 - step_y * (m-j)
            self.table.setItem(m-j+1,0,QTableWidgetItem(f"{y_val:.3f}"))



        self.table.setEditTriggers(QtWidgets.QAbstractItemView.EditTrigger.NoEditTriggers)

        # Углы управления


        #self.table.setItem(0, 1, QTableWidgetItem("xi"))
        self.table.setItem(0, 0, QTableWidgetItem("yj                   xi"))

        h_labels = [""] + [f"i={i}" for i in range(n + 1)]
        self.table.setHorizontalHeaderLabels(h_labels)
        h2_labels = [""] + [f"i={j}" for j in range(m ,-1,-1)]
        self.table.setVerticalHeaderLabels(h2_labels)


        # Основной цикл заполнения данных
        for row_in_table in range(m + 1):
            j_idx = m - row_in_table
            for i_idx in range(n + 1):
                idx_in_vector = j_idx * (n + 1) + i_idx

                # Берем значение из считанного файла
                value = data[idx_in_vector]
                if abs(value)<1e-17:
                    item = QTableWidgetItem(f"{0:.6g}")
                else:
                    item = QTableWidgetItem(f"{value:.6g}")

                # Куда кладем в таблицу?
                self.table.setItem(row_in_table + 1, i_idx + 1, item)

        layout.addWidget(self.table)

class SurfaceWindow(QMainWindow):

    def __init__(self, grid, title, a, b, c, d, func_name):
        super().__init__()

        self.grid = grid
        self.a = a
        self.b = b
        self.c = c
        self.d = d
        self.func_name = func_name

        self.setWindowTitle(title)
        self.resize(1200, 900)

        widget = QWidget()
        layout = QVBoxLayout()

        self.plotter = QtInteractor(widget)

        layout.addWidget(self.plotter)
        widget.setLayout(layout)

        self.setCentralWidget(widget)

        self.draw_surface(grid)

    def draw_surface(self, grid):

        self.plotter.clear()
        # downsample if grid is too big
        max_points = 200
        step = max(1, max(grid.shape)//max_points)
        g = grid[::step, ::step].astype(np.float32)

        plate_size = max(self.b - self.a, self.d - self.c)
        z_scale = plate_size / np.max(np.abs(grid))
        g_scaled = g * z_scale

        ny, nx = g.shape
        x = np.linspace(self.a, self.b, nx, dtype=np.float32)
        y = np.linspace(self.c, self.d, ny, dtype=np.float32)

        X, Y = np.meshgrid(x, y)

        surf = pv.StructuredGrid(X, Y, g_scaled)
        zmin = float(np.min(g))
        zmax = float(np.max(g))

        self.plotter.add_mesh(surf, cmap="viridis", smooth_shading=True, show_scalar_bar=True) # show_scalar_bar=False
        self.plotter.show_bounds(
            bounds=[self.a, self.b, self.c, self.d, zmin * z_scale, zmax * z_scale],
            grid='back',
            location='outer',
            xtitle='x',
            ytitle='y',
            ztitle=f"{self.func_name} * {z_scale:.2e}",
        )
        self.plotter.reset_camera()

    def closeEvent(self, event):
        try:
            self.plotter.close()
        except:
            pass
        event.accept()



if __name__ == "__main__":
    app = QtWidgets.QApplication(sys.argv)
    window = MyWindow()
    window.show()

    # Код после exec() выполнится только после ЗАКРЫТИЯ окна
    sys.exit(app.exec())