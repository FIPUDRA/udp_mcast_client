# Инструкция:
# 1. Установить интерпретатор python версии 3
# 2. Поправить константы (ниже)
# 3. Подключиться к сети, в которой запущен UDP-сервер
# 4. Запустить скрипт
#
# Формат пакета:
# +-------------------+--------------+-----+----------------+---------+-----+----------+
# | Стартовые 2 байта | Номер пакета | MAC | UNIX timestamp | Канал 1 | ... | Канал 16 |
# +-------------------+--------------+-----+----------------+---------+-----+----------+
#
# Стартовые 2 байта:
# Первый байт 0 (в HEX: 0x00)
# Второй байт 5 (в HEX: 0x05)

# 2 байта счетчика пакетов:
# Счетчик номер пакета от 0 до 255. Нужен для обнаружения потерянных пакетов.

# Канал №X:
# Преставляет из себя набор показаний с 16-и датчиков по 4 байта каждый
# Первый датчик передается в начале, последний в конце.
# Показания одного датчика передаются 2 байтами, младший байт идет в начале, старший в конце.
# Пример того, как выглядит один отсчет:
# +----------+----------+----------+----------+----------+----------+--- ---+-----------+
# | Датчик 1 | Датчик 1 | Датчик 1 | Датчик 1 | Датчик 2 | Датчик 2 |  ...  | Датчик 16 |
# |    MSB   |          |          |    LSB   |    MSB   |          |  ...  |    LSB    |
# +----------+----------+----------+----------+----------+----------+--- ---+-----------+
#

import socket
import struct
import math
from time import sleep
from os import urandom
import msvcrt

HOST = "239.1.2.3" # IP адрес UDP сервера
PORT = 32768 # порт, который слушает UDP сервер
MULTICAST_TTL = 2 # предельное число переходов, за которое пакет может существовать
DELAY = 0.001 # задержка м/д отправками пакета в секундах (по умолчанию 0.05)

# Package creation
cnt_packLSB = 0
cnt_packMSB = 0
count = 0 # 4 data byte
count1 = 157
data_arr = bytearray(80)
data_float = bytearray(4)

# startByte
data_arr[0] = 0 # 0x00
data_arr[1] = 5 # 0x05

# MAC
data_arr[4] = 237 # 0xED
data_arr[5] = 72 # 0x48
data_arr[6] = 69 # 0x45
data_arr[7] = 1 # 0x01

# timestamp
for i in range(8,13,1):
    data_arr[i] = 0 # 0x00
data_arr[13] = 88 # 0x58
  
# Open connection
CLIENT = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
CLIENT.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_TTL, MULTICAST_TTL)

print("Выберите форму сигнала: \n1 - Пила \n2 - Синус \n3 - Меандр")
key = msvcrt.getch()

print('Press any key to exit')
while True:
    if msvcrt.kbhit():
        break

    # Number of packeges
    data_arr[2] = cnt_packMSB
    data_arr[3] = cnt_packLSB
    data_arr[14] = cnt_packMSB
    
    # Data device
    for i in range(16,80,4):
        #key = msvcrt.getch()
        #print(str(key))
        if (key == b'1'):
            data_float = struct.pack('>f',count)
            data_arr[i] = data_float[0]
            data_arr[i+1] = data_float[1]
            data_arr[i+2] = cnt_packLSB
            data_arr[i+3] = cnt_packLSB
            count = count + 2.0
            if count == 32.0:
                count = 0
        if (key == b'2'):
            data_float = struct.pack('>f',math.sin(count))
            data_arr[i] = data_float[0]
            data_arr[i+1] = data_float[1]
            data_arr[i+2] = cnt_packLSB
            data_arr[i+3] = cnt_packLSB
            count = count + 2.0
        if (key == b'3'):
            data_float = struct.pack('>f',count)
            data_arr[i] = data_float[0]
            data_arr[i+1] = data_float[1]
            data_arr[i+2] = cnt_packLSB
            data_arr[i+3] = cnt_packLSB
            count = 1
            if cnt_packLSB >= 127:
                count = 0
    
    # Sending packege
    CLIENT.sendto(data_arr, (HOST, PORT))

    if cnt_packLSB == 255:
        cnt_packLSB = 0
        cnt_packMSB = cnt_packMSB + 1
    else:
        cnt_packLSB = cnt_packLSB + 1
        
    if cnt_packMSB == 255:
        cnt_packLSB = 0
        cnt_packMSB = 0
        
    sleep(DELAY)

# Close connection
CLIENT.close()
