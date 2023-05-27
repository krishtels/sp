#include <Windows.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <thread>
#include <iostream>
#include <chrono> 
#define SIZE_BUFFER 100000

#define PATH L"NotSorted.txt"
#define PATH2 L"SortedSync.txt"
#define PATH3 L"SortedAsync.txt"

using namespace std;

struct Param {
    //std::vector<char>& arr;
    char* arr;
    int l;
    int r;
    int depth;
    bool multithread;
};

void ShowArray(char* arr, int size = SIZE_BUFFER) {
    for (int i = 0; i < size; i++) {
        cout << arr[i] << " ";
    }
    cout << endl;
}

int CreateFileWithValues() {
    //Открываем файл
    HANDLE hFile = CreateFile(PATH, // имя файла
        GENERIC_READ | GENERIC_WRITE, // режим доступа
        FILE_SHARE_READ, // совместный доступ
        NULL,
        OPEN_ALWAYS, // как действовать
        FILE_ATTRIBUTE_NORMAL, // атрибуты файла
        NULL);

    OVERLAPPED olf = { 0 }; //Структура, в которой задана позиция в файле
    //Аналог LARGE_INTEGER
    //hEvent - ставим NULL
    //Offset младший разряд того, куда поставить курсор. 32-бит
    //OffsetHigh - старший разряд того, куда поставить курсор. 32-бит
    //Internal - ставим NULL(0). Это - возвращаемое значение, куда записывается новая поз. курсора
    //32-бит, младший разряд
    //InternalHigh - тоже что и предыдущее, только старший разряд

    //Как объеденять и разъеденять:
    LARGE_INTEGER li = { 0 };
    //LowPart - младший 32-битный разряд
    //HighPart - старший 32-битный разряд
    //QuadPart - 64 битное число

    CHAR* str = (CHAR*)calloc(SIZE_BUFFER + 1, sizeof(CHAR));

    for (int i = 0; i < SIZE_BUFFER; i++) {
        str[i] = rand() % ('z' - 'a' + 1) + 'a';
    }

    DWORD iNumWrite = 0;
    if (!WriteFile(hFile, str, SIZE_BUFFER, &iNumWrite, &olf))
        return 3;
    if (olf.Internal == -1 && GetLastError())
        return 4;

    CloseHandle(hFile);

    return 0;
}

std::vector<int> tmp(5000000, 0);

DWORD WINAPI MergeSort(LPVOID lpParam) {
    HANDLE hStdout = NULL;

    hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hStdout == INVALID_HANDLE_VALUE) {
        return 1;
    }
    Param* p = (Param*)lpParam;
    if (p->depth > 3) p->multithread = false;
    if (p->l >= p->r) {
        return 0;
    }

    int md = (p->l + p->r) / 2;

    if (p->multithread) {
        Param f = { p->arr, p->l, md, p->depth + 1, p->multithread };
        Param s = { p->arr, md+1, p->r, p->depth + 1, p->multithread };
        HANDLE th_merge_sort_1 = CreateThread(NULL, 0, MergeSort, &f, 1, 0);
        HANDLE th_merge_sort_2 = CreateThread(NULL, 0, MergeSort, &s, 1, 0);
        WaitForSingleObject(th_merge_sort_1, INFINITE);
        WaitForSingleObject(th_merge_sort_2, INFINITE);
    }
    else {
        Param f = { p->arr, p->l, md, p->depth + 1, p->multithread };
        Param s = { p->arr, md + 1, p->r, p->depth + 1, p->multithread };
        MergeSort(&f);
        MergeSort(&s);
    }

    int i = p->l, j = md + 1;
    int k = p->l;

    while (i <= md && j <= p->r) {
        tmp[k++] = min(p->arr[i], p->arr[j]);
        if (p->arr[i] < p->arr[j]) {
            i++;
        }
        else {
            j++;
        }
    }
    while (i <= md) {
        tmp[k++] = p->arr[i++];
    }
    while (j <= p->r) {
        tmp[k++] = p->arr[j++];
    }
    for (int i = p->l; i <= p->r; i++) {
        p->arr[i] = tmp[i];
    }

    //  ShowArray(arr, p->size);
    return 0;
}

