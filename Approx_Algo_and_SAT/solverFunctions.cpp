#pragma once

#include "ece650-prj.hpp"

// We need Void functions for this since pthreads only accept them as arguments
// WE NEED TO DELETE THE DYNAMICALLY ALLOCATED MEMORY
struct VertexCoverResult
{
  vector<int> *result;
  double elapsedTime;
};

void *vertexCoverWrapper(void *arg)
{
  Graph *graph = static_cast<Graph *>(arg);
  clockid_t cpuclock;
  if (pthread_getcpuclockid(pthread_self(), &cpuclock) != 0)
  {
    std::cerr << "Failed to get CPU clock ID." << std::endl;
    return nullptr;
  }

  struct timespec start, end;
  clock_gettime(cpuclock, &start);
  auto result = new vector<int>(graph->getVertexCover()); // Dynamically allocate memory for results
  clock_gettime(cpuclock, &end);
  double elapsedTime = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

  VertexCoverResult *resultStruct = new VertexCoverResult;
  resultStruct->result = result;
  resultStruct->elapsedTime = elapsedTime;

  return resultStruct; // Return the pointer as void*
}

void *vertexCoverWrapper3SAT(void *arg)
{
  Graph *graph = static_cast<Graph *>(arg);
  clockid_t cpuclock;
  if (pthread_getcpuclockid(pthread_self(), &cpuclock) != 0)
  {
    std::cerr << "Failed to get CPU clock ID." << std::endl;
    return nullptr;
  }

  struct timespec start, end;
  clock_gettime(cpuclock, &start);
  auto result = new vector<int>(graph->getVertexCover3SAT()); // Dynamically allocate memory for results
  clock_gettime(cpuclock, &end);
  double elapsedTime = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

  VertexCoverResult *resultStruct = new VertexCoverResult;
  resultStruct->result = result;
  resultStruct->elapsedTime = elapsedTime;

  return resultStruct; // Return the pointer as void*
}

void *approxVC1Wrapper(void *arg)
{
  Graph *graph = static_cast<Graph *>(arg);
  clockid_t cpuclock;
  if (pthread_getcpuclockid(pthread_self(), &cpuclock) != 0)
  {
    std::cerr << "Failed to get CPU clock ID." << std::endl;
    return nullptr;
  }

  struct timespec start, end;
  clock_gettime(cpuclock, &start);
  auto result = new vector<int>(graph->getApproxVC1());
  clock_gettime(cpuclock, &end);
  double elapsedTime = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

  VertexCoverResult *resultStruct = new VertexCoverResult;
  resultStruct->result = result;
  resultStruct->elapsedTime = elapsedTime;

  return resultStruct;
}

void *approxVC2Wrapper(void *arg)
{
  Graph *graph = static_cast<Graph *>(arg);
  clockid_t cpuclock;
  if (pthread_getcpuclockid(pthread_self(), &cpuclock) != 0)
  {
    std::cerr << "Failed to get CPU clock ID." << std::endl;
    return nullptr;
  }

  struct timespec start, end;
  clock_gettime(cpuclock, &start);
  auto result = new vector<int>(graph->getApproxVC2());
  clock_gettime(cpuclock, &end);
  double elapsedTime = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

  VertexCoverResult *resultStruct = new VertexCoverResult;
  resultStruct->result = result;
  resultStruct->elapsedTime = elapsedTime;

  return resultStruct;
}
