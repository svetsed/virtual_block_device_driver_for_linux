# Virtual Block Device Driver for Linux

![Linux Kernel](https://img.shields.io/badge/Linux_Kernel-5.15+-blue.svg)
![License](https://img.shields.io/badge/License-GPL_2.0-green.svg)

Простой драйвер виртуального блочного устройства, создающего RAM-диск в пространстве ядра Linux.

Статус: в процессе

## Особенности

- Создает блочное устройство `/dev/vdisk`
- Поддерживает операции чтения/записи
- Хранит данные в оперативной памяти
- Размер настраивается (по умолчанию 50 МБ)
- Поддержка TRIM-операций
