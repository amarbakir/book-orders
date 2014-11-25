#include "queue.h"
#include <string.h>
#include <stdio.h>

Queue QCreate() {
	Queue q;
	q = (struct queue *)malloc(sizeof(struct queue));
	if (q == NULL) {
		printf("Error: Out of memory.\n");
		return NULL;
	}
	q->numElem = 0;
	q->front = NULL;
	q->back = NULL;
	q->max = 128;
	return q;
}

int QDestroy(Queue q) {
	if (q->front == NULL) {
		printf("Error: Queue is NULL.\n");
		return 0;
	}
	QNode *curr;
	QNode *temp;
	curr = q->front;
	while (curr != NULL) {
		temp = curr;
		curr = curr->next;
		free(temp);
	}
	return 1;
}

int push(Queue q, QNode *n) {
	if (n == NULL || q == NULL) {
		printf("Error: Cannot push NULL node or Queue is NULL.\n");
		return 0;
	}
	if (q->front == NULL) {
		q->front = n;
		q->back = n;
		n->next = NULL;
		q->numElem++;
		return 1;
	}
	q->back->next = n;
	q->back = n;
	n->next = NULL;
	q->numElem++;
	return 1;
}

QNode *pop(Queue q) {
	QNode *r;
	if (q->numElem == 0) {
		return NULL;
	}
	r = q->front;
	q->front = q->front->next;
	q->numElem--;
	return r;
}
