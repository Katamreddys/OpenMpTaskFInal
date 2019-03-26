#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>
#include <omp.h>
#include <chrono>

#ifdef __cplusplus
extern "C" {
#endif

  void generateMergeSortData (int* arr, size_t n);
  void checkMergeSortResult (const int* arr, size_t n);

#ifdef __cplusplus
}
#endif


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
  
  int nbthreads = atoi(argv[2]);
  omp_set_num_threads(nbthreads);
  int granularity = n/nbthreads;
  // get arr data
  int * arr = new int [n];
  generateMergeSortData (arr, n);

  //insert sorting code here.
  auto timeStart = std::chrono::system_clock::now();
  for(  int i = 0;  i < n;  i++ )
  {       
    int present_ele = i % 2;     
    #pragma omp parallel for schedule(guided,granularity) ,shared(arr,present_ele, n)
    for(  int j = present_ele;  j < n-1;  j += 2  )
    {       
      if(  arr[ j ]  >  arr[ j+1 ]  )
      {       
        std::swap(  arr[ j ],  arr[ j+1 ]  );
        
      }       
    }       
  }
  auto timeEnd = std::chrono::system_clock::now();
  std::chrono::duration<double> total_time = timeEnd-timeStart;
  
  checkMergeSortResult (arr, n);
  std::cerr<<total_time.count()<<std::endl;
  
  delete[] arr;

  return 0;
}
