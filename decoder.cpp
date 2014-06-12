#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define ARRAY 0
#define BLOCK 1
#define STRING 2
#define BT 3

typedef unsigned char  Byte;

class JPEGimage{

public:
	void loadJPGEimage(const char *fileName);
    void ImageDecompress(const char *BMPfileName);

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

    void printData(int type, const char *name, const void *data);
    int decodeDC(void);
    void decodeAC(int *zeroRL, int *ACvalue);
    void RLED(int ZZ[64],int RL[64]);
};

void JPEGimage::printData(int type, const char *name, const void *data)
{
    int i,j;

    printf("%s:\n", name);
    
    if(type == BLOCK){
      int *block_data = (int *)data;
      for(i=0;i<8;i++){
        for(j=0;j<8;j++){
          printf("%4d, ", *(block_data + j + i*8));
        }
        printf("\n");
      } 
    }else if(type == ARRAY){
      int *array_data = (int *)data;
      for(j=0;j<64;j++){
        printf("%2d, ", array_data[j]);
      }
      printf("\n");
    }else if(type == STRING){
      char *string_data = (char *) data;
      printf("%s\n", string_data);
    }
    
    printf("\n");
}
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

/*
* The AC is composition with {ZeroRunLength, VALUE}
*/
void JPEGimage::decodeAC(int *zeroRL, int *ACvalue)
{
	int codeLen[16][11] = {
	    {4 ,3 ,4 ,6 ,8 ,10,12,14,18,25,26},
	    {0 ,5 ,8 ,10,13,16,22,23,24,25,26},
	    {0 ,6 ,10,13,20,21,22,23,24,25,26},
	    {0 ,7 ,11,14,20,21,22,23,24,25,26},
	    {0 ,7 ,12,19,20,21,22,23,24,25,26},
	    {0 ,8 ,12,19,20,21,22,23,24,25,26},
	    {0 ,8 ,13,19,20,21,22,23,24,25,26},
	    {0 ,9 ,13,19,20,21,22,23,24,25,26},
	    {0 ,9 ,17,19,20,21,22,23,24,25,26},
	    {0 ,10,18,19,20,21,22,23,24,25,26},
	    {0 ,10,18,19,20,21,22,23,24,25,26},
	    {0 ,10,18,19,20,21,22,23,24,25,26},
	    {0 ,11,18,19,20,21,22,23,24,25,26},
	    {0 ,12,18,19,20,21,22,23,24,25,26},
	    {0 ,13,18,19,20,21,22,23,24,25,26},
	    {12,17,18,19,20,21,22,23,24,25,26}
    };

    const char* code[16][11] = {
        {"1010",  "00",  "01",  "100",  "1011",  "11010",  "111000",  "1111000",  "1111110110",  "1111111110000010",  "1111111110000011"},
        {" ","1100","111001","1111001","111110110","11111110110","1111111110000100","1111111110000101","1111111110000110","1111111110000111","1111111110001000"},
        {" ","11011","11111000","1111110111","1111111110001001","1111111110001010","1111111110001011","1111111110001100","1111111110001101","1111111110001110","1111111110001111"},
        {" ","111010","111110111","11111110111","1111111110010000","1111111110010001","1111111110010010","1111111110010011","1111111110010100","1111111110010101","1111111110010110"},
        {" ","111011","1111111000","1111111110010111","1111111110011000","1111111110011001","1111111110011010","1111111110011011","1111111110011100","1111111110011101","1111111110011110"},
        {" ","1111010","1111111001","1111111110011111","1111111110100000","1111111110100001","1111111110100010","1111111110100011","1111111110100100","1111111110100101","1111111110100110"},
        {" ","1111011","11111111000","1111111110100111","1111111110101000","1111111110101001","1111111110101010","1111111110101011","1111111110101100","1111111110101101","1111111110101110"},
        {" ","11111001","11111111001","1111111110101111","1111111110110000","1111111110110001","1111111110110010","1111111110110011","1111111110110100","1111111110110101","1111111110110110"},
        {" ","11111010","111111111000000","1111111110110111","1111111110111000","1111111110111001","1111111110111010","1111111110111011","1111111110111100","1111111110111101","1111111110111110"},
        {" ","111111000","1111111110111111","1111111111000000","1111111111000001","1111111111000010","1111111111000011","1111111111000100","1111111111000101","1111111111000110","1111111111000111"},
        {" ","111111001","1111111111001000","1111111111001001","1111111111001010","1111111111001011","1111111111001100","1111111111001101","1111111111001110","1111111111001111","1111111111010000"},
        {" ","111111010","1111111111010001","1111111111010010","1111111111010011","1111111111010100","1111111111010101","1111111111010110","1111111111010111","1111111111011000","1111111111011001"},
        {" ","1111111010","1111111111011010","1111111111011011","1111111111011100","1111111111011101","1111111111011110","1111111111011111","1111111111100000","1111111111100001","1111111111100010"},
        {" ","11111111010","1111111111100011","1111111111100100","1111111111100101","1111111111100110","1111111111100111","1111111111101000", "1111111111101001","1111111111101010","1111111111101011"},
        {" ","111111110110","1111111111101100","1111111111101101","1111111111101110","1111111111101111","1111111111110000","1111111111110001","1111111111110010","1111111111110011","1111111111110100"},
        {"111111110111","1111111111110101","1111111111110110","1111111111110111","1111111111111000","1111111111111001","1111111111111010","1111111111111011","1111111111111100","1111111111111101","1111111111111110"}
    };
    printf("AC: ");
    //find the zero Run-lenth of AC
    int zero, size;
    for(zero = 0; zero < 16; zero++)
        for(size = 0; size < 11; size++)
            if(strncmp(bdptr, code[zero][size], strlen(code[zero][size])) == 0) 
                goto end;
    end:
    //move the point to the VALUE part
    bdptr += strlen(code[zero][size]);
    //store how many zero between two AC component
    *zeroRL = zero;

    printf("%s ", code[zero][size]);

    //The below is decompose the VALUE part from bitstream
    int valueLen = codeLen[zero][size] - strlen(code[zero][size]);
    char *value;
    value = (char *) malloc(valueLen * sizeof(char) + 1);
    value = strncpy(value, bdptr, valueLen);
    value[valueLen] = '\0';

    //move the point to the next AC/DC component
    bdptr += valueLen;    
    
    *ACvalue = 0;
    //convert binary str to integer
    for(int j = valueLen-1; j >= 0  ; j--){
        if(value[j] == '1')
            *ACvalue += (int)pow(2,valueLen - (j+1));
    }
    //get the 1's complementation
    if(value[0] == '0'){
        *ACvalue = *ACvalue + 1 - (int)pow(2, valueLen);
    }

    printf("%s(%d,%d)\n", value, *zeroRL,*ACvalue);

}

