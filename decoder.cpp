#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char  Byte;

class JPEGimage{

public:
	void loadJPGEimage(const char *fileName);
	JPEGimage(){
		JpegSizeX = 0;
		JpegSizeY = 0;
	}

	~JPEGimage(){

	}

private:
	char *bitstream_data;
	int JpegSizeX, JpegSizeY;
    int decodeDC(int *position);
};
/*
int JPEGimage::decodeDC(int *position)
{
    int codeLen[12] = {2,4,5,6,7,8,10,12,14,16,18,20};
    const char* code[12] = {"00","010","011","100","101","110","1110","11110","111110","1111110","11111110","111111110"};
    int i = 0, lenth_case = 0;

    if(strncmp(bitstream_data, code[0], 2) == 0);

}
*/
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
}

int main(void)
{
	JPEGimage jj;

	jj.loadJPGEimage("out.Ajpg");
	return 0;
}