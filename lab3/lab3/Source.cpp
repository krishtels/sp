#include <Windows.h>
#include <vector>
#include <thread>
#include <iostream>
#include <chrono> 

using namespace std;

std::vector<int> tmp(5000000, 0);

void MergeSort(std::vector<int>& arr, int l, int r, int depth, bool multithread = false) {
    if (depth > 3) multithread = false;
    if (l >= r) {
        return;
    }

    int md = (l + r) / 2;

    if (multithread) {
        thread leftThread = thread(MergeSort, std::ref(arr), l, md, depth + 1, multithread);
        thread rightThread = thread(MergeSort, std::ref(arr), md + 1, r, depth + 1, multithread);
        leftThread.join();
        rightThread.join();
    }
    else {
        MergeSort(arr, l, md, depth + 1, multithread);
        MergeSort(arr, md + 1, r, depth + 1, multithread);
    }

    int i = l, j = md + 1;
    int k = l;
    
    while (i <= md && j <= r) {
        tmp[k++] = min(arr[i], arr[j]);
        if (arr[i] < arr[j]) {
            i++;
        }
        else {
            j++;
        }
    }
    while (i <= md) {
        tmp[k++] = arr[i++];
    }
    while (j <= r) {
        tmp[k++] = arr[j++];
    }
    for (int i = l; i <= r; i++) {
        arr[i] = tmp[i];
    }
}


int main()
{
    int j = 2;
    int counter = 0;
    for (int i = 1; i < 100; i = i + 2)
    {
        counter++;
        j--;
        while (j < 15)
        {
            j = j + 5;
        }
    }
    cout << counter;
    /*
    srand(time(NULL));
    int arrSize = 2000000;
    vector<int> unsorted(arrSize, 0);
    // «аполн€ем массив случайными значени€ми
    for (int i = 0; i < arrSize; i++) {
        unsorted[i] = rand() % 100;
        //cout << unsorted[i] << " ";
    }

    std::vector<int> unsorted2 = unsorted;

    auto startTime = chrono::high_resolution_clock::now();
    MergeSort(unsorted, 0, unsorted.size() - 1, 1, false);
    chrono::nanoseconds duration1 = chrono::high_resolution_clock::now() - startTime;
    cout << "Sorted single thread" << endl;
    cout << "time for sort in single  thread: " << duration1.count() << " nanoseconds.\n";
    startTime = chrono::high_resolution_clock::now();
    MergeSort(unsorted2, 0, unsorted2.size() - 1, 1, true);
    chrono::nanoseconds duration2 = chrono::high_resolution_clock::now() - startTime;
    cout << "Sorted multi thread" << endl;
    cout << "time for sort in multi  thread: " << duration2.count() << " nanoseconds.\n";
    cout << "time difference: " << duration1.count() - duration2.count() << " nanoseconds.\n";

    return 0;
    */
}