# 📄 PRODUCT REQUIREMENTS DOCUMENT (PRD)

## 🏷️ Judul Project
**Visualisasi Komunikasi Data Jaringan Komputer Berbasis 3D Menggunakan OpenGL**

---

## 🎯 Latar Belakang
Dalam jaringan komputer, proses pengiriman data antar perangkat seringkali sulit dipahami karena bersifat abstrak. Oleh karena itu, dibutuhkan sebuah media visual interaktif berbasis 3D untuk membantu memahami alur komunikasi data secara lebih intuitif.

---

## 🎯 Tujuan
- Memvisualisasikan proses pengiriman data dari satu komputer ke komputer lain
- Menunjukkan peran perangkat jaringan (PC, Switch, Router)
- Menampilkan komunikasi jaringan dalam skala lokal dan global
- Mengimplementasikan konsep grafika komputer menggunakan OpenGL

---

## 👥 Target User
- Mahasiswa IT
- Dosen / pengajar
- Pemula yang belajar jaringan komputer

---

## 🧩 Fitur Utama

### 1. 🖥️ Menu Utama
- Start Simulation
- Pilih Mode:
  - Mode Lokal (PC - Switch - PC)
  - Mode Global (Bumi & Gedung)
- Exit

---

### 2. 🔗 Mode 1: Simulasi Jaringan Lokal

#### 🎬 Deskripsi
Menampilkan komunikasi data dari **PC-A ke PC-B melalui Switch dan Router**

#### 🧱 Komponen:
- PC-A (sender)
- PC-B (receiver)
- Switch
- Router
- Kabel jaringan (line)

#### ⚙️ Fitur:
- Tombol **Send Data**
- Animasi packet (bola kecil) bergerak:
  - PC-A → Switch → Router → Switch → PC-B
- Highlight device aktif (berubah warna)
- Label teks:
  - "Sending..."
  - "Processing at Switch"
  - "Routing..."

---

### 3. 🌍 Mode 2: Simulasi Jaringan Global

#### 🎬 Deskripsi
Visualisasi komunikasi data antar lokasi di dunia

#### 🌐 Komponen:
- Sphere (bumi) + texture peta dunia
- Gedung (cube + texture)
- Kabel global (garis melengkung)
- Data packet (objek kecil bergerak)

#### ⚙️ Fitur:
- Bumi berputar perlahan
- Gedung di beberapa lokasi (misal Asia & Amerika)
- Animasi pengiriman data antar gedung
- Kabel berbentuk lengkungan (arc)

---

### 4. 🎥 Kamera & Navigasi
- WASD → gerak kamera
- Mouse → rotasi view
- Scroll → zoom in/out

---

### 5. 💡 Visual & Efek
- Lighting (Phong / Blinn-Phong)
- Texture mapping
- Highlight object aktif
- Animasi halus

---

## 🧠 Konsep Informatika yang Digunakan
- Komunikasi data jaringan
- Routing sederhana (alur paket)
- Representasi node & edge (graf)
- Transformasi 3D (translate, rotate, scale)

---

## 🛠️ Teknologi yang Digunakan
- OpenGL
- GLFW (window & input)
- GLAD (OpenGL loader)
- stb_image (texture)
- (Opsional) Assimp (model loading)

---

## 🧱 Arsitektur Sistem (Sederhana)

### Class Utama:
- `Node` (PC, Switch, Router, Gedung)
- `Connection` (kabel)
- `Packet` (data berjalan)
- `Camera`
- `SceneManager`

---

## 🔄 Alur Sistem

### Mode Lokal:
1. User klik "Send Data"
2. Packet muncul di PC-A
3. Bergerak ke Switch
4. Menuju Router
5. Diteruskan ke PC-B
6. Status: "Delivered"

---

### Mode Global:
1. User klik "Send Data"
2. Packet muncul di Gedung A
3. Bergerak melalui kabel melengkung
4. Sampai ke Gedung B

---

## 📊 Kriteria Keberhasilan
- Objek 3D tampil dengan benar
- Animasi packet berjalan lancar
- Interaksi user berjalan (menu & input)
- Visualisasi sesuai konsep jaringan

---

## ⚠️ Batasan (Scope)
- Tidak menggunakan jaringan nyata (hanya simulasi)
- Routing tidak kompleks (tidak pakai protokol asli)
- Fokus pada visualisasi, bukan backend network

---

## 🚀 Pengembangan Lanjutan (Opsional)
- Tambah lebih banyak node
- Simulasi packet loss
- UI lebih interaktif
- Multi-path routing

---

## 🏁 Kesimpulan
Project ini menggabungkan:
- Visual 3D
- Konsep jaringan
- Implementasi OpenGL

Sehingga menjadi media pembelajaran interaktif yang menarik dan informatif.