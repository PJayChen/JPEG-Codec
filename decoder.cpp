#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef unsigned char  Byte;

class JPEGimage{

public:
	void loadJPGEimage(const char *fileName);
	int decodeDC(void);

	JPEGimage(){
		JpegSizeX = 0;
		JpegSizeY = 0;
	}

	~JPEGimage(){

	}

private:
	char *bitstream_data;
	char *bdptr; //point to the current processing position of bitstream_data
	int JpegSizeX, JpegSizeY;
    
};


/*
* DC is composition with {SIZE, VALUE}
* first we need to find out the SIZE part
* than we can know the length of VALUE
* so the next step is convert the VALUE from binary string(char *) to decimal(int)
*
* note: use the "bdptr" to point the currnet processing position of "bitstream_data"
*/
int JPEGimage::decodeDC(void)
{
    const int codeTotalLen[12] = {2,4,5,6,7,8,10,12,14,16,18,20};
    const int codeLen[12] = {2, 3, 3, 3, 3, 3, 4, 5, 6, 7, 8, 9};
    const char* code[12] = {"00","010","011","100","101","110","1110","11110","111110","1111110","11111110","111111110"};
    int i = 0;
    
    //Find the which DC code is
    for(i = 0; i < 12 ; i++)
    	if(strncmp(bdptr, code[i], codeLen[i]) == 0)
    		break;
    
    //move pointer to point to "VALUE" of DC
    bdptr += codeLen[i];
    //find the length of VALUE 
    int valueLen = codeTotalLen[i] - codeLen[i];
    
    //store the VALUE part of DC in value
    char *value;
    value = (char *) malloc(valueLen * sizeof(char) + 1);
    value = strncpy(value, bdptr, valueLen);
    value[valueLen] = '\0';

    //move ptr to next component of the bitstream
    bdptr += valueLen;

    int DC = 0;
    //convert binary str to integer
    for(int j = valueLen-1; j >= 0  ; j--){
    	if(value[j] == '1')
    		DC += (int)pow(2,valueLen - (j+1));
    }
    //get the 1's complementation
    if(value[0] == '0'){
    	DC = DC + 1 - (int)pow(2, valueLen);
    }

    //printf("bitstream_data start from %d, current is %d\n", bitstream_data, bdptr);
    printf("DC: %s(%d)  \n", value, DC);
    

    return DC;
}

void JPEGimage::loadJPGEimage(const char *fileName)
{
	long file_size = 0;
	FILE *fp;
	Byte read_byte, temp_byte;
	int i = 0, cnt = 0;

	//open file
	if(NULL == (fp=fopen(fileName, "r+b")))
		printf("Error in opening file.\n");


    //The first 2 byte in file is the image size
    JpegSizeX = (unsigned int)getc(fp) * 8;
    JpegSizeY = (unsigned int)getc(fp) * 8;
    printf("The Jpeg Image size is %d*%d\n", JpegSizeX, JpegSizeY);
    

	//find the size of the file 	
	fseek(fp, 0, SEEK_END); //move position indicator(PI) to the end of file
	file_size = ftell(fp);
	fseek(fp, 2, SEEK_SET); //move PI back to the start of the file
	printf("File \"%s\" open success, the size is %ld byte\n", fileName, file_size);
    file_size -= 2; //ignore the first 2 byte(represent for # of blocks)    

	//allocate memory for the string to contain read bitstream
	bitstream_data = (char *) malloc((file_size * 8 * sizeof(char)) + 1);
    bdptr = bitstream_data;
	//translate the binary to store in string(ex: 0xf0 -> "11110000")
	while(!feof(fp)){
		read_byte = getc(fp);
		for(i = 7; i >= 0; i--){
			temp_byte = read_byte & 0x01; //mask the bit other than LSB
			bitstream_data[cnt*8 + i] = (temp_byte == 0x01)?'1':'0';			
			read_byte = read_byte >> 1;
		}
		cnt++;
	}

	bitstream_data[file_size*8] = '\0'; 
	printf("The file content is \n%s\n", bitstream_data);
	if(fp) fclose(fp);

	decodeDC();
}

int main(void)
{
	JPEGimage jj;

	jj.loadJPGEimage("out.Ajpg");
	//jj.decodeDC(0);
	return 0;
}