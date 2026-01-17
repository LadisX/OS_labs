#include "thread.h"

using namespace std;
using namespace std::chrono;

int block_size = 2;
mutex result_mutex;

void calculateBlock(const vector<vector<int>>& m1,
    const vector<vector<int>>& m2,
    vector<vector<int>>& result,
    int startRow,
    int endRow,
    int startCol,
    int endCol,
    int startK,
    int endK)
{
    int rows_in_block = endRow - startRow;
    int cols_in_block = endCol - startCol;
    vector<vector<int>> temp_matrix(rows_in_block, vector<int>(cols_in_block));

    for (int i = startRow; i < endRow; ++i) {
        for (int k = startK; k < endK; ++k) {
            int temp = m1[i][k];
            for (int j = startCol; j < endCol; ++j) {
                temp_matrix[i - startRow][j - startCol] += temp * m2[k][j];
            }
        }
    }

    lock_guard<mutex> lock(result_mutex);
    for (int i = 0; i < rows_in_block; i++) {
        for (int j = 0; j < cols_in_block; j++) {
            result[startRow + i][startCol + j] += temp_matrix[i][j];
        }
    }
}

int main(int argc, char* argv[])
{
    //vector <vector<int>> m1 = { {1, 2, 3, 4}, {2, 2, 1, 1}, {3, 2, 1, 2},  {2, 2, 1, 1} };
    //vector <vector<int>> m2 = { {2, 2, 3, 1}, {3, 2, 4, 2}, {1, 2, 3, 1},  {1, 2, 3, 4} };
     int n;
     if (argc >= 2) {
         n = stoi(argv[1]);
     }
     if (argc >= 3) {
         block_size = stoi(argv[2]);
     }
     bool foundInput = 0;
     ifstream fin("../../../../../input.txt");
     if (fin.is_open()) {
         fin >> n;
         fin.close();
         foundInput = 1;
     }
     else
         cout << "Error not found input";
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
    vector <vector <int>> result(m1.size(), vector<int>(m2[0].size()));
    vector<thread> threads;
    for (int i = 0; i < m1.size(); i += block_size) {
        for (int j = 0; j < m2[0].size(); j += block_size) {
            for (int k = 0; k < m2.size(); k += block_size) {
                int endRow = min(i + block_size, (int)m1.size());
                int endCol = min(j + block_size, (int)m2[0].size());
                int endK = min(k + block_size, (int)m2.size());

                threads.emplace_back(calculateBlock, cref(m1), cref(m2), ref(result),
                    i, endRow, j, endCol, k, endK);
            }
        }
    }
    for (auto& t : threads) {
        t.join();
    }
    
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);

    cout << "thread: " << duration.count() << " ms" << endl;
     ofstream fout("../../../../../outputThread.txt", ios::app);
     if (fout.is_open() && foundInput) {
         fout << block_size << " " << n <<  " " << duration.count() << " " << endl;
         fout.close();
     }
    //for (int i = 0; i < result.size(); i++) {
    //    for(int j = 0; j < result.size(); j++) {
    //        cout << result[i][j] << " ";
    //    }
    //    cout << endl;
    //}
}
