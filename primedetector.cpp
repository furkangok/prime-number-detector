#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include <algorithm>

using namespace std;

#define N 10000000
ofstream myFile;

int prime[N];
string RESULT_TABLE_COLUMNS = "M;Openmp Loop Scheduling Method;Chunk Size;T1;T2;T4;T8;S2;S4;S8\n";

double
findPrimes(omp_sched_t kind, int M, int chunk_size, int num_threads);
int writeToCsvFile(int M, string method, int chunk_size, int t1, int t2, int t4, int t8);
int printPrimes();
int j = 0;

int main(int argc, char *argv[])
{
    myFile.open("results.csv");
    myFile << RESULT_TABLE_COLUMNS;

    int M = atoi(argv[argc - 1]);
    int chunk_size = atoi(argv[argc - 2]);
    int t1, t2, t4, t8;

    t1 = 100000 * findPrimes(omp_sched_dynamic, M, chunk_size, 1);
    t2 = 100000 * findPrimes(omp_sched_dynamic, M, chunk_size, 2);
    t4 = 100000 * findPrimes(omp_sched_dynamic, M, chunk_size, 4);
    t8 = 100000 * findPrimes(omp_sched_dynamic, M, chunk_size, 8);
    writeToCsvFile(M, "dynamic", chunk_size, t1, t2, t4, t8);

    t1 = 100000 * findPrimes(omp_sched_static, M, chunk_size, 1);
    t2 = 100000 * findPrimes(omp_sched_static, M, chunk_size, 2);
    t4 = 100000 * findPrimes(omp_sched_static, M, chunk_size, 4);
    t8 = 100000 * findPrimes(omp_sched_static, M, chunk_size, 8);
    writeToCsvFile(M, "static", chunk_size, t1, t2, t4, t8);

    t1 = 100000 * findPrimes(omp_sched_guided, M, chunk_size, 1);
    t2 = 100000 * findPrimes(omp_sched_guided, M, chunk_size, 2);
    t4 = 100000 * findPrimes(omp_sched_guided, M, chunk_size, 4);
    t8 = 100000 * findPrimes(omp_sched_guided, M, chunk_size, 8);
    writeToCsvFile(M, "guided", chunk_size, t1, t2, t4, t8);

    //printPrimes();
    myFile.close();
}

int writeToCsvFile(int M, string method, int chunk_size, int t1, int t2, int t4, int t8)
{
    double baseTime = t1;
    string resultRow = to_string(M) + ";" + method + ";" + to_string(chunk_size) + ";" + to_string(t1) + ";" + to_string(t2) + ";" + to_string(t4) + ";" + to_string(t8) + ";" + to_string((baseTime / t2)) + ";" + to_string((baseTime / t4)) + ";" + to_string((baseTime / t8)) + ";\n";
    myFile << resultRow;
    return 0;
}

double findPrimes(omp_sched_t kind, int M, int chunk_size, int num_threads)
{
    int k, n, quo, rem;
    int serialPart = sqrt(M);

    time_t t;
    double t1, t2;

    if (M < 2)
    {
        return 0;
    }
    else if (M < 3)
    {
        prime[0] = 2;
    }
    else if (M == 3)
    {
        prime[0] = 2;
        prime[1] = 3;
    }
    else
    {
        prime[0] = 2;
        n = 3;
        j = 0;
        j = j + 1;
        prime[j] = n;
        k = 1;
        for (n = 5; n < serialPart; n += 2)
        {
            k = 1;
            bool found = false;
            while (prime[k] & !found)
            {

                quo = n / prime[k];
                rem = n % prime[k];
                if (rem != 0)
                {
                    if (quo <= prime[k])
                    {
                        j += 1;
                        prime[j] = n;
                        found = true;
                    }
                    else
                    {
                        k += 1;
                    }
                }
                else
                {
                    break;
                }
            }
        }
        if (serialPart % 2 == 0)
        {
            serialPart += 1;
        }
        bool found;

        omp_set_num_threads(num_threads);

        t1 = omp_get_wtime();
        omp_set_schedule(kind, chunk_size);
#pragma omp parallel shared(prime, j) private(n, k, quo, rem, found)
        {
#pragma omp for nowait
            for (n = serialPart; n < M - 1; n += 2)
            {

                k = 1;
                found = false;
                while (prime[k] & !found)
                {
                    quo = n / prime[k];
                    rem = n % prime[k];
                    if (rem != 0)
                    {
                        if (quo <= prime[k])
                        {
#pragma omp critical
                            {
                                j += 1;
                                prime[j] = n;
                                found = true;
                            }
                        }
                        else
                        {
                            k += 1;
                        }
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }
        t2 = omp_get_wtime();
    }

    return t2 - t1;
}

int printPrimes()
{
    sort(prime, prime + j + 1);

    for (int k = 0; k <= j; k++)
    {
        cout << prime[k] << endl;
    }

    cout << "TOTAL: " << j + 1 << endl;
    return 0;
}