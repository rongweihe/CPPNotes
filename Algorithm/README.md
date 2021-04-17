

## 经典排序算法



### 冒泡排序

```c++
#include <bits/stdc++.h>

using namespace std;

void Bubble_sort(int *a,int n)
{
    int i,j;
    bool flag= true;
    for(int i=0; i<n&&flag; ++i)
    {
        flag=false;
        for(int j=n-2; j>=i; --j)
        {
            if(a[j] >a[j+1])
            {
                swap(a[j],a[j+1]);
                flag= true;
            }
        }
    }
}
int main()
{
    int a[6]={4,7,23,56,8,100};
    Bubble_sort(a,6);
    for(int i=0; i<6; ++i) cout<<a[i]<<endl;
}
```



### 归并排序

```c++
/*
【思路】：
解决了上面的合并有序数列问题，再来看归并排序，其的基本思路就是将数组分成二组A，B，如果这二组组内的数据都是有序的，那么就可以很方便的将这二组数据进行排序。如何让这二组组内数据有序了？

可以将A，B组各自再分成二组。依次类推，当分出来的小组只有一个数据时，可以认为这个小组组内已经达到了有序，然后再合并相邻的二个小组就可以了。这样通过先递归的分解数列，再合并数列就完成了归并排序。
*/

#include <bits/stdc++.h>

using namespace std;

///将有二个有序数列a[first...mid]和a[mid...last]合并。
void merge_arr(int *a,int L,int M,int R,int *tp) {
    int i=L,j=M+1;
    int k=L;
    while(i<=M&& j<=R) {
        if(a[i]<=a[j]) tp[k++]=a[i++];
        else tp[k++]=a[j++];
    }
    while(i<=M) tp[k++]=a[i++];
    while(j<=R) tp[k++]=a[j++];
    for(i=L; i<=R; ++i)
        a[i]=tp[i];
}
/**/
void merge_sort(int *a,int L,int R,int *tp) {
    int mid;
    if(L<R) {
        mid= (L+R)/2;
        merge_sort(a,L,mid,tp); ///左边有序
        merge_sort(a,mid+1,R,tp);///右边有序
        merge_arr(a,L,mid,R,tp);///再将二个有序数列合并
    }
}
int main() {
    int a[6]={4,7,23,56,8,100};
    int tp[6];
    merge_sort(a,0,5,tp);
    for(int i=0; i<6; ++i) cout<<a[i]<<endl;
}
```



### 快速排序

```c++
#include <bits/stdc++.h>
using namespace std;
int Partition(int *a,int L,int R)
{
    int piv = a[L];
    int tmp= piv;
    while(L<R)
    {
        while(L<R && a[R]>=piv) --R;
        a[L]=a[R];
        while(L<R && a[L]<=piv) ++L;
        a[R]=a[L];
    }
    a[L]=tmp;
    return L;
}
void quick_sort(int *a,int L,int R)
{
    int pivot;
    if(L<R)
    {
        pivot = Partition(a,L,R);
        quick_sort(a,L,pivot-1);
        quick_sort(a,pivot+1,R);
    }
}
int main()
{
    int a[]={4,7,23,56,8,100};
    quick_sort(a,0,6);
    for(int i=0; i<6; ++i) cout<<a[i]<<endl;
}
```

