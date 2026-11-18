# Flappy Bird Gyro untuk Waveshare RP2040 LCD 1.69"

Game 2D Obstacle sederhana untuk perangkat Waveshare dengan layar LCD 1,69 inci (resolusi 240 × 280). Burung dikendalikan dengan mengayunkan perangkat; tampilan dan skor dibuat menggunakan LVGL.

## Fitur

- Kendali object seperti bola melalui giroskop QMI8658.
- Pipa bergerak dengan celah acak dan skor bertambah setiap pipa berhasil dilewati.
- Pesan `GAME OVER` saat burung menabrak pipa atau batas bawah layar.
- Ayunan yang sama memulai ulang permainan setelah kalah.
- Tekan dan tahan tombol power perangkat sekitar satu detik untuk mematikan layar dan perangkat.

## Perangkat keras

- Waveshare RP2040 LCD 1.69" (layar 240 × 280, touch CST816S dan sensor QMI8658 pada board).
- Kabel USB untuk pemrograman dan daya.

Pin SPI, I2C, tombol, dan daya sudah didefinisikan di `DEV_Config.h`; gunakan board dengan pemetaan pin tersebut.

## Perangkat lunak

- Arduino IDE atau toolchain Arduino yang mendukung board RP2040 Waveshare.
- Core Arduino RP2040 yang sesuai dengan board.
- Library LVGL (`lvgl.h`). Konfigurasi LVGL harus cocok dengan integrasi pada `LVGL_example.cpp` dan board.
- Berkas pendukung driver Waveshare yang disertakan di repositori ini.

Repositori ini belum menyertakan konfigurasi proyek atau versi LVGL yang dipatok. Jika kompilasi gagal karena API atau konfigurasi LVGL, cocokkan versi library dan `lv_conf.h` dengan contoh Waveshare yang digunakan.

## Menjalankan

1. Pasang core board RP2040 dan library LVGL di Arduino IDE.
2. Buka sketch utama `2D_Game.ino` bersama seluruh berkas sumber dan header di folder proyek ini. Jika IDE meminta nama folder sketch sama dengan nama file `.ino`, salin proyek ke folder bernama `2D_Game` atau sesuaikan nama folder dan file utama.
3. Pilih board Waveshare RP2040 LCD 1.69" yang sesuai, lalu pilih port USB perangkat.
4. Kompilasi dan unggah sketch. Buka Serial Monitor bila ingin melihat pesan inisialisasi.

## Cara bermain

- Ayunkan perangkat pada sumbu giroskop Y untuk membuat burung melompat.
- Hindari pipa hijau dan jangan jatuh melewati tepi bawah layar.
- Setelah kalah, ayunkan perangkat untuk memulai permainan baru.
- Tahan tombol power perangkat untuk mematikan perangkat.

Sensitivitas ayunan, gravitasi, kekuatan lompatan, ukuran celah, dan kecepatan pipa dapat diubah pada bagian variabel game di `2D_Game.ino`.

## Struktur berkas

- `2D_Game.ino` — inisialisasi board, logika game, input giroskop, dan shutdown.
- `LVGL_example.*` — inisialisasi tampilan LVGL.
- `LCD_1in69.*`, `CST816S.*`, `QMI8658.*`, `PCF85063A.*`, `DEV_Config.*` — driver LCD, touch, sensor, RTC, dan GPIO.
- `GUI_Paint.*`, `ImageData.*`, `fonts.h`, `font*.cpp` — utilitas grafis dan aset font.

## Lisensi

Beberapa berkas driver menyertakan pemberitahuan lisensi MIT dari Waveshare. Periksa header berkas terkait sebelum mendistribusikan ulang perubahan atau keseluruhan proyek.
