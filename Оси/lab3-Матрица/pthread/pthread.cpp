#include "pthread.h"

using namespace std;
using namespace std::chrono;

int block_size = 16;
pthread_mutex_t result_mutex = PTHREAD_MUTEX_INITIALIZER;

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

void* calculateBlock(void* arg)
{
    ThreadData* data = static_cast<ThreadData*>(arg);

    const vector<vector<int>>& m1 = *(data->m1);
    const vector<vector<int>>& m2 = *(data->m2);
    vector<vector<int>>& result = *(data->result);
    int startRow = data->startRow;
    int endRow = data->endRow;
    int startCol = data->startCol;
    int endCol = data->endCol;
    int startK = data->startK;
    int endK = data->endK;

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

    pthread_mutex_lock(&result_mutex);
    for (int i = 0; i < rows_in_block; i++) {
        for (int j = 0; j < cols_in_block; j++) {
            result[startRow + i][startCol + j] += temp_matrix[i][j];
        }
    }
    pthread_mutex_unlock(&result_mutex);
    delete data;
    return nullptr;

}

int main(int argc, char* argv[])
{
    """Фан факт этот код примерно в 5 раз медленнее, чем на thread и windows.h
        Я думаю это связано с тем что я запускаю через wsl и мб есть какие-то приколы с доступным количеством потоков"""

    //vector <vector<int>> m1 = { {1, 2, 3, 4}, {2, 2, 1, 1}, {3, 2, 1, 2},  {2, 2, 1, 1} };
    //vector <vector<int>> m2 = { {2, 2, 3, 1}, {3, 2, 4, 2}, {1, 2, 3, 1},  {1, 2, 3, 4} };
    int n = 300;
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
    vector<pthread_t> threads;
    for (int i = 0; i < m1.size(); i += block_size) {
        for (int j = 0; j < m2[0].size(); j += block_size) {
            for (int k = 0; k < m2.size(); k += block_size) {
                int endRow = min(i + block_size, (int)m1.size());
                int endCol = min(j + block_size, (int)m2[0].size());
                int endK = min(k + block_size, (int)m2.size());

                ThreadData* data = new ThreadData{ &m1, &m2, &result, i, endRow, j, endCol, k, endK};

                pthread_t thread;
                pthread_create(&thread, nullptr, calculateBlock, data);
                threads.push_back(thread);
            }
        }
    }

    for (pthread_t thread : threads) {
        pthread_join(thread, nullptr);
    }

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);

    cout << "pthread: " << duration.count() << " ms" << endl;

    /*for (int i = 0; i < result.size(); i++) {
        for(int j = 0; j < result.size(); j++) {
            cout << result[i][j] << " ";
        }
        cout << endl;
    }*/

    return 0;
}
