#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>
#include <omp.h>
#include <string.h>
#include <stdlib.h>
#include <chrono>

#ifdef __cplusplus
extern "C" {
#endif

  void generateMergeSortData (int* arr, size_t n);
  void checkMergeSortResult (int* arr, size_t n);

#ifdef __cplusplus
}
#endif


void merge(int * arr, int begin, int middle, int end,int * work_arr) 
{ 
  int i, j, k; 
  for(i=begin,j=middle+1,k=begin;i<=middle &&j<=end;)
  {
    if(arr[i] <arr[j]){
      work_arr[k] = arr[i];
      i++; k++;
    }
    else
    {
      work_arr[k] = arr[j];
      j++; k++;
     }           
  }
  if(i <= middle)
  {
   for(;i<=middle;i++,k++)
    work_arr[k] = arr[i];
  }
  if(j <= end)
  {
   for(;j<=end;j++,k++)
    work_arr[k] = arr[j];
  }
  for(i = begin;i<=end;i++)
      arr[i] = work_arr[i];
} 

int granularity;

void mergesort(int * arr, int begin, int end,int * work_arr) 
{
  if( begin >= end)
    return;
  int middle = (begin+end)/2;
  #pragma omp task untied firstprivate(arr,work_arr,begin,middle,granularity)
    mergesort(arr,begin,middle,work_arr);
  #pragma omp task untied firstprivate(arr,work_arr,end,middle,granularity)
    mergesort(arr,middle+1,end,work_arr);
  #pragma omp taskwait
  merge(arr,begin,middle,end,work_arr);
}


int main (int argc, char* argv[]) {
  //forces openmp to create the threads beforehand
  #pragma omp parallel
  {
    int fd = open (argv[0], O_RDONLY);
    if (fd != -1) {
      close (fd);
    }
    else {
      std::cerr<<"something is amiss"<<std::endl;
    }
  }
  
  if (argc < 3) { std::cerr<<"usage: "<<argv[0]<<" <n> <nbthreads>"<<std::endl;
    return -1;
  }

  int n = atoi(argv[1]);
  
  
  // get arr data
  int * arr = new int [n];
  int * work_arr = new int [n];
  generateMergeSortData (arr, n);


  int nbthreads= atoi(argv[2]);
  granularity = n/nbthreads;
  omp_set_num_threads(nbthreads);

  
  auto timeStart = std::chrono::system_clock::now();
    #pragma omp parallel
    { 
      #pragma omp single
      {
        mergesort(arr,0,n-1,work_arr);
      }  
    }
  auto timeEnd = std::chrono::system_clock::now();
  std::chrono::duration<double> total_time = timeEnd-timeStart;
   
  checkMergeSortResult (arr, n);
  std::cerr<<total_time.count()<<std::endl;
  
  delete[] arr;
  delete[] work_arr;

  return 0;
}
