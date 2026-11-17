
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <limits>
#include <chrono>
#include <iomanip>

using namespace std;

// 1. Menghitung total jarak rute tertutup
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

// 2. Menampilkan urutan rute
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

// 4. Algoritma Nearest Neighbor + 2-Opt
vector<int> solveNearestNeighbor2Opt(
    const vector<vector<double>>& D,
    vector<int>& ruteNN
) {
    int n = D.size();

    vector<bool> dikunjungi(n, false);

    ruteNN.clear();
    ruteNN.reserve(n + 1);

    // TAHAP 1: NEAREST NEIGHBOR

    int sekarang = 0;

    ruteNN.push_back(sekarang);
    dikunjungi[sekarang] = true;

    // Memilih pelanggan terdekat yang belum dikunjungi
    for (int step = 1; step < n; step++) {

        int terdekat = -1;

        double minJarak =
            numeric_limits<double>::max();

        for (int kandidat = 1; kandidat < n; kandidat++) {

            if (!dikunjungi[kandidat] &&
                D[sekarang][kandidat] < minJarak) {

                minJarak = D[sekarang][kandidat];
                terdekat = kandidat;
            }
        }

        dikunjungi[terdekat] = true;

        ruteNN.push_back(terdekat);

        sekarang = terdekat;
    }

    // Kembali ke gudang
    ruteNN.push_back(0);

    // TAHAP 2: OPTIMASI 2-OPT

    vector<int> rute2Opt = ruteNN;

    int m = rute2Opt.size();

    bool adaPerbaikan = true;

    while (adaPerbaikan) {

        adaPerbaikan = false;

        for (int i = 1; i < m - 2; i++) {

            for (int k = i + 1; k < m - 1; k++) {

                double jarakLama =
                    D[rute2Opt[i - 1]][rute2Opt[i]] +
                    D[rute2Opt[k]][rute2Opt[k + 1]];

                double jarakBaru =
                    D[rute2Opt[i - 1]][rute2Opt[k]] +
                    D[rute2Opt[i]][rute2Opt[k + 1]];

                // Membalik urutan jika menghasilkan jarak lebih pendek
                if (jarakBaru + 1e-9 < jarakLama) {

                    reverse(
                        rute2Opt.begin() + i,
                        rute2Opt.begin() + k + 1
                    );

                    adaPerbaikan = true;
                }
            }
        }
    }

    return rute2Opt;
}

// 5. Prosedur pengujian dan benchmark
void jalankanPengujian(
    const string& judul,
    const vector<string>& lokasi,
    const vector<vector<double>>& D,
    int jumlahUji
) {
    vector<int> ruteNN;
    vector<int> rute2Opt;

    // Warm-up untuk menstabilkan pengukuran
    for (int w = 0; w < 50; w++) {
        rute2Opt = solveNearestNeighbor2Opt(D, ruteNN);
    }

    // Mengukur waktu komputasi
    auto mulai = chrono::steady_clock::now();

    for (int uji = 0; uji < jumlahUji; uji++) {
        rute2Opt = solveNearestNeighbor2Opt(D, ruteNN);
    }

    auto selesai = chrono::steady_clock::now();

    double totalUs =
        chrono::duration<double, micro>(
            selesai - mulai
        ).count();

    double rataRataUs = totalUs / jumlahUji;

    // Menghitung jarak kedua rute
    double jarakNN = hitungTotalJarak(ruteNN, D);

    double jarak2Opt = hitungTotalJarak(rute2Opt, D);

    double penghematan = jarakNN - jarak2Opt;

    double persentase = 0.0;

    if (jarakNN > 0) {
        persentase = (penghematan / jarakNN) * 100.0;
    }

    // Menampilkan hasil
    cout << "\n========================================\n";
    cout << judul << endl;
    cout << "========================================\n";

    cout << "Jumlah Lokasi       : "
         << lokasi.size() << endl;

    cout << "Jumlah Pelanggan    : "
         << lokasi.size() - 1 << endl;

    cout << "\nHASIL NEAREST NEIGHBOR\n";

    cout << "Rute NN             : ";
    cetakRute(ruteNN, lokasi);

    cout << fixed << setprecision(1);

    cout << "Total Jarak NN      : "
         << jarakNN << " km\n";

    cout << "\nHASIL OPTIMASI 2-OPT\n";

    cout << "Rute Setelah 2-Opt  : ";
    cetakRute(rute2Opt, lokasi);

    cout << "Jarak Setelah 2-Opt : "
         << jarak2Opt << " km\n";

    cout << "\nANALISIS PERBAIKAN\n";

    cout << "Penghematan Jarak   : "
         << penghematan << " km\n";

    cout << "Persentase Perbaikan: "
         << fixed << setprecision(2)
         << persentase << " %\n";

    cout << "\nPENGUKURAN WAKTU\n";

    cout << "Jumlah Pengujian    : "
         << jumlahUji << " kali\n";

    cout << fixed << setprecision(3);

    cout << "Rata-rata Waktu     : "
         << rataRataUs << " mikrodetik\n";

    cout << "Dalam Milidetik     : "
         << rataRataUs / 1000.0 << " ms\n";
}

// 6. Program utama
int main() {

    // PERCOBAAN 1: 1 GUDANG + 8 PELANGGAN

    vector<string> lokasi = {
        "Gudang", "Pel-A", "Pel-B", "Pel-C",
        "Pel-D", "Pel-E", "Pel-F", "Pel-G", "Pel-H"
    };

    // Graf berbobot menggunakan matriks jarak
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
        lokasi, D, 1000
    );

    // PERCOBAAN 2: MENAMBAH 1 PELANGGAN
    tambahLokasi(
        lokasi, D, "Pel-I",
        {13.5, 8.5, 8.6, 2.8, 8.1, 4.5, 6.1, 3.6, 10.8, 0.0}
    );

    jalankanPengujian(
        "PERCOBAAN 2 - 9 PELANGGAN",
        lokasi, D, 1000
    );

    // PERCOBAAN 3: MENAMBAH 1 PELANGGAN LAGI
    tambahLokasi(
        lokasi, D, "Pel-J",
        {7.8, 4.1, 3.2, 2.8, 5.0, 6.0, 5.4, 6.1, 5.4, 5.7, 0.0}
    );

    jalankanPengujian(
        "PERCOBAAN 3 - 10 PELANGGAN",
        lokasi, D, 1000
    );

    return 0;
}
