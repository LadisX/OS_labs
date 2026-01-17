#include <iostream>
#include <windows.h>
#include <vector>
#include <chrono>
#include <fstream>
#include <string>

using namespace std;
using namespace std::chrono;

int block_size = 2;
CRITICAL_SECTION cs;

struct ThreadData {
    const vector<vector<int>>* m1;
    const vector<vector<int>>* m2;
    vector<vector<int>>* result;
    int startRow;
    int endRow;
    int startCol;
    int endCol;
    int startK;
    int endK;
};

DWORD WINAPI calculateBlock(LPVOID arg) {
    ThreadData* data = (ThreadData*)arg;

    int rows_in_block = data->endRow - data->startRow;
    int cols_in_block = data->endCol - data->startCol;
    vector<vector<int>> temp_matrix(rows_in_block, vector<int>(cols_in_block));

    for (int i = data->startRow; i < data->endRow; ++i) {
        for (int k = data->startK; k < data->endK; ++k) {
            int temp = (*data->m1)[i][k];
            for (int j = data->startCol; j < data->endCol; ++j) {
                temp_matrix[i - data->startRow][j - data->startCol] += temp * (*data->m2)[k][j];
            }
        }
    }

    EnterCriticalSection(&cs);
    for (int i = 0; i < rows_in_block; i++) {
        for (int j = 0; j < cols_in_block; j++) {
            (*data->result)[data->startRow + i][data->startCol + j] += temp_matrix[i][j];
        }
    }
    LeaveCriticalSection(&cs);

    delete data;
    return 0;
}

int main(int argc, char* argv[])
{
    int n = 100;
    //vector <vector<int>> m1 = { {1, 2, 3, 4}, {2, 2, 1, 1}, {3, 2, 1, 2},  {2, 2, 1, 1} };
    //vector <vector<int>> m2 = { {2, 2, 3, 1}, {3, 2, 4, 2}, {1, 2, 3, 1},  {1, 2, 3, 4} };
    if (argc >= 2)
        n = stoi(argv[1]);
    if (argc >= 3)
        block_size = stoi(argv[2]);
    
    vector <vector <int>> m1(n, vector<int>(n));
    vector <vector <int>> m2(n, vector<int>(n));

    for (int i = 0; i < m1.size(); i++)
        for (int j = 0; j < m1[i].size(); j++)
            m1[i][j] = rand() % 10000;

    for (int i = 0; i < m2.size(); i++)
        for (int j = 0; j < m2[i].size(); j++)
            m2[i][j] = rand() % 10000;

    auto start = high_resolution_clock::now();
    vector <vector <int>> result(m1.size(), vector<int>(m2[0].size()));

    InitializeCriticalSection(&cs);
    vector<HANDLE> threads;
    for (int i = 0; i < m1.size(); i += block_size) {
        for (int j = 0; j < m2[0].size(); j += block_size) {
            for (int k = 0; k < m2.size(); k += block_size) {
                int endRow = min(i + block_size, (int)m1.size());
                int endCol = min(j + block_size, (int)m2[0].size());
                int endK = min(k + block_size, (int)m2.size());
                ThreadData* data = new ThreadData{ &m1, &m2, &result, i, endRow, j, endCol, k, endK};
                HANDLE thread = CreateThread(nullptr, 0, calculateBlock, data, 0, nullptr);
                threads.push_back(thread);
            }
        }
    }

    const int very_cool_constant = 64;

    for (size_t i = 0; i < threads.size(); i += very_cool_constant) {
        int count = min(very_cool_constant, (int)threads.size() - i);
        WaitForMultipleObjects(count, &threads[i], TRUE, INFINITE);
    }

    for (auto thread : threads) {
        CloseHandle(thread);
    }

    DeleteCriticalSection(&cs);

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);

    cout << "windows.h: " << duration.count() << " ms" << endl;
    //for (int i = 0; i < result.size(); i++) {
//    for(int j = 0; j < result.size(); j++) {
//        cout << result[i][j] << " ";
//    }
//    cout << endl;
//}
}
