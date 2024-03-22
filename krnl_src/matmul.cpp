#include <iostream>
#include "matmul.h"




//void matmul(uint32_t a[][COLA],uint32_t b[][COLB],uint32_t out[][COLB]){//,uint32_t rowa,uint32_t cola,uint32_t colb ){
void matmul(uint32_t* a,uint32_t* b,uint32_t* out,uint32_t a_row,uint32_t a_col,uint32_t b_col){
int b_row=a_col;
int c_row=a_row;
int c_col=b_col;
int l=0;

const unsigned int c_size=MAX_SIZE;

#pragma HLS INTERFACE mode=m_axi bundle=gmem1 port=a depth=6	///doesnt work without setting depth
#pragma HLS INTERFACE mode=m_axi bundle=gmem0 port=b depth=6
#pragma HLS INTERFACE mode=m_axi bundle=gmem1 port=out depth=6

int atemp[MAX_SIZE][MAX_SIZE];
int btemp[MAX_SIZE][MAX_SIZE];
int outtemp[MAX_SIZE][MAX_SIZE];

#pragma HLS ARRAY_PARTITION variable=atemp dim=1 type=complete
#pragma HLS ARRAY_PARTITION variable=btemp dim=2 type=complete
#pragma HLS ARRAY_PARTITION variable=outtemp dim=0 type=complete

/*#pragma HLS bind_storage type=RAM_S2P variable=atemp impl=LUTRAM
#pragma HLS bind_storage type=RAM_S2P variable=btemp impl=LUTRAM
#pragma HLS bind_storage type=RAM_S2P variable=outtemp impl=LUTRAM*/


///////// Initialize output to all zeros ///////////
initzero:for(int i = 0; i < MAX_SIZE;i++){

	{
	for(int j = 0; j < MAX_SIZE;j++)
	outtemp[i][j]=0;
}
}


/////////// Load a to local memory //////////////
loada:for(int i=0,j=0,z=0;z<MAX_SIZE*MAX_SIZE;j++,z++){
#pragma HLS LOOP_TRIPCOUNT min=c_size*c_size max=c_size*c_size
				if(j==a_col){
					i++;
					j=0;
				}
				//atemp[i][j]=(z<a_row*a_col) ? a[z] : 0;
				if(z<a_row*a_col){
				atemp[i][j]=a[z];
				}


}


/////////// Load b to local memory //////////////

loadb:for(int i=0,j=0,z=0;z<MAX_SIZE*MAX_SIZE;j++,z++){
#pragma HLS LOOP_TRIPCOUNT min=c_size*c_size max=c_size*c_size
				if(j==b_col){
					i++;
					j=0;
				}
				if(z<b_row*b_col){
				btemp[i][j]=b[z];
				}
				//btemp[i][j]=(z<b_row*b_col) ? b[z] : 0;
}



/////////// Matrix multiplication //////////////

rowa:	for (uint32_t i=0;i<a_row;i++){
#pragma HLS LOOP_TRIPCOUNT min=c_size max=c_size
			//if(i<a_row){
colb:		for(uint32_t j=0;j<b_col;j++){
#pragma HLS LOOP_TRIPCOUNT min=c_size max=c_size

cola:			for(uint32_t z=0;z<MAX_SIZE;z++){
#pragma HLS LOOP_TRIPCOUNT min=c_size max=c_size

				outtemp[i][j]+=(z<a_col) ? (atemp[i][z]*btemp[z][j]) : 0;




		}
			}
			}



////////////Send results to global memory /////////////
sendout:for(int i=0,j=0,z=0;z<MAX_SIZE*MAX_SIZE;j++,z++){
#pragma HLS LOOP_TRIPCOUNT min=c_size*c_size max=c_size*c_size
	if(j==c_col){
		i++;
		j=0;
	}
	out[z]=(z<c_row*c_col) ? outtemp[i][j] : 0;
}


}
