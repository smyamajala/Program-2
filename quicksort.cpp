// Quicksort
// 
// Author: Rob Gysel
// ECS60, UC Davis
// Adapted from: Lysecky & Vahid "Data Structures Essentials", zyBooks

#include "quicksort.h"

void QuickSort(std::vector<int>* numbers, int& comp_count, int& mem_count) {
   QuickSortRecurse(numbers, 0, numbers->size() - 1, comp_count, mem_count);
}

void QuickSortRecurse(std::vector<int>* numbers, int i, int k, int& comp_count, int& mem_count) {
   int j = 0;
   
   /* Base case: If there are 1 or zero elements to sort,
    partition is already sorted */
   if (i >= k) {
      return;
   }
   
   /* Partition the data within the array. Value j returned
    from partitioning is location of last element in low partition. */
   j = Partition(numbers, i, k, comp_count, mem_count);
   
   /* Recursively sort low partition (i to j) and
    high partition (j + 1 to k) */
   QuickSortRecurse(numbers, i, j, comp_count, mem_count);
   QuickSortRecurse(numbers, j + 1, k, comp_count, mem_count);
   
   return;
}

int Partition(std::vector<int>* numbers, int i, int k, int& comp_count, int& mem_count) {
   int l = 0;
   int h = 0;
   int midpoint = 0;
   int pivot = 0;
   int temp = 0;
   bool done = false;
   
   /* Pick middle element as pivot */
   midpoint = i + (k - i) / 2;
   pivot = (*numbers)[midpoint];  // 1 memory access (read pivot)
   mem_count++;
   
   l = i;
   h = k;
   
   while (!done) {
      
      /* Increment l while numbers[l] < pivot */
      while ((*numbers)[l] < pivot) {
         comp_count++;  // 1 comparison in while condition
         mem_count++;   // 1 memory access (reading numbers[l])
         ++l;
      }
      // Count the final comparison that failed the while loop
      comp_count++;  // The comparison that made the while condition false
      mem_count++;   // The memory access for that final comparison
      
      /* Decrement h while pivot < numbers[h] */
      while (pivot < (*numbers)[h]) {
         comp_count++;  // 1 comparison in while condition  
         mem_count++;   // 1 memory access (reading numbers[h])
         --h;
      }
      // Count the final comparison that failed the while loop
      comp_count++;  // The comparison that made the while condition false
      mem_count++;   // The memory access for that final comparison
      
      /* If there are zero or one elements remaining,
       all numbers are partitioned. Return h */
      if (l >= h) {
         comp_count++;  // 1 comparison for the if condition
         done = true;
      }
      else {
         comp_count++;  // 1 comparison for the if condition (else case)
         
         /* Swap numbers[l] and numbers[h],
          update l and h */
         temp = (*numbers)[l];           // 1 memory access (read numbers[l])
         (*numbers)[l] = (*numbers)[h];  // 1 read + 1 write = 2 memory accesses
         (*numbers)[h] = temp;           // 1 write = 1 memory access
         mem_count += 4;  // Total for swap: 1 + 2 + 1 = 4 memory accesses
         
         ++l;
         --h;
      }
   }
   
   return h;
} 
