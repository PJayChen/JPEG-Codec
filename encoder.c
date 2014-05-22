#include <stdio.h>
#include <math.h>
#include <string.h>

#define abs(x) ( ( (x) < 0)? -(x):(x) )

int myIntRound(float dInput)
{
    if(dInput >= 0.0f)
    {
        return ((int)(dInput + 0.5f));
    }
    return ((int)(dInput - 0.5f));
}

float C(int u)
{
  if(u==0)
    return (1.0/sqrt(2.0));
  else
    return 1.0;
}
 
void DCT(int f[8][8],int F[8][8])
{
  int u, v, x, y;
  float a;
  for(u=0;u<8;u++)
    for(v=0;v<8;v++)
      {
    	a = 0.0;
    for(x=0;x<8;x++)
      for(y=0;y<8;y++)
        a += (float)(f[x][y]) * cos( (2.0*(float)(x)+1.0)*(float)(u)*M_PI/16.0 ) * cos((2.0*(float)(y)+1.0)*(float)(v)*M_PI/16.0); 	
    	F[u][v] = myIntRound(0.25*C(u)*C(v)*a);
      }
}

void Quantize(int F[8][8], int QF[8][8])
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
    for(j=0;j<8;j++){
    	//QF[i][j] = (int)( ( F[i][j] + (int)(q[i][j] / 2) ) / q[i][j]);
    	QF[i][j] = myIntRound(F[i][j]/q[i][j]);
    	//QF[i][j] = (int)(F[i][j]/q[i][j]);
    }
}

