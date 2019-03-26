#include <omp.h>
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include<chrono>
#include <unistd.h>
#include <math.h>


#ifdef __cplusplus
extern "C" {
#endif

  void generateReduceData (int* arr, size_t n);

#ifdef __cplusplus
}
#endif

int total_arr_sum;

void reduce_sum_calculation(int *arr, int graularity, int n){
  #pragma omp parallel
  {
    int arr_thread_sum;
    #pragma omp single 
    {
      for(int i=0; i<n; i+=graularity){
        int loop_start = i;
        int loop_end = loop_start+graularity;
        if(loop_end>n){
          loop_end = n;
        }
        #pragma omp task
        {
          int arr_thread_sum;
          for(int j=loop_start;j<loop_end;j++){
            arr_thread_sum+=arr[j];
          }
        }
        #pragma omp critical
        {
          total_arr_sum+=arr_thread_sum;
        }
      }
    }
  }
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
  
  if (argc < 3) {
    std::cerr<<"usage: "<<argv[0]<<" <n> <nbthreads>"<<std::endl;
    return -1;
  }


  int n = atoi(argv[1]);
  
  
  
  total_arr_sum = 0;
  int * arr = new int [n];
  int nbthreads= atoi(argv[2]);
  int graularity = n/nbthreads;
  omp_set_num_threads(nbthreads);
  generateReduceData (arr, atoi(argv[1]));

  //insert reduction code here
  auto timeStart = std::chrono::system_clock::now();
  reduce_sum_calculation(arr,graularity,n);
  auto timeEnd = std::chrono::system_clock::now();
  std::chrono::duration<double> total_time = timeEnd-timeStart;

  std::cout<<total_arr_sum<<std::endl;

  std::cerr<<total_time.count()<<std::endl;
    

  delete[] arr;

  return 0;
}

  
  
  
