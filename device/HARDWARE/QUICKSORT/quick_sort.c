#include "quick_sort.h"

typedef struct _Range {
    int start, end;
} Range;

Range new_Range(int s, int e) {
    Range r;
    r.start = s;
    r.end = e;
    return r;
}
void swap(Cell *x, Cell *y) {
  Cell t = *x;
  *x = *y;
  *y = t;
}

void quick_sort(Cell arr[], const int len) {
  int p = 0, mid, left, right;
  Range r[BMU_LIST_NUM * BMU_VOL_NUM], range;
    if (len <= 0)
        return; // ����len���ֵؓ�r���l���e�`��Segment Fault��
    // r[]ģ�M�б�,p�锵��,r[p++]��push,r[--p]��pop��ȡ��Ԫ��
    r[p++] = new_Range(0, len - 1);
    while (p) {
        range = r[--p];
        if (range.start >= range.end)
            continue;
        mid = arr[(range.start + range.end) / 2].vol; // �xȡ���g�c������c
        left = range.start, right = range.end;
        do
        {
            while (arr[left].vol < mid) ++left;   // �z�y�����c����Ƿ����Ҫ��
            while (arr[right].vol > mid) --right; //�z�y�����c�҂��Ƿ����Ҫ��
 
            if (left <= right)
            {
                swap(&arr[left],&arr[right]);
                left++;right--;               // �Ƅ�ָ����^�m
            }
        } while (left <= right);
 
        if (range.start < right) r[p++] = new_Range(range.start, right);
        if (range.end > left) r[p++] = new_Range(left, range.end);
    }
}
