#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "customer-database.h"
#include "consumer-database.h"
#include "book-orders.h"
#include "queue.h"
//MAIN PROG ARGS: ./book-orders <cust database input file> <book order input file> <categories files>

int main(int argc, char **argv){
	char *cust_file = argv[1];
	char *orders_file = argv[2];
	char *categories_file = argv[3];
	
	printf("customer file: %s\n", cust_file);
	printf("order file: %s\n", orders_file);
	printf("categories file: %s\n", categories_file);
	printf("\n");
	
	CDB tcdb = CDCreate();
	tcdb = read_customers(tcdb, cust_file); //first fxn
	cdb = tcdb;
	
	
	CSA tcsa = CSACreate();
	tcsa = read_categories(tcsa, categories_file); //2nd fxn
	csa = tcsa;
	
	pthread_t tid[csa->numCons + 1];
	pthread_create(&tid[0], NULL, producer, (void *)orders_file);
	
	int i;
	for (i = 0; i < csa->numCons; i++) {
		pthread_create(&tid[i + 1], NULL, consumer, (void *)&(csa->consumerdata[i]));
	}
	
	for (i = 0; i < csa->numCons + 1; i++) {
		pthread_join(tid[i], NULL);
	}
	
	PrintDB(cdb);
	printf("ENDING MAIN\n");
	return 0;
}

CDB read_customers(CDB cdb, char *filename){ //cdb is the customer database ptr you got in this fxn
	
	FILE *customer_file;
	customer_file = fopen(filename, "r");
	
	if (customer_file == NULL){ //obv checking if null like DUH
    		printf("ERR: could not read file %s", filename);
    		return NULL;
	}
	
	char line[300]; //remember MAY NEED TO MODIFY
	char delims[3];
	delims[0] = '|';
	delims[1] = '\n';
	delims[2] = '\0';

	//cdb = CDCreate();
	while(fgets(line, 300, customer_file) != NULL){ //stored in 'line'
		printf("line: %s", line);
		
		Customer *cust =(Customer *) malloc(sizeof(Customer));
		char *token;
		
		char *name;
		long id;// char id;
		double balance;// char balance;
		char *address;
		char *state;
		char *zip;
		
		token = strtok(line, delims); //name
		name =(char *) malloc(strlen(token) + 1);
		strcpy(name, token);
		name[strlen(name)] = '\0';
		//printf("	name: %s\n", name);
		
		token = strtok(NULL, delims); //id
		id = strtol(token, NULL, 10);
		//printf("	id: %ld\n", id);
		
		token = strtok(NULL, delims); //balance
		balance = strtod(token, NULL);
		//printf("	balance: %f\n", balance);
		
		token = strtok(NULL, delims);
		address =(char *) malloc(strlen(token) + 1);
		strcpy(address, token);
		address[strlen(address)] = '\0';
		//printf("	addr: %s\n", address);
		
		token = strtok(NULL, delims);
		state =(char *) malloc(strlen(token) + 1);
		strcpy(state, token);
		state[strlen(state)] = '\0';
		//printf("	state: %s\n", state);
		
		token = strtok(NULL, delims);
		zip =(char *) malloc(strlen(token) + 1);
		strcpy(zip, token);
		zip[strlen(zip)] = '\0';
		//printf("	zip: %s\n", zip);
		
		cust->name = name;
		cust->id = id;
		cust->balance = balance;
		cust->address = address;
		cust->state = state;
		cust->zip = zip;
		
		//printCustomer(cust);
		CDInsert(cdb, cust);
		free(cust);
				
	}
	void *vcdb = (void *) cdb->dbarray; //vcdb is the actual customer array, casted to a void (doesnt have the cdb wrapper)
	qsort(vcdb,cdb->numCust,sizeof(Customer),customercomp);
	cdb->dbarray = (Customer *) vcdb;
	return cdb;

}

CSA read_categories(CSA csa, char *filename){ //reads in the categories textfile

	FILE *categories_file;
	categories_file = fopen(filename, "r");
	
	if (categories_file == NULL){ //obv checking if null like DUH
    		printf("ERR: could not read file %s\n", filename);
    		return NULL;
	}
	
	char line[200]; //remember MAY NEED TO MODIFY
	char delims[3];
	delims[0] = '|';
	delims[1] = '\n';
	delims[2] = '\0';

	printf("\n----printing categories-----\n");
	while(fgets(line, 200, categories_file) != NULL){ //stored in 'line'
		
		printf("line: %s", line);
		char *token;
		token = strtok(line, delims); //name
		char *category = (char *) malloc(strlen(token) + 1);
		strcpy(category, token);
		category[strlen(category)] = '\0';
		printf("	category: '%s'\n", category);
	
		CSAInsert(csa, category);
	}
	void *vcsa = (void *) csa->consumerdata; //vcsa is the actual consumerdata array, casted to a void (doesnt have the csa wrapper)
	qsort(vcsa,csa->numCons,sizeof(ConsumerStruct),consumercomp);
	csa->consumerdata = (ConsumerStruct *) vcsa;
	printf("----ending categories-----\n\n");
	return csa;

}

