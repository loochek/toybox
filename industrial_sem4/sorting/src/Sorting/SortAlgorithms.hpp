#ifndef SORT_ALGORITHMS_HPP
#define SORT_ALGORITHMS_HPP

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

#endif