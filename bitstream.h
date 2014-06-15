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
	char *allData;//to contain all DC + AC
	char *onlyDCdata; //contain only DC component
	long numberOfBits;
	int numberOfDCbits;
	void displayAC(bitstream_data *bd);
	void stringToBinary(const char* a, FILE *fp);
	void catAllComponent(char *all);
	void catAcComponent(bitstream_data *bd, char *all);
	void catAllDCcomponent(char *allDC);

public:

	Bitstream(void){
		head_block = new bitstream_data;
		head_block->data = NULL;
		head_block->AC_head = NULL;
		head_block->next = NULL;

		allData = NULL;
		onlyDCdata = NULL;
		numberOfBits = 0;
		numberOfDCbits = 0;
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
	void writeToFileInBinary(const char *outFileName);
	void writeTiFileOnlyDC(const char *outFileName);

};

void Bitstream::add_DC(const char *DC)
{
	//new node
	ptr_block = new bitstream_data;
	ptr_block->data = (char *)malloc(strlen(DC) + 1);
	strcpy(ptr_block->data, DC);
	ptr_block->next = NULL;
	ptr_block->AC_head = NULL;
	
	numberOfBits += strlen(DC);//conunt there are how many bits 
	numberOfDCbits += strlen(DC);

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
	ptr_block->data = (char *)malloc(strlen(AC)+1);
	strcpy(ptr_block->data, AC);
	ptr_block->AC_head = NULL;
	ptr_block->next = NULL;

	numberOfBits+= strlen(AC);//conunt there are how many bits 

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
	printf("There are %ld bits(DC: %d)\n", numberOfBits, numberOfDCbits);
}

void Bitstream::displayAC(bitstream_data *bd)
{	
	bd = bd->AC_head->next; //head didn't store data
	while(bd){
		printf("%s ", bd->data);
		bd = bd->next;
	}
}

/*
 *	concatenate AC component into a string
 */
void Bitstream::catAcComponent(bitstream_data *bd, char *all)
{
	bd = bd->AC_head->next; //head didn't store data
	while(bd){
		strcat(all, bd->data);
		bd = bd->next;
	}
}

/*
 *	concatenate ALL(DC and AC) component into a string
 */
void Bitstream::catAllComponent(char *all)
{
	current_block = head_block->next;//head didn't store data
	while(current_block){
		strcat(all, current_block->data);
		catAcComponent(current_block, all);
		current_block = current_block->next;
	}	
}

void Bitstream::catAllDCcomponent(char *allDC)
{
	current_block = head_block->next;//head didn't store data
	while(current_block){
		strcat(allDC, current_block->data);
		strcat(allDC, "1010");
		current_block = current_block->next;
	}		
}

void Bitstream::writeTiFileOnlyDC(const char *outFileName)
{
	FILE *fp;
	onlyDCdata = new char[numberOfDCbits + 4*4096 + 1];
	*onlyDCdata = '\0';

	if(NULL == (fp=fopen(outFileName, "a+b")))
		printf("Error in opening file.\n");

	catAllDCcomponent(onlyDCdata);
	printf("write the following data into file %s \n%s\n", outFileName, onlyDCdata);
	stringToBinary(onlyDCdata, fp);

	if(fp) fclose(fp);

	delete[] onlyDCdata;
}

/*
 *  write string of all component into the file point by fp
 */
void Bitstream::writeToFileInBinary(const char *outFileName)
{
	FILE *fp;
	allData = new char[numberOfBits];
	*allData = '\0';

	if(NULL == (fp=fopen(outFileName, "a+b")))
		printf("Error in opening file.\n");

	catAllComponent(allData);
	printf("write the following data into file %s \n%s\n", outFileName, allData);
	stringToBinary(allData, fp);

	if(fclose(fp))
		printf("Error in close file.\n");

	delete[] allData;
}

void Bitstream::stringToBinary(const char* a, FILE *fp)
{
	int len = strlen(a);
 	int b = 0;
 	int c = len/8;
 	int i,j;
 
 	//char d;
	for(i=0;i<=c;i++)
	{
   		b = 0;
   		for(j=0;j<8;j++)
      		if((i*8+j<len)&&(a[i*8+j]=='1'))
	       		b = b | (int)pow(2,7-j);
   		//d = b;
   		putc(b, fp);
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