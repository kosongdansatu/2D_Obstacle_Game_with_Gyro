# Serial Logger Pengujian LVGL RP2350

Program Python untuk menerima data performa LVGL dari board RP2350 melalui
UART/serial dan menyimpannya sebagai file CSV per skenario pengujian.

Setiap percobaan disimpan secara otomatis ke folder `data_pengujian` dengan
nama `skenario<nomor>_run<n>_<waktu>.csv`, sehingga data lama tidak tertimpa.

## Persyaratan

- Python 3.9 atau lebih baru
- Board RP2350 yang terhubung melalui USB serial
- Firmware yang mengirim tepat lima angka per baris, dipisahkan koma
- Modul Python `pyserial`

## Instalasi

Dari folder proyek:

```bash
python -m pip install --upgrade pip
python -m pip install -r requirements.txt
```

Jika menggunakan Conda, lingkungan dapat diaktifkan terlebih dahulu:

```bash
conda activate rp-flappy
```

## Format Data dari Firmware

Setiap baris data firmware harus berbentuk:

```text
timestamp_ms,fps,free_heap_bytes,render_time_us,latency_ms
```

Contoh:

```text
19665,33.60,464176,24043,-1.00
```

Kolom yang disimpan:

| Kolom | Deskripsi |
|---|---|
| `timestamp_ms` | Waktu dari board dalam milidetik |
| `fps` | Jumlah frame per detik |
| `free_heap_bytes` | Memori heap yang masih tersedia |
| `render_time_us` | Waktu render dalam mikrodetik |
| `latency_ms` | Latensi dalam milidetik |

Baris tanpa format lima angka akan diabaikan. Nilai `-1.00` dapat digunakan
oleh firmware untuk menunjukkan bahwa pengukuran tidak tersedia.

## Menjalankan Logger

### Contoh macOS

```bash
python serial_logger.py --port /dev/cu.usbmodem14201 --scenario 1 --duration 180
```

### Contoh Windows

```bash
python serial_logger.py --port COM5 --scenario 1 --duration 180
```

### Contoh Linux

```bash
python serial_logger.py --port /dev/ttyACM0 --scenario 1 --duration 180
```

Untuk menghentikan pengambilan data sebelum durasi habis, tekan `Ctrl+C`.

## Opsi Perintah

| Opsi | Wajib | Default | Deskripsi |
|---|---:|---:|---|
| `--port` | Ya | - | Nama port serial board |
| `--scenario` | Ya | - | Nomor skenario, 1-7 |
| `--duration` | Tidak | `180` | Durasi pengambilan data dalam detik |
| `--baud` | Tidak | `115200` | Baud rate serial |
| `--outdir` | Tidak | `data_pengujian` | Folder penyimpanan file CSV |

Untuk melihat bantuan lengkap:

```bash
python serial_logger.py --help
```

## Mencari Nama Port Serial

### macOS

```bash
ls /dev/cu.*
```

### Linux

```bash
ls /dev/ttyACM* /dev/ttyUSB*
```

Atau gunakan Python:

```bash
python -m serial.tools.list_ports
```

### Windows

Jalankan PowerShell:

```powershell
Get-CimInstance Win32_PnPEntity | Where-Object {$_.Name -match 'COM'}
```

Kemudian gunakan nama port, misalnya `COM5`.

## Lokasi dan Format File CSV

Secara default hasil disimpan di:

```text
data_pengujian/skenario<nomor>_run<n>_<YYYYMMDD>_<HHMMSS>.csv
```

Contoh:

```text
data_pengujian/skenario1_run2_20260917_143017.csv
```

Setiap file memiliki header:

```text
timestamp_ms,fps,free_heap_bytes,render_time_us,latency_ms
```

## Pemecahan Masalah

- **Port tidak ditemukan:** Lepas dan sambungkan kembali board, lalu cari nama
  port serial terbaru.
- **Tidak ada data tersimpan:** Pastikan baud rate firmware dan logger sama
  dan firmware mengirim lima nilai dipisahkan koma.
- **`ModuleNotFoundError: No module named 'serial'`:** Jalankan
  `python -m pip install -r requirements.txt`.
- **Port sedang digunakan:** Tutup Serial Monitor IDE atau aplikasi lain yang
  membuka port tersebut.
- **Board mereset setelah port dibuka:** Logger menunggu dua detik secara
  otomatis agar board siap mengirim data.
