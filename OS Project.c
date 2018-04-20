#include<stdio.h>
#include<conio.h>
#include<pthread.h> 
#include<semaphore.h>
#include<string.h>
#include<stdlib.h> 
#include<time.h>

#define no_seat 3
#define maxmum_sleep 5


int chair[3];
int c = 0; 


void rand_sleep(void);
void* studentProgramming(void* stu_id);
void* AssistingTeacher();

sem_t sem_student;
sem_t sem_assistant;


int seat_next = 0;
int next_teach = 0;

pthread_mutex_t mutex;

int main(int argc, char **argv)
{
	pthread_t teacherAss;
	pthread_t *students;
	
	int* s_id;
	
	int i;

	int numberOfstudent;

	printf("PLease Enter the number of student? ");

	scanf("%d", &numberOfstudent);

	students = (pthread_t*)malloc(sizeof(pthread_t) * numberOfstudent);
	s_id = (int*)malloc(sizeof(int) * numberOfstudent);
	
	memset(s_id, 0, numberOfstudent);

	sem_init(&sem_student,0,0);

	sem_init(&sem_assistant,0,1);

	srand(time(NULL));

	pthread_mutex_init(&mutex,NULL);
	pthread_create(&teacherAss,NULL,AssistingTeacher,NULL);

	for(i=0; i<3; i++)
	{
		s_id[i] = i+1;
		pthread_create(&students[i], NULL, studentProgramming, (void*) &s_id[i]);
	} 

	pthread_join(teacherAss, NULL);
	for(i=0; i<numberOfstudent;i++)
	{
		pthread_join(students[i],NULL);
	}	
	return 0;
}
void* studentProgramming(void* stu_id)
{
	int id = *(int*)stu_id;
	printf("[stu] This Much of student %d is doing programming\n",id);			
	while(1)
	{
		rand_sleep();
		pthread_mutex_lock(&mutex);
		
		if(c < no_seat)	
		{
			chair[seat_next] = id;
			c++;
			
			printf("	[stu] This much of student %d is waiting\n",id);
			printf("Here are the waiting students : [1] %d [2] %d [3] %d\n",chair[0],chair[1],chair[2]);
			seat_next = (seat_next+1) % no_seat;

			pthread_mutex_unlock(&mutex);
	
			sem_post(&sem_student);

			sem_wait(&sem_assistant);
		}
		else 
		
		{
			pthread_mutex_unlock(&mutex);
			
			printf("[stu] Sorry there are no more chairs.Already This much of student %d is programming\n",id);		
		}
	}				
}
void* AssistingTeacher()
{
	while(1)
	{
		sem_wait(&sem_student);		
		pthread_mutex_lock(&mutex);
		printf("	[teacherAss] Teacher Assistant is teaching student %d\n",chair[next_teach]);	
		chair[next_teach]=0;
		c--;
		
		printf("Here are the waiting students : [1] %d [2] %d [3] %d\n",chair[0],chair[1],chair[2]);
		next_teach = (next_teach + 1) % no_seat;
		
		rand_sleep();

		printf("	[teacherAss] Teacher have finished their job.\n");	
		pthread_mutex_unlock(&mutex);
		sem_post(&sem_assistant);
	}	
}
void rand_sleep(void)
{
	int time = rand() % maxmum_sleep + 1;
	sleep(time);
}