void JPEGimage::RLED(int ZZ[64],int RL[64])
{
  int rl=1;
  int i=1;
  int k = 0;
  ZZ[0] = RL[0];
  while(i<64)
    {
      if(RL[rl]==0 && RL[rl+1]==0)
    {
      for(k=i;k<64;k++)
        ZZ[k] = 0;
      return;
    }
      for(k=0;k<RL[rl];k++)
    ZZ[i++] = 0;
      ZZ[i++] = RL[rl+1];
      rl+=2;
    }
}

void ZigZagD(int QF[8][8],int ZZ[64])
{
  int i=0,j=0,k=0,d=0;
  while(k<36)
    {
      QF[i][j] = ZZ[k++];
      if((i==0)&&(j%2==0))
    {
      j++;
      d=1;
    }
      else if((j==0)&&(i%2==1))
    {
      i++;
      d=0;
    }
      else if(d==0)
    {
      i--;
      j++;
    }
      else
    {
      i++;
      j--;
    }
    }
  i = 7;
  j = 1;
  while(k<64)
    {
      QF[i][j] = ZZ[k++];
      if((i==7)&&(j%2==0))
    {
      j++;
      d=0;
    }
      else if((j==7)&&(i%2==1))
    {
      i++;
      d=1;
    }
      else if(d==0)
    {
      i--;
      j++;
    }
      else
    {
      i++;
      j--;
    }
    }
}

void QuantizeD(int F[8][8], int QF[8][8])
{
    int q[8][8] = {
         {16,11,10,16,24,40,51,61},
         {12,12,14,19,26,58,60,55},
         {14,13,16,24,40,57,69,56},
         {14,17,22,29,51,87,80,62},
         {18,22,37,56,68,109,103,77},
         {24,35,55,64,81,104,113,92},
         {49,64,78,87,103,121,120,101},
         {72,92,95,98,112,100,103,99} 
        };

    int i,j;
    for(i=0;i<8;i++)
        for(j=0;j<8;j++)
            F[i][j] = QF[i][j]*q[i][j];
}

float C(int u)
{
  if(u==0)
    return (1.0/sqrt(8.0));
  else
    return (1.0/2.0);
}

void DCTD(int f[8][8],int F[8][8])
{
  float a;
  for(int x=0;x<8;x++)
    for(int y=0;y<8;y++)
    {
        a = 0.0;
        for(int u=0;u<8;u++)
            for(int v=0;v<8;v++)
                a += C(u)*C(v)*float(F[u][v])*cos((2.0*float(x)+1.0)*float(u)*M_PI/16.0)*cos((2.0*float(y)+1.0)*float(v)*M_PI/16.0);
        f[x][y] = int(a);
    }
}

void JPEGimage::ImageDecompress(const char *BMPfileName)
{
    int DCval = 0;
    int zeroRL, value;
    int RL[64] = {0};
    int ZZ[64] = {0};
    int QF[8][8] = {0};
    int F[8][8] = {0};
    int f[8][8] = {0};
    int rl = 0;
    int EOB = 0;


    DCval += decodeDC();
    RL[rl++] = DCval;
    
    while(!EOB){
        decodeAC(&zeroRL, &value);
        RL[rl++] = zeroRL;
        RL[rl++] = value;
        EOB = ((zeroRL == 0)&&(value == 0))?1:0;
    }

    printData(ARRAY, "Zero Run-lenth", RL);
    RLED(ZZ, RL);
    printData(ARRAY, "ZigZag", ZZ);
    ZigZagD(QF, ZZ);
    printData(BLOCK, "Quantize", QF);
    QuantizeD(F, QF);
    printData(BLOCK, "DCT", F);
    DCTD(f, F);
    printData(BLOCK, "Block", f);
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
}

int main(void)
{
	JPEGimage jj;

	jj.loadJPGEimage("out.Ajpg");
    jj.ImageDecompress("out.bmp");
	
	return 0;
}