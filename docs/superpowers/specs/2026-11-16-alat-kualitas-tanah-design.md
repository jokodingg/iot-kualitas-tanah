# Desain: Prototipe Alat Pengecek Kualitas Tanah + Siram Otomatis

Tanggal: 2026-11-16
Konteks: tugas kuliah, prototipe (bukan alat ukur presisi).

## Tujuan
- Membaca kelembapan tanah, EC/TDS tanah, dan suhu tanah, lalu menampilkannya di LCD 16x2.
- Menyiram otomatis dengan pompa 12V saat tanah kering.
- Tahap 2: menampilkan data dan mengontrol pompa lewat web lokal yang disajikan ESP32-C3.

## Non-tujuan
- Akurasi setara laboratorium. Nilai EC dari probe TDS air hanya indikator relatif dan hanya valid saat tanah lembab. Batasan ini ditulis di laporan.
- pH, NPK, dan layanan cloud (Blynk/ThingSpeak).

## Pendekatan
- A (dipilih): framework Arduino di PlatformIO, logika ambang dengan histeresis. Library matang dan tutorial banyak.
- B: MicroPython. Ditolak karena dukungan library untuk C3 dan TDS lebih sedikit.
- Dashboard: web lokal (ESP32 sebagai web server) dipilih daripada Blynk karena tidak butuh akun atau internet.

## Perangkat keras
| Fungsi | Pin ESP32-C3 Super Mini |
|---|---|
| Sensor kelembapan kapasitif v1.2 | GPIO0 (ADC) |
| Modul TDS | GPIO1 (ADC) |
| DS18B20 (pull-up 4,7 kΩ ke 3V3) | GPIO3 |
| LCD 16x2 I2C (SDA/SCL) | GPIO8 / GPIO9 |
| Relay 1 channel | GPIO4 |

Daya: adaptor 12V 2A ke relay/pompa. Step-down 5V memberi daya ESP32-C3 (pin 5V), relay, dan LCD. Sensor diberi daya 3V3. Dioda 1N4007 dipasang terbalik paralel di pompa. Semua GND disatukan.

Penempatan probe: probe kelembapan, probe TDS, dan DS18B20 ditancapkan berdekatan di pot.

## Perilaku
- Sensor dibaca setiap 2 detik. Nilai ADC dirata-rata 10 sampel.
- Kelembapan dalam % dari kalibrasi dua titik: ADC kering (udara) dan ADC basah (air). Kedua nilai disimpan sebagai konstanta.
- EC memakai rumus DFRobot dengan kompensasi suhu dari DS18B20.
- Pompa ON jika kelembapan < 35%. Pompa OFF jika > 55% atau sudah menyala 10 detik. Setelah OFF, pompa tidak menyala lagi selama 60 detik. Semua angka ini konstanta yang mudah diubah.
- LCD baris 1: `M:42% T:28.5C`. Baris 2: `EC:850us P:OFF`.
- Tahap 2: ESP32 membuat WiFi AP sendiri. Halaman di `192.168.4.1` menampilkan data sensor yang diperbarui setiap 2 detik lewat endpoint JSON `/data`. Tombol mode AUTO/MANUAL dan tombol pompa ON/OFF tersedia. Mode manual tetap memakai batas 10 detik.

## Diserahkan ke pembuat
- Pembagian file kode, pemilihan library spesifik, dan tampilan halaman web.
- Nilai kalibrasi aktual diukur saat perakitan.