int SyncReadSortWrite() {
    HANDLE hFile = CreateFile(PATH,
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ,
        NULL,
        OPEN_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    OVERLAPPED olf = { 0 };

    auto startTime = chrono::high_resolution_clock::now();

    LPSTR buffer = (CHAR*)calloc(SIZE_BUFFER, sizeof(CHAR));
    DWORD iNumRead = 0;//Обязательный параметр. получает кол-во считанных байт

    if (!ReadFile(hFile, buffer, SIZE_BUFFER, &iNumRead, &olf))
        return 13;
    if (olf.Internal == -1 && GetLastError())
        return 2;

    olf.Offset += iNumRead;//добавляем кол-во прочитанных байт

    Param p = { buffer, 0, SIZE_BUFFER - 1, 1, false };

    HANDLE th_merge_sort = CreateThread(NULL, 0, MergeSort, &p, 1, 0);

    WaitForSingleObject(th_merge_sort, INFINITE);

    CloseHandle(th_merge_sort);
    CloseHandle(hFile);

    HANDLE hFile2 = CreateFile(PATH2,
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ,
        NULL,
        OPEN_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    LARGE_INTEGER li = { 0 };
    //LowPart - младший 32-битный разряд
    //HighPart - старший 32-битный разряд
    //QuadPart - 64 битное число
    olf = { 0 };

    DWORD iNumWrite = 0;
    if (!WriteFile(hFile2, buffer, SIZE_BUFFER, &iNumWrite, &olf))
        return 3;
    if (olf.Internal == -1 && GetLastError())
        return 4;


    CloseHandle(hFile2);

    chrono::nanoseconds duration1 = chrono::high_resolution_clock::now() - startTime;
    std::cout << "time for sort in sync read: " << duration1.count() << " nanoseconds.\n";

    return 0;
}

char* EndSorting(Param p1, Param p2, Param p3, Param p4) {
    size_t size = p1.r - p1.l + 1;
    size_t size4 = p4.r - p4.l + 1;
    int i1 = 0, i2 = 0, i3 = 0, i4 = 0;
    int i = 0;
    CHAR* result = (CHAR*)calloc(SIZE_BUFFER, sizeof(CHAR));

    while (i1 != size || i2 != size || i3 != size || i4 != size4) {
        char ch1 = 127, ch2 = 127, ch3 = 127, ch4 = 127;
        if (i1 <  size) {
            ch1 = p1.arr[i1];
        }
        if (i2 < size) {
            ch2 = p2.arr[i2];
        }
        if (i3 < size) {
            ch3 = p3.arr[i3];
        }
        if (i4 < size4) {
            ch4 = p4.arr[i4];
        }

        char min_val = min(min(ch1, ch2), min(ch3, ch4));
        if (min_val == ch1) {
            result[i] = p1.arr[i1];
            i1++; i++;
            continue;
        }
        if (min_val == ch2) {
            result[i] = p2.arr[i2];
            i2++; i++;
            continue;
        }
        if (min_val == ch3) {
            result[i] = p3.arr[i3];
            i3++; i++;
            continue;
        }
        if (min_val == ch4) {
            result[i] = p4.arr[i4];
            i4++; i++;
            continue;
        }
    }

    return result;
}

BOOL ReadFromFileAsync() {
    auto startTime = chrono::high_resolution_clock::now();

    BOOL bResult = FALSE;

    HANDLE hFile = CreateFile(PATH,
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ,
        NULL,
        OPEN_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if (hFile == INVALID_HANDLE_VALUE) {
        cout << "Error opening file" << endl;
        return FALSE;
    }

    DWORD dwLineSize = SIZE_BUFFER / 4; // size of each line, in bytes

    LPSTR bFirstLineBuf = (CHAR*)calloc(dwLineSize, sizeof(CHAR));
    LPSTR bSecondLineBuf = (CHAR*)calloc(dwLineSize, sizeof(CHAR));
    LPSTR bThirdLineBuf = (CHAR*)calloc(dwLineSize, sizeof(CHAR));
    LPSTR bFourthLineBuf = (CHAR*)calloc(dwLineSize, sizeof(CHAR));

    OVERLAPPED oReadFirstLine = { 0 };
    OVERLAPPED oReadSecondLine = { 0 };
    OVERLAPPED oReadThirdLine = { 0 };
    OVERLAPPED oReadFourthLine = { 0 };

    oReadFirstLine.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (!oReadFirstLine.hEvent) {
        cout << "Error creating I/O event for reading first line" << endl;
        return -1;
    }
    oReadFirstLine.Offset = 0; // offset of first line

    oReadSecondLine.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (!oReadSecondLine.hEvent) {
        cout << "Error creating I/O event for reading second line" << endl;
        return -1;
    }
    oReadSecondLine.Offset = dwLineSize; // offset of second line


    oReadThirdLine.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (!oReadThirdLine.hEvent) {
        cout << "Error creating I/O event for reading third line" << endl;
        return -1;
    }
    oReadThirdLine.Offset = dwLineSize * 2; // offset of third line

    oReadFourthLine.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (!oReadFourthLine.hEvent) {
        cout << "Error creating I/O event for reading forth line" << endl;
        return -1;
    }

    oReadFourthLine.Offset = dwLineSize * 3; // offset of fourth line

    DWORD iNumRead1 = 0;//Обязательный параметр. получает кол-во считанных байт
    DWORD iNumRead2 = 0;//Обязательный параметр. получает кол-во считанных байт
    DWORD iNumRead3 = 0;//Обязательный параметр. получает кол-во считанных байт
    DWORD iNumRead4 = 0;//Обязательный параметр. получает кол-во считанных байт
    
    if (!ReadFile(hFile, &bFirstLineBuf[0], dwLineSize, &iNumRead1, &oReadFirstLine)) {
        if (GetLastError() != ERROR_IO_PENDING) {
            cout << "Error starting I/O to read first line" << endl;
            return -1;
        }
    }

    if (!ReadFile(hFile, &bSecondLineBuf[0], dwLineSize, &iNumRead2, &oReadSecondLine)) {
        if (GetLastError() != ERROR_IO_PENDING) {
            cout << "Error starting I/O to read second line" << endl;
            return -1;
        }
    }

    if (!ReadFile(hFile, &bThirdLineBuf[0], dwLineSize, &iNumRead3, &oReadThirdLine)) {
        if (GetLastError() != ERROR_IO_PENDING) {
            cout << "Error starting I/O to read third line" << endl;
            return -1;
        }
    }

    if (!ReadFile(hFile, &bFourthLineBuf[0], SIZE_BUFFER - dwLineSize*3, &iNumRead4, &oReadFourthLine)) {
        if (GetLastError() != ERROR_IO_PENDING) {
            cout << "Error starting I/O to read forth line" << endl;
            CancelIo(hFile);
            return -1;
        }
    }
        // perform some stuff asynchronously  

    HANDLE hEvents[4];
    hEvents[0] = oReadFirstLine.hEvent;
    hEvents[1] = oReadSecondLine.hEvent;
    hEvents[2] = oReadThirdLine.hEvent;
    hEvents[3] = oReadFourthLine.hEvent;

    DWORD dwWaitRes = WaitForMultipleObjects(_countof(hEvents), hEvents, TRUE, INFINITE);

    if (dwWaitRes == WAIT_FAILED) {
        cout << "Error waiting for I/O to finish" << endl;
        CancelIo(hFile);
        return -2;
    }


    if (oReadFirstLine.hEvent) CloseHandle(oReadFirstLine.hEvent);
    if (oReadSecondLine.hEvent) CloseHandle(oReadSecondLine.hEvent);
    if (oReadThirdLine.hEvent) CloseHandle(oReadThirdLine.hEvent);
    if (oReadFourthLine.hEvent) CloseHandle(oReadFourthLine.hEvent);

    CloseHandle(hFile);
    HANDLE hEventsSorting[4];

    Param p1 = { bFirstLineBuf, 0, dwLineSize - 1, 1, false };
    Param p2 = { bSecondLineBuf, 0, dwLineSize - 1, 1, false };
    Param p3 = { bThirdLineBuf, 0, dwLineSize - 1, 1, false };
    Param p4 = { bFourthLineBuf, 0, SIZE_BUFFER - dwLineSize * 3 - 1, 1, false };
    

    hEventsSorting[0] = CreateThread(NULL, 0, MergeSort, &p1, 1, 0);
    hEventsSorting[1] = CreateThread(NULL, 0, MergeSort, &p2, 1, 0);
    hEventsSorting[2] = CreateThread(NULL, 0, MergeSort, &p3, 1, 0);
    hEventsSorting[3] = CreateThread(NULL, 0, MergeSort, &p4, 1, 0);

    DWORD results = WaitForMultipleObjects(_countof(hEventsSorting), hEventsSorting, TRUE, INFINITE);

    if (results == WAIT_FAILED) {
        cout << "Error waiting for I/O to finish" << endl;
        CancelIo(hFile);
        return -2;
    }


    if (hEventsSorting[0]) CloseHandle(hEventsSorting[0]);
    if (hEventsSorting[1]) CloseHandle(hEventsSorting[1]);
    if (hEventsSorting[2]) CloseHandle(hEventsSorting[2]);
    if (hEventsSorting[3]) CloseHandle(hEventsSorting[3]);

    char* res = EndSorting(p1, p2, p3, p4);
    
    HANDLE hFile2 = CreateFile(PATH3,
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ,
        NULL,
        OPEN_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    oReadFirstLine = { 0 };
    oReadSecondLine = { 0 };
    oReadSecondLine.Offset = dwLineSize;
    oReadThirdLine = { 0 };
    oReadThirdLine.Offset = dwLineSize * 2;
    oReadFourthLine = { 0 };
    oReadFourthLine.Offset = dwLineSize * 3;

    if (!WriteFile(hFile2, &res[0], dwLineSize, NULL, &oReadFirstLine)) {
        if (GetLastError() != ERROR_IO_PENDING) {
            cout << "Error starting I/O to read first line" << endl;
            return -1;
        }
    }

    if (!WriteFile(hFile2, &res[dwLineSize], dwLineSize, &iNumRead2, &oReadSecondLine)) {
        if (GetLastError() != ERROR_IO_PENDING) {
            cout << "Error starting I/O to read second line" << endl;
            return -1;
        }
    }

    if (!WriteFile(hFile2, &res[dwLineSize * 2], dwLineSize, &iNumRead3, &oReadThirdLine)) {
        if (GetLastError() != ERROR_IO_PENDING) {
            cout << "Error starting I/O to read third line" << endl;
            return -1;
        }
    }

    if (!WriteFile(hFile2, &res[dwLineSize * 3], dwLineSize, &iNumRead4, &oReadFourthLine)) {
        if (GetLastError() != ERROR_IO_PENDING) {
            cout << "Error starting I/O to read forth line" << endl;
            CancelIo(hFile);
            return -1;
        }
    }

    CloseHandle(hFile2);

    chrono::nanoseconds duration1 = chrono::high_resolution_clock::now() - startTime;
    std::cout << "time for sort in async read: " << duration1.count() << " nanoseconds.\n";

    return bResult;
}

int main(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
    srand(time(NULL));

    CreateFileWithValues();
    SyncReadSortWrite();
    ReadFromFileAsync();

    return 0;
}