#include <cstdlib>
#include <cstdio>
#include <unistd.h> /*sleep*/
#include <pthread.h>
#include <errno.h>

int gnum=0;
// pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;// init
pthread_mutex_t mutex;
static void* pthread_func1(void);
static void* pthread_func2(void);
int main(){
    pthread_t pt_1 =0;
    pthread_t pt_2=0;
    int ret=0;
    pthread_mutex_init(&mutex, nullptr);
    // argument of type "void *(*)()" 
    // is incompatible with parameter of type "void *(*)(void *)"
    ret = pthread_create(&pt_1, nullptr, pthread_func1, NULL);
    if(ret!=0){
        perror("pthread_func1 error");
    }
    ret = pthread_create(&pt_2, nullptr, pthread_func2, NULL);
    if(ret!=0){
        perror("pthread_func2 error");
    }
    pthread_join(pt_1, NULL);
    pthread_join(pt_2, NULL);
    printf("main program exit!\n");
    return 0;
}
static void* pthread_func1(void){
    int i=0;
    for(i=0; i<3; i++){
        printf("This is pthread_1!\n");
        pthread_mutex_lock(&mutex);
        gnum++;
        sleep(1);
        printf("thread_1 add one to num:%d\n", gnum);
        pthread_mutex_unlock(&mutex);
    }
    pthread_exit ( NULL );
    return nullptr;
}
static void* pthread_func_2 (void)
{
  int i = 0;

  for( i=0; i<5; i++ )  {
    printf ("This is pthread_2!/n");
    pthread_mutex_lock(&mutex);
    sleep (1);
    gnum++;
    printf ("Thread_2 add one to num:%d/n",gnum);
    pthread_mutex_unlock(&mutex);

  }
  pthread_exit ( NULL );
  return nullptr;
}