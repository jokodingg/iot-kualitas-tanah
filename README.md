# Alat Pengecek Kualitas Tanah + Siram Otomatis

Prototipe alat yang membaca **kelembapan** dan **suhu** tanah, lalu menyiram tanaman secara otomatis saat tanah kering. Data tampil di layar LCD dan bisa dipantau atau dikendalikan dari HP lewat WiFi, tanpa perlu internet.

Dibuat dengan board **ESP32-C3 Super Mini** dan **PlatformIO**.

---

## Daftar isi

1. [Cara kerja alat](#1-cara-kerja-alat)
2. [Cara memakai alat](#2-cara-memakai-alat)
3. [Komponen](#3-komponen)
4. [Skema pin](#4-skema-pin)
5. [Instalasi PlatformIO (Windows / Mac)](#5-instalasi-platformio-windows--mac)
6. [Upload program ke board](#6-upload-program-ke-board)
7. [Tes per komponen](#7-tes-per-komponen)
8. [Kalibrasi sensor kelembapan](#8-kalibrasi-sensor-kelembapan)
9. [Mengubah pengaturan](#9-mengubah-pengaturan)
10. [Masalah umum](#10-masalah-umum)

---

## 1. Cara kerja alat

```
Sensor kelembapan ─┐
                   ├─► ESP32-C3 ─► Relay ─► Pompa air ─► Pot tanaman
Sensor suhu ───────┘       │
                           ├─► Layar LCD
                           └─► WiFi ─► Dashboard di HP
```

1. Setiap 2 detik, alat membaca kelembapan tanah (dalam %) dan suhu tanah.
2. Jika kelembapan **di bawah 35%**, tanah dianggap kering dan pompa menyala.
3. Pompa menyala **5 detik**, lalu berhenti **10 detik** supaya air sempat meresap. Layar menampilkan `Jeda resap`.
4. Siklus ini diulang sampai kelembapan **di atas 55%**. Setelah itu pompa berhenti.

Ambang 35% dan 55% sengaja dibuat berjarak supaya pompa tidak bolak-balik menyala dan mati.

---

## 2. Cara memakai alat

### Menyalakan

1. Tancapkan sensor kelembapan ke tanah sampai garis batas di badan sensor, jangan lebih dalam.
2. Tancapkan sensor suhu (DS18B20) di dekatnya.
3. Masukkan pompa ke wadah air. **Pompa harus terendam** sebelum alat dinyalakan.
4. Arahkan ujung selang ke permukaan tanah di pot.
5. Colokkan kabel USB-C board ke adaptor HP 5V (minimal 1A).

### Membaca layar LCD

```
┌────────────────┐
│M:42% T:28.5C   │   M = kelembapan tanah, T = suhu tanah
│Pompa: OFF      │   status pompa
└────────────────┘
```

| Baris 2 | Arti |
|---|---|
| `Pompa: ON` | Pompa sedang menyiram |
| `Jeda resap 8s` | Menunggu air meresap, lalu tanah dicek lagi |
| `Pompa: OFF` | Tanah cukup lembab, tidak perlu disiram |
| `T:--` di baris 1 | Sensor suhu tidak terbaca (cek kabel) |

### Memakai dashboard dari HP

1. Buka pengaturan WiFi di HP, lalu sambungkan ke **`KualitasTanah`** dengan password **`tanah123`**.
2. Jika HP memberi peringatan "tidak ada internet", pilih **tetap tersambung**. WiFi ini memang tidak terhubung ke internet.
3. Buka browser lalu ketik **`192.168.4.1`**.

```
┌──────────────────────────────┐
│ Kelembapan      Suhu tanah   │
│   42 %           28.5 °C     │
│ Pompa: OFF                   │
│ [Siram 3 detik]   [Stop]     │
│ [Mode: AUTO]                 │
│ [Pompa ON]   [Pompa OFF]     │
└──────────────────────────────┘
```

| Tombol | Fungsi |
|---|---|
| **Siram 3 detik** | Menyalakan pompa 3 detik, lalu mati sendiri. Bisa dipakai kapan saja. |
| **Stop** | Mematikan pompa saat itu juga. |
| **Mode: AUTO / MANUAL** | AUTO: alat menyiram sendiri berdasarkan kelembapan. MANUAL: pompa hanya menyala lewat tombol. |
| **Pompa ON / OFF** | Hanya aktif di mode MANUAL. Pompa tetap mati sendiri setelah maksimal 5 detik. |

Data di dashboard diperbarui otomatis setiap 2 detik.

### Hal yang perlu diingat

- **Jangan biarkan sensor kelembapan di udara.** Di udara sensor membaca 0% (sangat kering), sehingga pompa terus menyiram.
- Letakkan wadah air **lebih rendah** dari ujung selang supaya air tidak mengalir sendiri saat pompa mati.
- Jauhkan board dan relay dari percikan air.

---

## 3. Komponen

| Komponen | Jumlah | Keterangan |
|---|---|---|
| ESP32-C3 Super Mini | 1 | Otak alat |
| Sensor kelembapan tanah kapasitif v1.2 | 1 | |
| Sensor suhu DS18B20 waterproof | 1 | |
| LCD 16x2 + modul I2C | 1 | Alamat biasanya `0x27` |
| Modul relay 1 channel 5V | 1 | |
| Pompa celup mini 5V | 1 | |
| Dioda 1N4007 | 1 | Pelindung dari lonjakan tegangan pompa |
| Kapasitor elektrolit 470–1000 µF | 1 | Menstabilkan daya saat pompa menyala |
| Resistor 4,7 kΩ | 1 | Untuk DS18B20 |
| Adaptor HP 5V minimal 1A + kabel USB-C | 1 | Sumber daya |
| Breadboard, kabel jumper, selang, wadah air | | |

---

## 4. Skema pin

Board dilihat dengan port USB di atas dan komponen menghadap Anda:

```
                         ESP32-C3 Super Mini
                          ┌─────[USB]─────┐
 Jalur 5V      ────────── ┤ 5V       GPIO5 ├─  (tidak dipakai)
 GND bersama   ────────── ┤ GND      GPIO6 ├─  (tidak dipakai)
 Jalur 3V3     ────────── ┤ 3V3      GPIO7 ├─  (tidak dipakai)
 Relay IN      ◄───────── ┤ GPIO4    GPIO8 ├──────► LCD SDA
 DS18B20 DATA  ─────────► ┤ GPIO3    GPIO9 ├──────► LCD SCL
 (tidak dipakai)        ─ ┤ GPIO2   GPIO10 ├─  (tidak dipakai)
 (tidak dipakai)        ─ ┤ GPIO1   GPIO20 ├─  (RX, jangan dipakai)
 Kelembapan AOUT ───────► ┤ GPIO0   GPIO21 ├─  (TX, jangan dipakai)
                          └────[antena]────┘
```

Urutan pin di atas adalah tata letak umum Super Mini. Sebelum merakit, cocokkan dengan label yang tercetak di board Anda.

### Detail sambungan

**Sensor kelembapan (pakai 3V3, bukan 5V)**

| Sensor | Board |
|---|---|
| VCC | 3V3 |
| GND | GND |
| AOUT | GPIO0 |

**Sensor suhu DS18B20 (pakai 3V3)**

| Kabel | Board |
|---|---|
| Merah | 3V3 |
| Hitam | GND |
| Kuning | GPIO3 |

Pasang **resistor 4,7 kΩ** antara kabel kuning dan 3V3.

**LCD I2C**

| LCD | Board |
|---|---|
| VCC | 5V |
| GND | GND |
| SDA | GPIO8 |
| SCL | GPIO9 |

**Relay**

| Relay | Board |
|---|---|
| VCC | 5V |
| GND | GND |
| IN | GPIO4 |

**Pompa (lewat relay)**

Relay punya 3 terminal sekrup: COM, NO, dan NC. Pakai **COM dan NO**, supaya pompa mati saat relay tidak aktif.

```
Pin 5V board ──► COM ┐
                     │ [saklar relay]
                NO ──┘──► pompa (+) merah
GND board ◄────────────── pompa (−) hitam

Dioda 1N4007 dipasang di kedua kabel pompa:
   sisi BERGARIS ke pompa (+), sisi lain ke pompa (−)

Kapasitor 470–1000 µF antara 5V dan GND, dekat board:
   kaki (−) ke GND
```

> **Peringatan**
> - Jangan sambungkan pompa langsung ke pin GPIO. Pin GPIO bisa rusak.
> - Jangan sambungkan sensor ke 5V. Pin ESP32-C3 hanya tahan 3,3V.
> - Pastikan dioda tidak terbalik. Dioda yang terbalik membuat korsleting saat pompa menyala.
> - Semua GND harus tersambung jadi satu.

---

## 5. Instalasi PlatformIO (Windows / Mac)

PlatformIO adalah alat untuk mengompilasi program dan mengunggahnya ke board. Cara termudah adalah memakainya di dalam **Visual Studio Code**.

### Langkah 1: Pasang Visual Studio Code

1. Buka https://code.visualstudio.com
2. Unduh versi untuk **Windows** atau **Mac**, lalu pasang seperti aplikasi biasa.

### Langkah 2: Pasang ekstensi PlatformIO

1. Buka VS Code.
2. Klik ikon **Extensions** di sisi kiri (ikon empat kotak), atau tekan `Ctrl+Shift+X` (Windows) / `Cmd+Shift+X` (Mac).
3. Cari **PlatformIO IDE**, lalu klik **Install**.
4. Tunggu sampai selesai. Proses ini bisa beberapa menit karena PlatformIO mengunduh perangkatnya sendiri.
5. Tutup lalu buka lagi VS Code. Ikon kepala semut PlatformIO akan muncul di sisi kiri.

> **Windows:** jika muncul pesan bahwa Python tidak ditemukan, pasang Python dari https://python.org. Saat instalasi, centang **"Add Python to PATH"**, lalu buka ulang VS Code.

### Langkah 3: Unduh proyek ini

**Cara mudah:** buka https://github.com/jokodinggg/iot-kualitas-tanah, klik tombol hijau **Code**, pilih **Download ZIP**, lalu ekstrak.

**Dengan git:**
```
git clone https://github.com/jokodinggg/iot-kualitas-tanah.git
```

### Langkah 4: Buka proyek

1. Di VS Code, pilih **File → Open Folder**.
2. Pilih folder **`iot-kualitas-tanah`**, yaitu folder yang berisi file `platformio.ini`.
3. Tunggu sampai PlatformIO selesai menyiapkan proyek. Lihat status di bagian bawah layar.

Saat build pertama, PlatformIO otomatis mengunduh semua library yang dibutuhkan. Anda tidak perlu memasangnya sendiri.

---

## 6. Upload program ke board

1. Colokkan board ke laptop dengan kabel USB-C. Pastikan kabelnya bisa untuk transfer data, bukan kabel khusus charging.
2. Di bilah biru paling bawah VS Code, klik tombol berikut:

| Ikon | Fungsi |
|---|---|
| ✓ (centang) | Build: cek apakah program bisa dikompilasi |
| → (panah kanan) | **Upload**: kirim program ke board |
| 🔌 (colokan) | **Serial Monitor**: lihat pesan dari board |

3. Klik **→ Upload** dan tunggu sampai muncul tulisan `SUCCESS`.
4. Klik **🔌 Serial Monitor**. Setiap 2 detik akan muncul baris seperti:
   ```
   raw=3650 moisture=50.3% temp=27.8C(ok) pump=OFF mode=auto
   ```

**Lewat terminal** (opsional, dari folder proyek):
```
pio run -e c3 -t upload
pio device monitor
```

> **Jangan colokkan USB laptop dan adaptor lain ke board bersamaan.** Saat upload, cukup pakai USB laptop.

---

## 7. Tes per komponen

Sebelum menjalankan program utama, sebaiknya tes setiap komponen satu per satu. Program tesnya ada di folder `testing/`.

| Folder | Yang dites | Hasil yang benar |
|---|---|---|
| `testing/soil-moisture` | Sensor kelembapan | Angka `raw` tinggi di udara, turun saat menyentuh air |
| `testing/ds18b20` | Sensor suhu | "sensor ditemukan: 1", suhu naik saat probe digenggam |
| `testing/lcd` | Layar LCD | Satu alamat ditemukan (biasanya `0x27`), layar menampilkan "Tes LCD OK" |
| `testing/relay` | Relay tanpa pompa | Relay berbunyi klik setiap 2 detik |
| `testing/pump` | Relay + pompa | Pompa menyala 3 detik sekali. Tekan tombol RESET untuk mengulang |

Cara menjalankan:
1. Di VS Code, pilih **File → Open Folder** lalu buka salah satu folder tes, misalnya `testing/lcd`.
2. Klik **→ Upload**, lalu **🔌 Serial Monitor**.

Lewat terminal:
```
cd testing/lcd
pio run -t upload
pio device monitor
```

Setelah semua tes lolos, buka lagi folder utama `iot-kualitas-tanah`, lalu upload program utama.

---

## 8. Kalibrasi sensor kelembapan

Setiap sensor memberi angka yang sedikit berbeda, jadi sensor perlu dikalibrasi sekali.

1. Jalankan `testing/soil-moisture`.
2. Tancapkan sensor ke **tanah kering** dari pot Anda. Catat angka `raw` setelah stabil.
3. Siram tanah sampai basah, tunggu sekitar 1 menit, lalu catat angka `raw` lagi.
4. Buka `include/config.h`, lalu isi kedua angka itu:
   ```cpp
   constexpr int ADC_DRY = 4095;   // raw saat tanah kering
   constexpr int ADC_WET = 3200;   // raw saat tanah basah
   ```
5. Upload ulang program utama.

> Jika angka di udara selalu **4095**, kemungkinan VCC sensor tersambung ke 5V. Pindahkan ke **3V3**, lalu kalibrasi ulang.

---

## 9. Mengubah pengaturan

Semua pengaturan ada di file `include/config.h`. Setelah mengubahnya, upload ulang program.

| Pengaturan | Nilai awal | Arti |
|---|---|---|
| `ON_BELOW` | `35.0` | Pompa mulai menyiram jika kelembapan di bawah angka ini (%) |
| `OFF_ABOVE` | `55.0` | Pompa berhenti jika kelembapan di atas angka ini (%) |
| `MAX_ON_MS` | `5000` | Lama pompa menyala per siklus (milidetik, 5000 = 5 detik) |
| `LOCKOUT_MS` | `10000` | Lama jeda resap di antara siklus (milidetik) |
| `WEB_PULSE_MS` | `3000` | Durasi tombol "Siram 3 detik" di dashboard |
| `ADC_DRY` / `ADC_WET` | `4095` / `3200` | Hasil kalibrasi sensor kelembapan |
| `LCD_ADDR` | `0x27` | Alamat LCD (lihat hasil `testing/lcd`) |
| `AP_SSID` / `AP_PASS` | `KualitasTanah` / `tanah123` | Nama dan password WiFi alat |

---

## 10. Masalah umum

| Masalah | Penyebab dan solusi |
|---|---|
| Upload gagal / port tidak ditemukan | Ganti kabel USB-C yang bisa transfer data. Jika masih gagal, **tahan tombol BOOT**, tekan dan lepas **RESET**, lalu lepas BOOT, kemudian upload lagi. |
| Serial Monitor kosong | Tekan tombol RESET di board. Pastikan kecepatan monitor **115200**. |
| LCD menyala tapi tidak ada tulisan | Putar potensiometer biru di belakang LCD untuk mengatur kontras. |
| LCD error sesaat saat pompa menyala | Lonjakan daya dari pompa. Pasang dioda 1N4007 dan kapasitor 470–1000 µF, lalu pakai adaptor 5V minimal 1A. Program sudah otomatis memulihkan LCD. |
| Banyak alamat muncul di `testing/lcd` | Jalur I2C bermasalah. Cek GND bersama, sambungan SDA/SCL, dan kabel jumper yang longgar. |
| `T:--` di LCD | Sensor suhu tidak terbaca. Cek kabel kuning ke GPIO3 dan resistor 4,7 kΩ. |
| Pompa terus menyala | Sensor kelembapan di udara atau tanah memang kering. Pastikan sensor tertancap di tanah. |
| Pompa menyala saat `Pompa: OFF` | Kabel pompa terpasang di terminal **NC**. Pindahkan ke **NO**. |
| Board restart sendiri saat pompa menyala | Daya kurang. Pakai adaptor 5V minimal 1A (bukan port laptop) dan pasang kapasitor. |
| WiFi `KualitasTanah` tidak muncul | Lihat Serial Monitor saat board baru menyala. Harus ada tulisan `WiFi AP aktif`. Tunggu beberapa detik, lalu refresh daftar WiFi di HP. |
| Dashboard tidak terbuka | Pastikan HP tersambung ke `KualitasTanah`, lalu buka `192.168.4.1` (bukan `https://`). |

---

## Struktur folder

```
iot-kualitas-tanah/
├── platformio.ini        Pengaturan PlatformIO untuk program utama
├── include/config.h      Semua pengaturan: pin, ambang, kalibrasi, WiFi
├── src/                  Program utama
│   ├── main.cpp          Alur utama
│   ├── sensors.cpp       Membaca sensor kelembapan dan suhu
│   ├── display.cpp       Menampilkan data di LCD
│   └── web.cpp           WiFi dan dashboard HP
├── lib/soil_logic/       Logika perhitungan kelembapan dan aturan pompa
├── test/                 Uji logika (jalan di laptop: pio test -e native)
├── testing/              Program tes per komponen
└── docs/                 Dokumen desain dan rencana
```
