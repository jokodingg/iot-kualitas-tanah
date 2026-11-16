# Tes per komponen

Setiap folder berisi satu program kecil untuk menguji satu komponen. Rakit dan tes satu komponen dulu sebelum menambah komponen berikutnya.

Cara menjalankan (ganti `<nama>`):

```
pio run -e <nama> -t upload && pio device monitor
```

| Urutan | Env | Komponen | Hasil yang benar |
|---|---|---|---|
| 1 | `moisture` | Sensor kelembapan (GPIO0) | Nilai `raw` di udara tinggi, di air rendah. Catat keduanya ke `ADC_DRY`/`ADC_WET` di `include/config.h`. |
| 2 | `tds` | Modul TDS (GPIO1) | Probe di udara mendekati 0 V. Probe di air keran sekitar 100–600 uS/cm. |
| 3 | `ds18b20` | Sensor suhu (GPIO3) | "sensor ditemukan: 1". Suhu masuk akal dan naik saat probe digenggam. |
| 4 | `lcd` | LCD I2C (GPIO8/9) | Alamat ditemukan, layar menampilkan "Tes LCD OK" dan angka hitungan. |
| 5 | `relay` | Relay tanpa pompa (GPIO4) | Relay berbunyi klik setiap 2 detik, sesuai teks ON/OFF di Serial. |
| 6 | `pump` | Relay + pompa 12V | Pompa menyala 3 detik sekali lalu mati. Tekan RESET untuk mengulang. |

Setelah semua lolos, upload firmware utama: `pio run -e c3 -t upload`.