void ZigZag(int QF[8][8],int ZZ[64])
{
  int i=0,j=0,k=0,d=0;
  while(k<36)
    {
      //      cout<<"["<<i<<","<<j<<"]";
      ZZ[k++] = QF[i][j];
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
  d = 0;
  while(k<64)
    {
      //      cout<<"["<<i<<","<<j<<"]";
      ZZ[k++] = QF[i][j];
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

int RLE(int ZZ[64],int RL[64])
{
  int rl=1;
  int i=1;
  int k = 0;
  RL[0] = ZZ[0];

  /*Find non-zero value from the end*/
  int end_non_zero = 64;
  while(end_non_zero-- >= 0){
  	if(ZZ[end_non_zero] != 0) break;
  }
  

  while(i<64 && i <= end_non_zero){
    k=0;
    while((i<64)&&(ZZ[i]==0)&&(k<15))
	  {
      i++;
	    k++;
	  }
      
    if(i==64){
	    RL[rl++] = 0;
	    RL[rl++] = 0;
	  }else{ 
	    RL[rl++] = k;
	    RL[rl++] = ZZ[i++];
	  }
  }
  //EOF
  if(i == end_non_zero){
  	RL[rl++] = 0;
    RL[rl++] = 0;
  }
  if(!(RL[rl-1]==0 && RL[rl-2]==0)){
    RL[rl++] = 0;
    RL[rl++] = 0;
  }
  /*
  if((RL[rl-4]==15)&&(RL[rl-3]==0)){
    RL[rl-4]=0;
    rl-=2;
  }
  */
  return rl;
}

int getCat(int a)
{
  if(a==0)
	 return 0;
  else if(abs(a)<=1)
	 return 1;
  else if(abs(a)<=3)
	 return 2;
  else if(abs(a)<=7)
	 return 3;
  else if(abs(a)<=15)
	 return 4;
  else if(abs(a)<=31)
	 return 5;
  else if(abs(a)<=63)
	 return 6;
  else if(abs(a)<=127)
	 return 7;
  else if(abs(a)<=255)
	 return 8;
  else if(abs(a)<=511)
	 return 9;
  else if(abs(a)<=1023)
	 return 10;
  else if(abs(a)<=2047)
	 return 11;
  else if(abs(a)<=4095)
	 return 12;
  else if(abs(a)<=8191)
	 return 13;
  else if(abs(a)<=16383)
	 return 14;
  else
	 return 15;
}

void getDCcode(int a,int& lenb,char *size_value)
{
  int codeLen[12] = {3,4,5,6,7,8,10,12,14,16,18,20};
  const char* code[12] = {"010","011","100","00","101","110","1110","11110","111110","1111110","11111110","111111110"};
  int cat = getCat(a); //get SIZE of DC
  lenb = codeLen[cat]; //pick out the Code length by SIZE. neme by Image Compression JPGE.pdf
  strcpy(size_value,code[cat]);
  int j;
  int c = a;

  /*If DC is negtive, get 1's Complement of DC*/
  if(a<0)
     c+=(int)pow(2,cat)-1;
  /*Code DC from decimal to binary*/
  for(j=lenb-1;j>lenb-cat-1;j--)
  {
     if(c%2==1)
	     size_value[j] = '1';
     else
	     size_value[j] = '0';
     c/=2;
  }
  size_value[lenb] = '\0';
}


/*
TODO: Detect No o between two AC value

*/
void getACcode(int n,int a, int& lenb, char* b)
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
    {"","1100","111001","1111001","111110110","11111110110","1111111110000100","1111111110000101","1111111110000110","1111111110000111","1111111110001000"},
    {"","11011","11111000","1111110111","1111111110001001","1111111110001010","1111111110001011","1111111110001100","1111111110001101","1111111110001110","1111111110001111"},
    {"","111010","111110111","11111110111","1111111110010000","1111111110010001","1111111110010010","1111111110010011","1111111110010100","1111111110010101","1111111110010110"},
    {"","111011","1111111000","1111111110010111","1111111110011000","1111111110011001","1111111110011010","1111111110011011","1111111110011100","1111111110011101","1111111110011110"},
    {"","1111010","1111111001","1111111110011111","1111111110100000","1111111110100001","1111111110100010","1111111110100011","1111111110100100","1111111110100101","1111111110100110"},
    {"","1111011","11111111000","1111111110100111","1111111110101000","1111111110101001","1111111110101010","1111111110101011","1111111110101100","1111111110101101","1111111110101110"},
    {"","11111001","11111111001","1111111110101111","1111111110110000","1111111110110001","1111111110110010","1111111110110011","1111111110110100","1111111110110101","1111111110110110"},
    {"","11111010","111111111000000","1111111110110111","1111111110111000","1111111110111001","1111111110111010","1111111110111011","1111111110111100","1111111110111101","1111111110111110"},
    {"","111111000","1111111110111111","1111111111000000","1111111111000001","1111111111000010","1111111111000011","1111111111000100","1111111111000101","1111111111000110","1111111111000111"},
    {"","111111001","1111111111001000","1111111111001001","1111111111001010","1111111111001011","1111111111001100","1111111111001101","1111111111001110","1111111111001111","1111111111010000"},
    {"","111111010","1111111111010001","1111111111010010","1111111111010011","1111111111010100","1111111111010101","1111111111010110","1111111111010111","1111111111011000","1111111111011001"},
    {"","1111111010","1111111111011010","1111111111011011","1111111111011100","1111111111011101","1111111111011110","1111111111011111","1111111111100000","1111111111100001","1111111111100010"},
    {"","11111111010","1111111111100011","1111111111100100","1111111111100101","1111111111100110","1111111111100111","1111111111101000", "1111111111101001","1111111111101010","1111111111101011"},
    {"","111111110110","1111111111101100","1111111111101101","1111111111101110","1111111111101111","1111111111110000","1111111111110001","1111111111110010","1111111111110011","1111111111110100"},
    {"111111110111","1111111111110101","1111111111110110","1111111111110111","1111111111111000","1111111111111001","1111111111111010","1111111111111011","1111111111111100","1111111111111101","1111111111111110"}
  };

  int cat = getCat(a);
  lenb = codeLen[n][cat];
  strcpy(b,code[n][cat]);
  int j;
  int c = a;
  
  if(a<0)
     c+=(int)pow(2,cat)-1;
  for(j=lenb-1;j>lenb-cat-1;j--)
  {
     if(c%2==1)
	     b[j] = '1';
     else
	     b[j] = '0';
     
     c/=2;
  }
  b[lenb] = '\0';
}

void Encode(int RL[64], int rl, char* output)
{
//   char output[33*26];
   char b[32];
   int bLen;
   getDCcode(RL[0],bLen,b);
   //   cout<<"Code : "<<RL[0]<<" "<<b;
   strcpy(output,b);
   int i;
   for(i=1;i<rl;i+=2)
   {
    	getACcode(RL[i],RL[i+1],bLen,b);
    	//	cout<<" , "<<RL[i]<<" "<<RL[i+1]<<" "<<b;
    	strcat(output,b);
   }
//   writeToFile(output);
}

int main(void)
{
	
	int test[8][8] = {
				  {139, 144, 149, 153, 155, 155, 155, 155},
				  {144, 151, 153, 156, 159, 156, 156, 156},
				  {150, 155, 160, 163, 158, 156, 156, 156},
				  {159, 161, 162, 160, 160, 159, 159, 159},
				  {159, 160, 161, 162, 162, 155, 155, 155},
				  {161, 161, 161, 161, 160, 157, 157, 157},
				  {162, 162, 161, 163, 162, 157, 157, 157},
				  {162, 162, 161, 161, 163, 158, 158, 158}
				  };

	int dctresult[8][8];

	int Qresult[8][8];
	int ZZresult[64];
	int RLEresult[64] = {0};
	int rlength;
	char out[1000] = {'0'};

	DCT(test, dctresult);
	Quantize(dctresult, Qresult);
	ZigZag(Qresult, ZZresult);
	rlength = RLE(ZZresult, RLEresult);
	Encode(RLEresult, rlength, out);

	int i=0, j=0;

  printf("8*8 Block: \n");
	for(i=0;i<8;i++){
		for(j=0;j<8;j++){
			
			printf("%d, ", test[i][j]);
		}
		printf("\n");
	}	
	printf("\nDCT: \n");
	for(i=0;i<8;i++){
		for(j=0;j<8;j++){
			
			printf("%d, ", dctresult[i][j]);
		}
		printf("\n");
	}
	printf("\nQuantize: \n");
	for(i=0;i<8;i++){
		for(j=0;j<8;j++){
			
			printf("%d, ", Qresult[i][j]);
		}
		printf("\n");
	}
	printf("\nZigZag: \n");
	for(i=0;i<64;i++){
		printf("%d ", ZZresult[i]);
	}
	printf("\n");
	printf("\nrl = %d", rlength);
  	printf("\nRun-length:\n");
	for(i=0;i<64;i++){
		printf("%d ", RLEresult[i]);
	}
	printf("\n\nEncode result: \n %s\n", out);

	return 0;
}
