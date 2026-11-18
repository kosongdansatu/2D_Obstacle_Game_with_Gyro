"""
serial_logger.py
-----------------
Logger otomatis untuk pengujian performa LVGL pada RP2350.
Membaca data CSV yang dikirim firmware via UART, lalu menyimpannya
ke file per-skenario dengan penomoran otomatis (percobaan ke-1, 2, 3, ...).

Cara pakai:
    python serial_logger.py --port /dev/cu.usbmodem14201 --scenario 1 --duration 180

    --port      : nama serial port (Windows: COM5, dsb; Linux/Mac: /dev/ttyACM0)
    --scenario  : nomor skenario pengujian (1-7), sesuai Tabel Matriks di paper
    --duration  : lama pengujian dalam detik (default 180 = 3 menit)
    --baud      : baud rate (default 115200, samakan dengan Serial.begin() di firmware)
"""

import serial
import csv
import time
import argparse
import os
from datetime import datetime

def main():
    parser = argparse.ArgumentParser(description="Logger data pengujian LVGL RP2350")
    parser.add_argument("--port", required=True, help="Serial port, misal COM5 atau /dev/ttyACM0")
    parser.add_argument("--baud", type=int, default=115200, help="Baud rate serial")
    parser.add_argument("--scenario", required=True, help="Nomor skenario pengujian (1-7)")
    parser.add_argument("--duration", type=int, default=180, help="Durasi pengujian (detik)")
    parser.add_argument("--outdir", default="data_pengujian", help="Folder output CSV")
    args = parser.parse_args()

    os.makedirs(args.outdir, exist_ok=True)

    # Cari nomor percobaan berikutnya secara otomatis (biar tidak menimpa file lama)
    existing = [f for f in os.listdir(args.outdir) if f.startswith(f"skenario{args.scenario}_")]
    run_number = len(existing) + 1

    timestamp_str = datetime.now().strftime("%Y%m%d_%H%M%S")
    filename = os.path.join(args.outdir, f"skenario{args.scenario}_run{run_number}_{timestamp_str}.csv")

    print(f"[INFO] Menghubungkan ke {args.port} @ {args.baud} baud...")
    ser = serial.Serial(args.port, args.baud, timeout=1)
    time.sleep(2)  # beri waktu board reset setelah koneksi serial dibuka

    print(f"[INFO] Mulai logging skenario {args.scenario}, percobaan ke-{run_number}")
    print(f"[INFO] Durasi: {args.duration} detik. Simpan ke: {filename}")
    print("[INFO] Tekan Ctrl+C untuk berhenti lebih awal.\n")

    row_count = 0
    start_time = time.time()

    with open(filename, "w", newline="") as f:
        writer = csv.writer(f)
        # Header kolom, sesuaikan urutan dengan format yang dikirim firmware
        writer.writerow(["timestamp_ms", "fps", "free_heap_bytes", "render_time_us", "latency_ms"])

        try:
            while (time.time() - start_time) < args.duration:
                line = ser.readline().decode("utf-8", errors="ignore").strip()

                if not line:
                    continue

                # Validasi sederhana: baris harus berupa 5 nilai dipisah koma
                parts = line.split(",")
                if len(parts) != 5:
                    # Abaikan baris yang bukan data (misal pesan debug lain dari firmware)
                    continue

                try:
                    # Pastikan semua bisa dikonversi ke angka sebelum disimpan
                    numeric_parts = [float(p) for p in parts]
                except ValueError:
                    continue

                writer.writerow(parts)
                row_count += 1

                if row_count % 10 == 0:
                    elapsed = time.time() - start_time
                    print(f"  [{elapsed:.0f}s] {row_count} baris tersimpan | terakhir: {line}")

        except KeyboardInterrupt:
            print("\n[INFO] Dihentikan manual oleh pengguna.")

    ser.close()
    print(f"\n[SELESAI] Total {row_count} baris data tersimpan di: {filename}")

if __name__ == "__main__":
    main()
