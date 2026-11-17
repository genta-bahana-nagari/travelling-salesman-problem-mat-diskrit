
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <numeric>
#include <limits>
#include <chrono>
#include <iomanip>

using namespace std;

// 1. Menghitung total jarak suatu rute
double hitungTotalJarak(
    const vector<int>& rute,
    const vector<vector<double>>& D
) {
    double total = 0.0;

    for (size_t i = 0; i < rute.size() - 1; i++) {
        total += D[rute[i]][rute[i + 1]];
    }

    return total;
}

// 2. Menampilkan rute berdasarkan nama lokasi
void cetakRute(
    const vector<int>& rute,
    const vector<string>& lokasi
) {
    for (size_t i = 0; i < rute.size(); i++) {
        cout << lokasi[rute[i]];

        if (i < rute.size() - 1) {
            cout << " -> ";
        }
    }
    cout << endl;
}

// 3. Menambahkan pelanggan baru ke graf berbobot
void tambahLokasi(
    vector<string>& lokasi,
    vector<vector<double>>& D,
    const string& namaBaru,
    const vector<double>& jarakBaru
) {
    int ukuranLama = D.size();

    if (jarakBaru.size() != ukuranLama + 1) {
        cout << "Data jarak tidak valid!" << endl;
        return;
    }

    lokasi.push_back(namaBaru);

    for (int i = 0; i < ukuranLama; i++) {
        D[i].push_back(jarakBaru[i]);
    }

    D.push_back(jarakBaru);
}

// 4. Algoritma Brute Force
vector<int> solveBruteForce(
    const vector<vector<double>>& D,
    long long& jumlahRute,
    bool tampilkanSemua
) {
    int n = D.size();

    // Membuat daftar pelanggan (tanpa gudang)
    vector<int> pelanggan(n - 1);
    iota(pelanggan.begin(), pelanggan.end(), 1);

    double jarakTerbaik = numeric_limits<double>::max();

    vector<int> ruteTerbaik;

    jumlahRute = 0;

    cout << "\nDaftar Seluruh Kemungkinan Rute:\n";

    do {
        jumlahRute++;

        // Rute dimulai dari gudang
        vector<int> rute = {0};

        rute.insert(
            rute.end(),
            pelanggan.begin(),
            pelanggan.end()
        );

        // Kembali ke gudang
        rute.push_back(0);

        // Menghitung total jarak
        double total = hitungTotalJarak(rute, D);

        if (tampilkanSemua) {
            cout << "Rute " << setw(6) << jumlahRute << " : ";

            cetakRute(rute, vector<string>{
                "Gudang", "Pel-A", "Pel-B", "Pel-C",
                "Pel-D", "Pel-E", "Pel-F", "Pel-G",
                "Pel-H", "Pel-I", "Pel-J"
            });

            cout << "Total Jarak: "
                 << fixed << setprecision(1)
                 << total << " km\n\n";
        }

        // Memperbarui rute terbaik
        if (total < jarakTerbaik) {
            jarakTerbaik = total;
            ruteTerbaik = rute;
        }

    } while (next_permutation(
        pelanggan.begin(),
        pelanggan.end()
    ));

    return ruteTerbaik;
}

// 5. Prosedur pengujian
void jalankanPengujian(
    const string& judul,
    const vector<string>& lokasi,
    const vector<vector<double>>& D,
    bool tampilkanSemua
) {
    long long jumlahRute = 0;

    auto mulai = chrono::high_resolution_clock::now();

    vector<int> ruteOptimal = solveBruteForce(
        D,
        jumlahRute,
        tampilkanSemua
    );

    auto selesai = chrono::high_resolution_clock::now();

    double waktuMs =
        chrono::duration<double, milli>(
            selesai - mulai
        ).count();

    double jarakOptimal =
        hitungTotalJarak(ruteOptimal, D);

    cout << "\n========================================\n";
    cout << judul << endl;
    cout << "========================================\n";

    cout << "Jumlah Lokasi          : "
         << lokasi.size() << endl;

    cout << "Jumlah Pelanggan       : "
         << lokasi.size() - 1 << endl;

    cout << "Jumlah Rute Dievaluasi : "
         << jumlahRute << endl;

    cout << "Rute Terbaik           : ";
    cetakRute(ruteOptimal, lokasi);

    cout << "Jarak Minimum          : "
         << fixed << setprecision(1)
         << jarakOptimal << " km" << endl;

    cout << "Waktu Komputasi        : "
         << fixed << setprecision(3)
         << waktuMs << " ms" << endl;
}

// 6. Program utama
int main() {

    // DATA AWAL: 1 GUDANG + 8 PELANGGAN

    vector<string> lokasi = {
        "Gudang", "Pel-A", "Pel-B", "Pel-C",
        "Pel-D", "Pel-E", "Pel-F", "Pel-G", "Pel-H"
    };

    // Graf berbobot direpresentasikan dengan matriks jarak
    vector<vector<double>> D = {
        {0.0, 6.3, 5.4, 10.6, 9.1, 13.0, 10.8, 13.0, 3.2},
        {6.3, 0.0, 5.0, 6.1, 8.6, 10.0, 4.5, 7.1, 3.2},
        {5.4, 5.0, 0.0, 5.8, 4.1, 7.6, 8.1, 9.2, 4.1},
        {10.6, 6.1, 5.8, 0.0, 6.1, 4.5, 5.0, 4.1, 8.1},
        {9.1, 8.6, 4.1, 6.1, 0.0, 5.0, 10.3, 10.2, 8.2},
        {13.0, 10.0, 7.6, 4.5, 5.0, 0.0, 9.4, 7.8, 11.2},
        {10.8, 4.5, 8.1, 5.0, 10.3, 9.4, 0.0, 3.2, 7.6},
        {13.0, 7.1, 9.2, 4.1, 10.2, 7.8, 3.2, 0.0, 10.0},
        {3.2, 3.2, 4.1, 8.1, 8.2, 11.2, 7.6, 10.0, 0.0}
    };

    // PERCOBAAN 1: 8 PELANGGAN
    jalankanPengujian(
        "PERCOBAAN 1 - 8 PELANGGAN",
        lokasi, D, false
    );

    // PERCOBAAN 2: MENAMBAH 1 PELANGGAN
    tambahLokasi(
        lokasi, D, "Pel-I",
        {13.5, 8.5, 8.6, 2.8, 8.1, 4.5, 6.1, 3.6, 10.8, 0.0}
    );

    jalankanPengujian(
        "PERCOBAAN 2 - 9 PELANGGAN",
        lokasi, D, false
    );

    // PERCOBAAN 3: MENAMBAH 1 PELANGGAN LAGI
    tambahLokasi(
        lokasi, D, "Pel-J",
        {7.8, 4.1, 3.2, 2.8, 5.0, 6.0, 5.4, 6.1, 5.4, 5.7, 0.0}
    );

    jalankanPengujian(
        "PERCOBAAN 3 - 10 PELANGGAN",
        lokasi, D, false
    );

    return 0;
}