void *producer(void *fn){
	//pthread_detach(pthread_self());
	char *filename;
	filename = (char *)fn;
	//printf("--------ORDERS--------\n");
	FILE *orders_file;
	orders_file = fopen(filename, "r");
	
	if (orders_file == NULL){
    		printf("ERR: could not read orders file %s\n", filename);
    		return NULL;
	}
	
	char line[300];
	char delims[3];
	delims[0] = '|';
	delims[1] = '\n';
	delims[2] = '\0';

	//pthread_detach(pthread_self());
	while(fgets(line, 300, orders_file) != NULL){ //each line is an order
		int index;
		QNode *order;
		order = (QNode *)malloc(sizeof(QNode));
		//printf("line: %s", line);
		//printf("PRODUCER: Preparing order for %s\n", order->bname);
		char *token;

		char *name;	//book title
		double cost;	//book cost
		long id;	//customer id
		char *category;	//book category
		
		token = strtok(line, delims); //name
		name =(char *) malloc(strlen(token) + 1);
		strcpy(name, token);
		name[strlen(name)] = '\0';
		printf("PRODUCER: Preparing order for %s\n", name);
		printf("	title: %s\n", name);

		token = strtok(NULL, delims); //balance
		cost = strtod(token, NULL);
		printf("	cost: %f\n", cost);

		token = strtok(NULL, delims); //id
		id = strtol(token, NULL, 10);
		printf("	customer id: %ld\n", id);

		token = strtok(NULL, delims); //name
		category =(char *) malloc(strlen(token) + 1);
		strcpy(category, token);
		category[strlen(category)] = '\0';
		printf("	category: %s\n", category);
		
		order->bname = name;
		order->price = cost;
		order->id = id;
		order->category = category;
		order->next = NULL;
		index = binarySearch2(csa, order->category, 0, csa->numCons -1);
		printf("consumer index: %d\n", index);
		if (index >= 0) {
			pthread_mutex_lock(&csa->consumerdata[index].mutex);
			if (csa->consumerdata[index].q->numElem == csa->consumerdata[index].q->max)
			{
				pthread_cond_signal(&csa->consumerdata[index].dataAvailable); // shout at consumer
				printf("PRODUCER waits for consumer thread '%s' because of full queue.\n", csa->consumerdata[index].category);
				pthread_cond_wait(&csa->consumerdata[index].spaceAvailable, &csa->consumerdata[index].mutex);
				printf("PRODUCER resuming creation of orders for consumer thread '%s'.\n", csa->consumerdata[index].category);
			}
			if(push(csa->consumerdata[index].q, order) == 0) {
				printf("Error: Push failed.\n");
				return NULL;
			}
			pthread_cond_signal(&csa->consumerdata[index].dataAvailable); // shout at consumer
			pthread_mutex_unlock(&csa->consumerdata[index].mutex);
		}
	}
	csa->done = 1;
	int i;
	for (i = 0; i < csa->numCons - 1; i++) {
		pthread_cond_signal(&csa->consumerdata[i].dataAvailable);
	}
	PrintDB(cdb);
	return NULL;
}

void *consumer(void *cs) {
	//pthread_detach(pthread_self());
	ConsumerStruct *consumerstruct;
	consumerstruct = (ConsumerStruct *)cs;
	if (cdb == NULL || cs == NULL) {
		printf("ERROR: CDB or consumer struct NULL.\n");
		return NULL;
	}
	while(!csa->done || consumerstruct->q->numElem >= 0){
		int index;
		QNode *order;
		
		pthread_mutex_lock(&consumerstruct->mutex);
		if (consumerstruct->q->numElem <= 0 && !csa->done)
		{
			pthread_cond_signal(&consumerstruct->spaceAvailable); // shout at producer
			printf("CONSUMER thread '%s' waits for producer because of empty queue.\n", consumerstruct->category);
			pthread_cond_wait(&consumerstruct->dataAvailable, &consumerstruct->mutex);
			printf("CONSUMER thread '%s' resuming order processing.\n", consumerstruct->category);
		}
		order = pop(consumerstruct->q);
		printf("CONSUMER: Processing order for %s\n", order->bname);
		index = binarySearch(cdb, order->id, 0, cdb->numCust - 1);
		printf("index: %d\n", index);
		if (index >= 0) {
			pthread_mutex_lock(&cdb->dbarray[index].queue_mutex);
			CDUpdate(cdb, order, index);
			pthread_mutex_unlock(&cdb->dbarray[index].queue_mutex);
		} else {
			printf("Error: customer not found in CDUpdate.");
			return 0;
		}
		//printf("done with customer update\n");
		free(order->bname);
		free(order->category);
		free(order);
		pthread_cond_signal(&consumerstruct->spaceAvailable); // shout at producer
		pthread_mutex_unlock(&consumerstruct->mutex);
		//PrintDB(cdb);
	}
	printf("Consumer %s is done.\n", consumerstruct->category);
	return NULL;
}
