#ifndef BITSTREAM_H
#define BITSTREAM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct bitstream_data{
	char *data; //Could be DC or AC
	bitstream_data *AC_head;
	bitstream_data *next;
};


class Bitstream{
private:
	bitstream_data *ptr_block; //new data
	bitstream_data *head_block;
	bitstream_data *current_block;
	bitstream_data *prev_block; 
	void displayAC(bitstream_data *bd);
public:

	Bitstream(void){
		head_block = new bitstream_data;
		head_block->data = NULL;
		head_block->AC_head = NULL;
		head_block->next = NULL;
	}

	/*
		TO-DO: free all allocated memory
	*/
	~Bitstream(void){

	}

	void add_DC(const char *DC);
	void add_ACtoTailBlock(const char *AC); //ad AC value at tail block 
	void displayAllDCs(void);
	void displayAll(void);
	void displayTailImageBlockDCAC(void);
};

void Bitstream::add_DC(const char *DC)
{
	//new node
	ptr_block = new bitstream_data;
	ptr_block->data = (char *)malloc(sizeof(DC)+1);
	strcpy(ptr_block->data, DC);
	ptr_block->next = NULL;
	ptr_block->AC_head = NULL;
	//allocate memory for AC component
	ptr_block->AC_head = new bitstream_data;
	ptr_block->AC_head->next = NULL;
	ptr_block->AC_head->data = NULL;
	ptr_block->AC_head->AC_head = NULL;//never be used

	prev_block = head_block;
	current_block = head_block->next;
	
	//find tail block
	while(current_block){
		prev_block = current_block;
		current_block = current_block->next;
	}
	//insert the new block at the tail
	current_block = prev_block;
	current_block->next = ptr_block;
}

void Bitstream::add_ACtoTailBlock(const char *AC)
{
	//new node 
	ptr_block = new bitstream_data;
	ptr_block->data = (char *)malloc(sizeof(AC)+1);
	strcpy(ptr_block->data, AC);
	ptr_block->AC_head = NULL;
	ptr_block->next = NULL;

	prev_block = head_block;
	current_block = head_block->next;
	
	//find tail block image
	while(current_block){
		prev_block = current_block;
		current_block = current_block->next;
	}
	current_block = prev_block;
	//locate out the head of AC list
	prev_block = current_block->AC_head;
	current_block = current_block->AC_head->next;
	
	//find tail AC in tial block image
	while(current_block){
		prev_block = current_block;
		current_block = current_block->next;
	}
	current_block = prev_block;  
	current_block->next = ptr_block;
}

void Bitstream::displayAllDCs(void)
{
	current_block = head_block->next;
	while(current_block){
		printf("%s \n", current_block->data);
		current_block = current_block->next;
	}
}

void Bitstream::displayTailImageBlockDCAC(void)
{
	current_block = head_block->next;
	prev_block = head_block;
	while(current_block){
		prev_block = current_block;
		current_block = current_block->next;
	}
	current_block = prev_block;
	printf("%s, ", current_block->data);
	displayAC(current_block);
	printf("\n");
}

void Bitstream::displayAll(void)
{
	current_block = head_block->next;//head didn't store data
	while(current_block){
		printf("%s, ", current_block->data);
		displayAC(current_block);
		current_block = current_block->next;
		printf("\n");
	}	
	printf("\n");
}

void Bitstream::displayAC(bitstream_data *bd)
{	
	bd = bd->AC_head->next; //head didn't store data
	while(bd){
		printf("%s ", bd->data);
		bd = bd->next;
	}
}

#endif

/*
int main(void)
{
	Bitstream b;
	b.add_DC("11111");
	b.add_ACtoTailBlock("00000");
	b.add_ACtoTailBlock("000110000111");
	b.add_ACtoTailBlock("011000000");
	b.add_DC("0111");
	b.add_ACtoTailBlock("000001100");
	b.add_ACtoTailBlock("11111111111111101111110");
	b.add_ACtoTailBlock("011000000");
	b.add_DC("11111111111111101111111110");
	b.displayAll();
	return 0;
}
*/