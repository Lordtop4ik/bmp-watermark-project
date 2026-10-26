import struct
import os
import subprocess
import sys

# Принудительно включаем UTF-8 для вывода в консоль (фикс для Windows CI)
if sys.stdout.encoding != 'utf-8':
    sys.stdout.reconfigure(encoding='utf-8')

def create_bmp(filename, width, height, b, g, r):
    # Создает простой 24-битный BMP, залитый одним цветом
    row_size = (width * 3 + 3) & ~3
    padding = row_size - width * 3
    file_size = 54 + row_size * height
    
    # File Header
    file_header = struct.pack('<2sIHHI', b'BM', file_size, 0, 0, 54)
    # Info Header
    info_header = struct.pack('<IiiHHIIiiII', 40, width, height, 1, 24, 0, row_size * height, 2835, 2835, 0, 0)
    
    with open(filename, 'wb') as f:
        f.write(file_header)
        f.write(info_header)
        for _ in range(height):
            for _ in range(width):
                f.write(struct.pack('<BBB', b, g, r))
            f.write(b'\x00' * padding)

def check_bmp_pixel(filename, x, y):
    with open(filename, 'rb') as f:
        f.seek(54 + y * ((2 * 3 + 3) & ~3) + x * 3)
        b, g, r = struct.unpack('<BBB', f.read(3))
        return r, g, b

def main():
    os.makedirs('tests/tmp', exist_ok=True)
    
    # 1. Создаем тестовые изображения 5x5
    create_bmp('tests/tmp/orig.bmp', 5, 5, 0, 255, 0) # Чисто зеленый
    create_bmp('tests/tmp/mark.bmp', 5, 5, 0, 0, 255) # Чисто красный

    # 2. Создаем временный конфиг
    with open('tests/tmp/test.ini', 'w') as f:
        f.write("input_path = tests/tmp/orig.bmp\n")
        f.write("watermark_path = tests/tmp/mark.bmp\n")
        f.write("output_path = tests/tmp/result.bmp\n")
        f.write("alpha = 0.5\n")
        f.write("offset_x = 0\n")
        f.write("offset_y = 0\n")

    # 3. Определяем путь к исполняемому файлу в зависимости от ОС
    exe_name = 'watermark.exe' if sys.platform.startswith('win') else 'watermark'
    exe_path = os.path.join('build', 'Release', exe_name) if sys.platform.startswith('win') else os.path.join('build', exe_name)
    if not os.path.exists(exe_path):
        exe_path = os.path.join('build', exe_name) # fallback

    print(f"Запуск: {exe_path}")
    
    # 4. Запускаем программу
    result = subprocess.run([exe_path, 'tests/tmp/test.ini'], capture_output=True, text=True)
    if result.returncode != 0:
        print("STDOUT:", result.stdout)
        print("STDERR:", result.stderr)
        print("Тест провален: программа завершилась с ошибкой.")
        sys.exit(1)

    # 5. Проверяем результат
    # Смешиваем зеленый (0,255,0) и красный (0,0,255) с alpha=0.5
    # Ожидаем: R = 0*0.5 + 255*0.5 = 127, G = 255*0.5 + 0*0.5 = 127, B = 0
    r, g, b = check_bmp_pixel('tests/tmp/result.bmp', 0, 0)
    
    print(f"Пиксель (0,0) в результате: R={r}, G={g}, B={b}")
    if abs(r - 127) <= 1 and abs(g - 127) <= 1 and b == 0:
        print("[SUCCESS] Тест пройден! Цвета смешались правильно.")
        sys.exit(0)
    else:
        print("[ERROR] Тест провален! Неверный цвет.")
        sys.exit(1)

if __name__ == '__main__':
    main()