#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>

using namespace std;
using namespace std::chrono;

int main()
{
    //vector <vector<int>> m1 = { {1, 2, 3, 4}, {2, 2, 1, 1}, {3, 2, 1, 2},  {2, 2, 1, 1} };
    //vector <vector<int>> m2 = { {2, 2, 3, 1}, {3, 2, 4, 2}, {1, 2, 3, 1},  {1, 2, 3, 4} };

    int n = 500;
    /*ifstream fin("../../input.txt");
    if (fin.is_open()) {
        fin >> n;
        fin.close();
    }*/
    int m = n;
    int rows1 = n;
    int cols1 = m;
    int rows2 = cols1;    
    int cols2 = m;

    vector <vector <int>> m1(rows1, vector<int>(cols1));
    vector <vector <int>> m2(rows2, vector<int>(cols2));

    for (int i = 0; i < m1.size(); i++)
        for (int j = 0; j < m1[i].size(); j++)
            m1[i][j] = rand() % 10000;

    for (int i = 0; i < m2.size(); i++)
        for (int j = 0; j < m2[i].size(); j++)
            m2[i][j] = rand() % 10000;
      
    auto start = high_resolution_clock::now();
    vector <vector <int>> result (m1.size(), vector<int>(m2[0].size()));
    for (int i = 0; i < m1.size(); i++) {
        for (int k = 0; k < m1.size(); k++){
            int temp = m1[i][k];
            for (int j = 0; j < m2[0].size(); j++) {
                result[i][j] += temp * m2[k][j];
            }
        }
    }
    /*
    btw вот этот код на 20% медленнее из-за смешной локальности данных
    for (int i = 0; i < m1.size(); i++) {
        for (int j = 0; j < m2[0].size(); j++) {
            for (int k = 0; k < m1.size(); k++) {
                result[i][j] += m1[i][k] * m2[k][j];
            }
        }
    }
    */
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);

    cout << "Basic: " << duration.count() << " ms" << endl;
    ofstream fout("../../outputBasic.txt", ios::app);
    if (fout.is_open()) {
        fout << n << " " << duration.count() << " " << endl;
        fout.close();
    }
    /*for (int i = 0; i < result.size(); i++) {
        for(int j = 0; j < result.size(); j++) {
            cout << result[i][j] << " ";
        }
        cout << endl;
    }*/
}
