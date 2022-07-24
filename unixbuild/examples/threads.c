/**
 * This is a simple example of using threads to speed up a computationally
 * expensive operation, in this case finding the maximum of a large array of
 * integers.
 */
#include <limits.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define THREAD_COUNT 4
#define ARRAY_SIZE 500000000

void benchmark(int (*f)(int*), int*);
int find_max_single_threaded(int*);
int find_max_multi_threaded(int*);

int main() {
  srand(time(NULL));

  puts("Initializing the test data");
  int* array = malloc(ARRAY_SIZE * sizeof *array);
  if (array == NULL) {
    perror("malloc()");
    exit(1);
  }

  for (size_t i = 0; i < ARRAY_SIZE; i++) {
    array[i] = rand();
  }

  puts("\nRunning single-threaded computation");
  benchmark(find_max_single_threaded, array);
  printf("\nRunning multi-threaded computation (threads = %d)\n", THREAD_COUNT);
  benchmark(find_max_multi_threaded, array);

  free(array);

  return 0;
}

#define NANOS_IN_SEC 1000000000

// Returns the difference between the two times, in seconds.
double diff_timespecs(struct timespec start, struct timespec end) {
  // In the timespec struct, tv_sec is the current second and tv_nsec the
  // fractional nano-second (although the clock might not actually have
  // nanosecond precision).
  long nanoseconds;
  if (end.tv_sec == start.tv_sec) {
    // If the start and end times are in the same second, just subtract the
    // fractional nanoseconds.
    nanoseconds = end.tv_nsec - start.tv_nsec;
  } else {
    // Otherwise, take the remaining nanoseconds of the start time's seconds...
    nanoseconds = (NANOS_IN_SEC - start.tv_nsec) +
                  // ...the whole seconds in between the start and end time...
                  ((end.tv_sec - start.tv_sec) * NANOS_IN_SEC)
                  // ...and the fractional end time second.
                  + end.tv_nsec;
  }

  return ((double)nanoseconds) / NANOS_IN_SEC;
}

void benchmark(int (*f)(int*), int* array) {
  struct timespec start_time;
  if (clock_gettime(CLOCK_REALTIME, &start_time) < 0) {
    perror("clock_gettime()");
    exit(1);
  }

  int result = f(array);

  struct timespec end_time;
  if (clock_gettime(CLOCK_REALTIME, &end_time) < 0) {
    perror("clock_gettime()");
    exit(1);
  }

  double seconds = diff_timespecs(start_time, end_time);
  printf("Got result %d in %.3f seconds\n", result, seconds);
}

int find_max_single_threaded(int* array) {
  int max = INT_MIN;
  for (size_t i = 0; i < ARRAY_SIZE; i++) {
    if (array[i] > max) {
      max = array[i];
    }
  }
  return max;
}

typedef struct {
  size_t start_index;
  size_t end_index;
  int* array;
  int result;
} thread_data;

void* thread_routine(void* arg) {
  thread_data* data = (thread_data*)arg;

  int max = INT_MIN;
  for (size_t i = data->start_index; i < data->end_index; i++) {
    if (data->array[i] > max) {
      max = data->array[i];
    }
  }

  // Store the result in the thread data, which we know will outlive this
  // thread.
  data->result = max;
  pthread_exit(NULL);
}

int find_max_multi_threaded(int* array) {
  // Partition the array and spawn a thread for each partition to search for the
  // maximum in parallel.
  pthread_t threads[THREAD_COUNT];
  thread_data data[THREAD_COUNT];
  size_t partition_size = ARRAY_SIZE / THREAD_COUNT;
  for (size_t i = 0; i < THREAD_COUNT; i++) {
    data[i].start_index = i * partition_size;
    data[i].end_index = (i + 1) * partition_size;
    data[i].array = array;
    if (pthread_create(threads + i, NULL, thread_routine, data + i) < 0) {
      perror("pthread_create()");
      exit(1);
    }
  }

  // Find the maximum of the individual threads' results.
  int max = INT_MIN;
  for (size_t i = 0; i < THREAD_COUNT; i++) {
    pthread_join(threads[i], NULL);
    if (data[i].result > max) {
      max = data[i].result;
    }
  }
  return max;
}
