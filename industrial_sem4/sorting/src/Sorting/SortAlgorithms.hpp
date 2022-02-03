#ifndef SORT_ALGORITHMS_HPP
#define SORT_ALGORITHMS_HPP

template<typename T>
static int partition(T *data, int elemCnt);

template<typename T>
void bubbleSort(T *data, int elemCnt)
{
    for (int i = 0; i < elemCnt; i++)
    {
        for (int j = 0; j < elemCnt - i - 1; j++)
        {
            if (data[j + 1] < data[j])
            {
                T tmp = data[j + 1];
                data[j + 1] = data[j];
                data[j] = tmp;
            }
        }
    }
}

template<typename T>
void selectionSort(T *data, int elemCnt)
{
    for (int i = 0; i < elemCnt; i++)
    {
        int minIdx = i;
        for (int j = i + 1; j < elemCnt; j++)
        {
            if (data[j] < data[minIdx])
                minIdx = j;
        }

        T tmp = data[i];
        data[i] = data[minIdx];
        data[minIdx] = tmp;
    }
}

template<typename T>
void quickSort(T *data, int elemCnt)
{
    if (elemCnt < 2)
        return;

    int pivotIdx = partition(data, elemCnt);
    quickSort(data, pivotIdx + 1);
    quickSort(&data[pivotIdx + 1], elemCnt - pivotIdx - 1);
}

template<typename T>
static int partition(T *data, int elemCnt)
{
    T pivot = data[rand() % elemCnt];

    int l = 0, r = elemCnt - 1;
    while (true)
    {
        while (l < elemCnt && data[l] < pivot)
            l++;
        if (l == elemCnt)
            l--;

        while (r >= 0 && pivot < data[r])
            r--;
        if (r == -1)
            r++;

        if (l >= r)
            return r;

        T tmp = data[l];
        data[l] = data[r];
        data[r] = tmp;

        l++;
        r--;
    }   
}

#endif