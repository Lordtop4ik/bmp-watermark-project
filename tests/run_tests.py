import struct
import os
import subprocess
import sys

# Принудительно включаем UTF-8 (на всякий случай, хотя ниже мы используем английский)
if sys.stdout.encoding != 'utf-8':
    sys.stdout.reconfigure(encoding='utf-8')

def create_bmp(filename, width, height, b, g, r):
    # Создает простой 24-битный BMP, залитый одним цветом
    row_size = (width * 3 + 3) & ~3  # Выравнивание строки до 4 байт
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

def check_bmp_pixel(filename, width, x, y):
    # Читает цвет конкретного пикселя из BMP-файла
    row_size = (width * 3 + 3) & ~3
    with open(filename, 'rb') as f:
        # 54 - размер заголовков, y * row_size - смещение до нужной строки, x * 3 - смещение внутри строки
        f.seek(54 + y * row_size + x * 3)
        b, g, r = struct.unpack('<BBB', f.read(3))
        return r, g, b

def main():
    os.makedirs('tests/tmp', exist_ok=True)
    
    width = 100
    height = 100
    
    print(f"Generating test images {width}x{height}...")
    # 1. Создаем тестовые изображения
    create_bmp('tests/tmp/orig.bmp', width, height, 0, 255, 0) # Чисто зеленый
    create_bmp('tests/tmp/mark.bmp', width, height, 0, 0, 255) # Чисто красный

    # 2. Определяем путь к исполняемому файлу в зависимости от ОС
    exe_name = 'watermark.exe' if sys.platform.startswith('win') else 'watermark'
    exe_path = os.path.join('build', 'Release', exe_name) if sys.platform.startswith('win') else os.path.join('build', exe_name)
    if not os.path.exists(exe_path):
        exe_path = os.path.join('build', exe_name) # fallback

    print(f"Running executable: {exe_path}")
    
    # 3. Запускаем программу через аргументы командной строки
    result = subprocess.run([
        exe_path, 
        'tests/tmp/orig.bmp', 
        'tests/tmp/mark.bmp', 
        'tests/tmp/result.bmp', 
        '0.5', '0', '0'
    ], capture_output=True, text=True)
    
    if result.returncode != 0:
        print("STDOUT:", result.stdout)
        print("STDERR:", result.stderr)
        print("Test failed: program exited with error.")
        sys.exit(1)

    # 4. Проверяем результат в центре картинки (50, 50)
    # Смешиваем зеленый (0,255,0) и красный (0,0,255) с alpha=0.5
    # Ожидаем: R = 0*0.5 + 255*0.5 = 127, G = 255*0.5 + 0*0.5 = 127, B = 0
    r, g, b = check_bmp_pixel('tests/tmp/result.bmp', width, 50, 50)
    
    print(f"Pixel at (50,50) in result: R={r}, G={g}, B={b}")
    if abs(r - 127) <= 1 and abs(g - 127) <= 1 and b == 0:
        print("[SUCCESS] Test passed! Colors blended correctly.")
        sys.exit(0)
    else:
        print("[ERROR] Test failed! Incorrect color.")
        sys.exit(1)

if __name__ == '__main__':
    main()