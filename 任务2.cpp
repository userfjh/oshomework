#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define BUFFER_SIZE 5
int buffer[BUFFER_SIZE];
int count = 0;
int in = 0;
int out = 0;

sem_t empty;
sem_t full;
pthread_mutex_t mutex;

// 生产者线程函数
void* producer(void* arg) {
    int item;
    for (int i = 0; i < 10; i++) {
        // 生产一个产品
        item = rand() % 100;
        
        // 等待缓冲区有空位
        sem_wait(&empty);
        
        // 进入临界区
        pthread_mutex_lock(&mutex);
        
        // 将产品放入缓冲区
        buffer[in] = item;
        printf("生产者生产: %d 放入位置 %d\n", item, in);
        in = (in + 1) % BUFFER_SIZE;
        count++;
        
        // 离开临界区
        pthread_mutex_unlock(&mutex);
        
        // 通知缓冲区有新数据
        sem_post(&full);
        
        // 模拟生产时间
        usleep(rand() % 100000);
    }
    pthread_exit(NULL);
}

// 消费者线程函数
void* consumer(void* arg) {
    int item;
    for (int i = 0; i < 10; i++) {
        // 等待缓冲区有数据
        sem_wait(&full);
        
        // 进入临界区
        pthread_mutex_lock(&mutex);
        
        // 从缓冲区取出产品
        item = buffer[out];
        printf("消费者消费: %d 从位置 %d\n", item, out);
        out = (out + 1) % BUFFER_SIZE;
        count--;
        
        // 离开临界区
        pthread_mutex_unlock(&mutex);
        
        // 通知缓冲区有空位
        sem_post(&empty);
        
        // 模拟消费时间
        usleep(rand() % 150000);
    }
    pthread_exit(NULL);
}

int main() {
    pthread_t producer_thread, consumer_thread;
    
    // 初始化信号量和互斥锁
    sem_init(&empty, 0, BUFFER_SIZE);  // 缓冲区初始有BUFFER_SIZE个空位
    sem_init(&full, 0, 0);             // 缓冲区初始有0个数据
    pthread_mutex_init(&mutex, NULL);  // 初始化互斥锁
    
    // 创建生产者和消费者线程
    pthread_create(&producer_thread, NULL, producer, NULL);
    pthread_create(&consumer_thread, NULL, consumer, NULL);
    
    // 等待线程结束
    pthread_join(producer_thread, NULL);
    pthread_join(consumer_thread, NULL);
    
    // 销毁信号量和互斥锁
    sem_destroy(&empty);
    sem_destroy(&full);
    pthread_mutex_destroy(&mutex);
    
    return 0;
}