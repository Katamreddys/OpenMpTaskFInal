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

  void generateLCS(char* X, int m, char* Y, int n);
  void checkLCS(char* X, int m, char* Y, int n, int result);

#ifdef __cplusplus
}
#endif

int lcs_cal( char *X, int m, char *Y,  int n, int nbthreads ) 
{ 
  int** partial_arr = new int*[m+1];
  for (int i=0; i<=m; ++i) {
    partial_arr[i] = new int[n+1];
  }
  
  int i, j; 
  
  #pragma omp parallel for schedule(guided)
  for (i=0; i<=m; i++) 
  { 
    for (j=0; j<=n; j++) 
    { 
      if (i == 0 || j == 0) 
        partial_arr[i][j] = 0; 

      else if (X[i-1] == Y[j-1]) 
        partial_arr[i][j] = partial_arr[i-1][j-1] + 1; 

      else
        partial_arr[i][j] = std::max(partial_arr[i-1][j], partial_arr[i][j-1]); 
    } 
  } 

  int result = partial_arr[m][n];
  
  #pragma omp taskwait
  for (int i=0; i<=m; ++i) { 
    delete[] partial_arr[i];
  }
  delete[] partial_arr;
  
  return result; 
  
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

  if (argc < 4) { std::cerr<<"usage: "<<argv[0]<<" <m> <n> <nbthreads>"<<std::endl;
    return -1;
  }

  int m = atoi(argv[1]);
  int n = atoi(argv[2]);

  // get string data 
  char *X = new char[m];
  char *Y = new char[n];
  int nbthreads = atoi(argv[3]);
  omp_set_num_threads(nbthreads);
  generateLCS(X, m, Y, n);

  
  //insert LCS code here.
  int result = -1; // length of common subsequence
  auto timeStart = std::chrono::system_clock::now();
  result = lcs_cal( X, m, Y,  n, nbthreads );
  auto timeEnd = std::chrono::system_clock::now();
  std::chrono::duration<double> total_time = timeEnd-timeStart;
  checkLCS(X, m, Y, n, result);
  std::cerr<<total_time.count()<<std::endl;

  return 0;
}
