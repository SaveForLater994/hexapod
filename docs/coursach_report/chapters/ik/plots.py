import numpy as np
import matplotlib.pyplot as plt

x = np.linspace(0, 10, 100)
y1 = np.sin(x)  # Первая линия (синусоида)
y2 = np.cos(x)  # Вторая линия (косинусоида)

# Создаем фигуру и оси
plt.figure(figsize=(8, 4)) # Размер окна графика в дюймах

# Строим две линии
plt.plot(x, y1, label='Синус', color='blue', linewidth=2)
plt.plot(x, y2, label='Косинус', color='red', linestyle='--') # Штриховая линия

# Добавляем оформление
plt.title('Тестовый график с двумя линиями')
plt.xlabel('Ось X (время)')
plt.ylabel('Ось Y (значение)')
plt.grid(True) # Включаем сетку
plt.legend()   # Показываем легенду (лабелы линий)

plt.savefig('media/test.svg', format='svg') # Сохраняем график в формате SVG
