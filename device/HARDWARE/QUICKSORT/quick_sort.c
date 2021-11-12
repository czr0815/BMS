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
        return; // 避免len等於負值時引發段錯誤（Segment Fault）
    // r[]模擬列表,p為數量,r[p++]為push,r[--p]為pop且取得元素
    r[p++] = new_Range(0, len - 1);
    while (p) {
        range = r[--p];
        if (range.start >= range.end)
            continue;
        mid = arr[(range.start + range.end) / 2].vol; // 選取中間點為基準點
        left = range.start, right = range.end;
        do
        {
            while (arr[left].vol < mid) ++left;   // 檢測基準點左側是否符合要求
            while (arr[right].vol > mid) --right; //檢測基準點右側是否符合要求
 
            if (left <= right)
            {
                swap(&arr[left],&arr[right]);
                left++;right--;               // 移動指針以繼續
            }
        } while (left <= right);
 
        if (range.start < right) r[p++] = new_Range(range.start, right);
        if (range.end > left) r[p++] = new_Range(left, range.end);
    }
}
