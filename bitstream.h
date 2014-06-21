#ifndef BITSTREAM_H
#define BITSTREAM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//Could be used by block_list or AC_list
struct bitstream_data{
	char *data; //Could be DC or AC
	bitstream_data *AC_head; //point to the AC_list head
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
	long numberOfBits; //Total bits of the data(DC + AC)
	int numberOfDCbits; //sum of DC bits
	void displayAC(bitstream_data *bd);
	void stringToBinary(const char* a, FILE *fp);
	void catAllComponent(char *all);
	void catAcComponent(bitstream_data *bd, char *all);
	void catAllDCcomponent(char *allDC);
	void catSomeAcComponent(bitstream_data *bd, char *all, int number);
	void catAllDCandSomeACcomponent(char *all, int number);

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
	void writeToFileOnlyDC(const char *outFileName);
	void writeToFileWithSomeAC(const char *outFileName, int ACnumber);
	inline void mv_CurBlk_to_next(void);
	inline void mv_CurBlk_to_head(void);
	void add_ACtoCurBlk(const char *AC);
};



/*
 *  Move the current_block point to next node of block_list
 */
inline void Bitstream::mv_CurBlk_to_next(void)
{
	current_block = current_block->next;
}

/*
 * Make the current_block point back to first node(head_block->next) of block_list
 */
inline void Bitstream::mv_CurBlk_to_head(void)
{	
	if(head_block->next != NULL)
		current_block = head_block->next;
	else 
		printf("error: head_block->next is NULL");
}

/*
 * This function will add a AC into new node of tail AC_lsit 
 * of the block node point by current_block.
 * 
 * Notice, 
 * use this function you must be clearly known where the current_block is point to.
 */
void Bitstream::add_ACtoCurBlk(const char *AC)
{
	//new node 
	ptr_block = new bitstream_data;
	ptr_block->data = (char *)malloc(strlen(AC)+1);
	strcpy(ptr_block->data, AC);
	ptr_block->AC_head = NULL;
	ptr_block->next = NULL;

	//conunt there are how many bits 	
	numberOfBits+= strlen(AC);

	bitstream_data *tail_node;

	//locate out the head of AC list
	prev_block = current_block->AC_head;
	tail_node = current_block->AC_head->next;
	
	//find tail AC in tial block image
	while(tail_node){
		prev_block = tail_node;
		tail_node = tail_node->next;
	}
	tail_node = prev_block;  
	tail_node->next = ptr_block;

}

/*
* Add a DC component into the block_list
*
* this function will create a new node to contain a DC
* than link this node at the tail of the block_list
*
* current_block will point to the tail node of the block_list
*
* how many times add_DC() has beed called namely there are how many blocks.
*/
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

/*
 * Add a AC component into the AC_list
 * 
 * The AC_list is point by the current_block->AC_head
 * Each block_list node has a linked list of AC_list
 *
 * This function will create a new node to contain the AC
 * than link the node at the end of AC_head
 */
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

/*
 * Display all block_list's data namely DC component of the image
 */
void Bitstream::displayAllDCs(void)
{
	current_block = head_block->next;
	while(current_block){
		printf("%s \n", current_block->data);
		current_block = current_block->next;
	}
}

/*
 * Display the latest data (the tail of list)
 */
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

/*
 * Display all stroed data in the block_list and AC_list
 */
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

/*
 * Display all AC component which the AC_list head was pointed by bd
 */
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

/*
 *	concatenate All blocks DC component into a string
 */
void Bitstream::catAllDCcomponent(char *allDC)
{
	current_block = head_block->next;//head didn't store data
	while(current_block){
		strcat(allDC, current_block->data);
		strcat(allDC, "1010");
		current_block = current_block->next;
	}		
}

void Bitstream::catSomeAcComponent(bitstream_data *bd, char *all, int number)
{
	bd = bd->AC_head->next; //head didn't store data
	int i = 0;

		while(bd){
			strcat(all, bd->data);
			bd = bd->next;	
			if(i++ >= number) break;
		}
		if(bd != NULL) strcat(all, "1010");
}

/*
 *	concatenate All DC and Some AC component into a string
 */
void Bitstream::catAllDCandSomeACcomponent(char *all, int number)
{
	current_block = head_block->next;//head didn't store data
	while(current_block){
		strcat(all, current_block->data);
		catSomeAcComponent(current_block, all, number);
		current_block = current_block->next;
	}	
}

void Bitstream::writeToFileWithSomeAC(const char *outFileName, int ACnumber)
{
	FILE *fp;
	allData = new char[numberOfBits];
	*allData = '\0';

	if(NULL == (fp=fopen(outFileName, "a+b")))
		printf("Error in opening file.\n");

	catAllDCandSomeACcomponent(allData, ACnumber);
	printf("write the following data into file %s \n%s\n%ld Byte\n", outFileName, allData, strlen(allData)/8);
	stringToBinary(allData, fp);

	if(fclose(fp))
		printf("Error in close file.\n");

	delete[] allData;
}

/*
 *  write string of all DC component into the file point by fp
 */
void Bitstream::writeToFileOnlyDC(const char *outFileName)
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
	printf("write the following data into file %s \n%s\n%ld Byte\n", outFileName, allData, strlen(allData)/8);
	stringToBinary(allData, fp);

	if(fclose(fp))
		printf("Error in close file.\n");

	delete[] allData;
}

/*
 *  convert the string to the binary format
 */
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