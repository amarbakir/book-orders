#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "customer-database.h"
#include "consumer-database.h"
#include "book-orders.h"
//MAIN PROG ARGS: ./book-orders <cust database input file> <book order input file> <categories files>

int main(int argc, char **argv){
	char *cust_file = argv[1];
	char *order_file = argv[2];
	char *categories_file = argv[3];
	
	printf("customer file: %s\n", cust_file);
	printf("order file: %s\n", order_file);
	printf("categories file: %s\n", categories_file);
	printf("\n");
	
	CDB cdb;
	cdb = CDCreate();
	cdb = read_customers(cdb, cust_file);
	PrintDB(cdb);
	
	read_categories(categories_file);
	
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
	const char delims[2] = "|\n";
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
		name[strlen(token)] = '\0';
		printf("	name: %s\n", name);
		
		token = strtok(NULL, delims); //id
		id = strtol(token, NULL, 10);
		printf("	id: %ld\n", id);
		
		token = strtok(NULL, delims); //balance
		balance = strtod(token, NULL);
		printf("	balance: %f\n", balance);
		
		token = strtok(NULL, delims);
		address =(char *) malloc(strlen(token) + 1);
		strcpy(address, token);
		address[strlen(token)] = '\0';
		printf("	addr: %s\n", address);
		
		token = strtok(NULL, delims);
		state =(char *) malloc(strlen(token) + 1);
		strcpy(state, token);
		state[strlen(token)] = '\0';
		printf("	state: %s\n", state);
		
		token = strtok(NULL, delims);
		zip =(char *) malloc(strlen(token) + 1);
		strcpy(zip, token);
		zip[strlen(token)] = '\0';
		printf("	zip: %s\n", zip);
		
		cust->name = name;
		cust->id = id;
		cust->balance = balance;
		cust->address = address;
		cust->state = state;
		cust->zip = zip;
		
		//printCustomer(cust);
		CDInsert(cdb, cust);
				
	}
	void *vcdb = (void *) cdb->dbarray; //vdb is the actual customer array, casted to a void (doesnt have the cdb wrapper)
	qsort(vcdb,cdb->numCust,sizeof(Customer),structcomp);
	cdb->dbarray = (Customer *) vcdb;
	return cdb;
}

int read_categories(char *filename){ //reads in the categories textfile

	FILE *categories_file;
	categories_file = fopen(filename, "r");
	
	if (categories_file == NULL){ //obv checking if null like DUH
    		printf("ERR: could not read file %s\n", filename);
    		return -1;
	}
	
	char line[200]; //remember MAY NEED TO MODIFY
	const char delims[2] = "\n ";
	
	while(fgets(line, 200, categories_file) != NULL){ //stored in 'line'
		printf("line: %s", line);
		char *token;
		token = strtok(line, delims); //name
		char *category = (char *) malloc(strlen(token) + 1);
		strcpy(category, token);
		category[strlen(token)] = '\0';
		printf("	category: '%s'\n", category);
	}
	return 0;

}
/*------------HELPER FUNCTIONS-------------*/

void printCustomer(Customer *dude){
	printf("---PRINTING CUSTOMER---\n");
	printf("Customer Name: %s\n", dude->name);
	printf("Customer ID: %ld\n", dude->id);
	printf("Customer Balance: %f\n", dude->balance);
	printf("Customer Address: %s\n", dude->address);
	printf("Customer State: %s\n", dude->state);
	printf("Customer Zip: %s\n", dude->zip);
	printf("---END PRINTING CUSTOMER---\n");
}
