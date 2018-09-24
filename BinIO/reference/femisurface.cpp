#include <stdio.h> 
#include <stdlib.h> 
#include <math.h> 
#include <iostream> 
#include <fstream> 
#include <string> 
#include <strings.h> 
#include <cstring> 
#include <cstdlib> 
using namespace std; 

#define NKPTS_MAX 100000 
#define NBANDS_MAX 2000 

int main ( int argc, char *argv[]) 
{ 
if( argc < 4) 
{ 
printf("INPUT FORMAT: \n"); 
printf("./v2xsf.exe nkx nky nkz \n"); 
exit(0); 
} 
int nkx, nky,nkz; 
nkx=atoi(argv[1]); 
nky=atoi(argv[2]); 
nkz=atoi(argv[3]); 

fstream fid; 
fid.open("OUTCAR",ios::in); 
if ( !fid ) 
{ 
printf("cant open OUTCAR\n"); 
exit(0); 
} 

char str[255], stmp[255]; 

//keyword1[]="NKPTS"; 
int NKPTS,NBANDS; 
do { 
fid.getline(str,255); 
if(strstr(str,"NKPTS") != NULL ) 
{ 
printf("str=%s\n", str); 
sscanf(str,"%s%s%s%d",stmp,stmp,stmp,&NKPTS); 
sscanf(str,"%s%s%s%s%s%s%s%s%s%s%s%s%s%s%d",stmp,stmp,stmp,stmp,stmp,stmp,stmp,stmp,stmp,stmp,stmp,stmp,stmp,stmp,&NBANDS); 
printf("NKPTS: %d, NBANDS: %d\n",NKPTS,NBANDS); 
break; 
} 
}while( !fid.eof()) ; 



float A[3][3], B[3][3]; 
do { 
fid.getline(str,255); 
if(strstr(str,"reciprocal lattice vectors") != NULL ) 
{ 
for(int i=0;i<3;i++) 
{ 
fid.getline(str,255); 
sscanf(str,"%f%f%f%f%f%f",&A[i][0],&A[i][1],&A[i][2],&B[i][0],&B[i][1],&B[i][2]); 
} 
printf("A1: %f %f %f \n",A[0][0],A[0][1],A[0][2]); 
printf("A2: %f %f %f \n",A[1][0],A[1][1],A[1][2]); 
printf("A3: %f %f %f \n",A[2][0],A[2][1],A[2][2]); 
printf("B1: %f %f %f \n",B[0][0],B[0][1],B[0][2]); 
printf("B2: %f %f %f \n",B[1][0],B[1][1],B[1][2]); 
printf("B3: %f %f %f \n",B[2][0],B[2][1],B[2][2]); 

break; 
} 
}while( !fid.eof()); 



float Ef; 
do { 
fid.getline(str,255); 
if(strstr(str,"E-fermi") != NULL ) 
{ 
printf("str=%s\n", str); 
sscanf(str,"%s%s%f",stmp,stmp,&Ef); 
printf("Ef: %f\n",Ef); 
break; 
} 
}while( !fid.eof()) ; 



fid.getline(str,255); 
fid.getline(str,255); 

float kpt[NKPTS_MAX][3]; 
float Egv[NKPTS_MAX][NBANDS_MAX]; 
for(int ik =0; ik < NKPTS; ik++) 
{ 
// k-point 1: kx ky kz 
fid.getline(str,255); 
sscanf(str,"%s%s%s%f%f%f",stmp,stmp,stmp,&kpt[ik][0],&kpt[ik][1],&kpt[ik][2]); 
fid.getline(str,255); 

for(int ib=0;ib<NBANDS;ib++) 
{ 
int bandno,occ; 
fid.getline(str,255); 
sscanf(str,"%d%f%f",&bandno,&Egv[ik][ib],&occ); 
// printf("%d %d : %f\n", ik,ib,Egv[ik][ib]); 
} 

fid.getline(str,255); 
} 

fid.close(); 


// find those cross Ef 

int Nf=0; 
static int nstore[NBANDS_MAX]; 
for( int ib =0 ; ib < NBANDS; ib ++) 
{ 
float vmin=10000, vmax=-10000; 
for( int ik =0 ; ik < NKPTS ; ik++ ) 
{ 
if ( Egv[ik][ib] < vmin) 
{ vmin = Egv[ik][ib];} 
if( Egv[ik][ib] > vmax ) 
{ vmax= Egv[ik][ib];} 
} 
if( vmax > Ef && vmin < Ef) 
{ nstore[Nf++]=ib; } 

} 
int ib; 
float Egvf[NKPTS][Nf]; 
for(int i=0; i<Nf; i++) 
{ 
ib=nstore[i]; 
for( int ik=0 ; ik < NKPTS ; ik ++ ) 
{ 
Egvf[ik][i]=Egv[ik][ib]; 
} 
} 

//save to bxsf 

FILE *fid2; 
fid2=fopen("FS.bxsf","w"); 
if( fid2 == NULL) 
{ 
printf( "can't create fout: FS.bxsf\n"); 
exit(0); 
} 

fprintf(fid2,"BEGIN_INFO\n"); 
fprintf(fid2," Fermi Energy: %7.5f\n",Ef); 
fprintf(fid2,"END_INFO\n"); 


fprintf(fid2,"\n"); 

fprintf(fid2,"BEGIN_BLOCK_BANDGRID_3D\n"); 
fprintf(fid2," one_word_comment \n"); 
fprintf(fid2," BEGIN_BANDGRID_3D\n"); 
fprintf(fid2," %d \n",Nf); 
fprintf(fid2," %d %d %d \n",nkx,nky,nkz); 
fprintf(fid2," 0 0 0\n"); // origin Gamma 
fprintf(fid2," %f %f %f \n", B[0][0],B[0][1],B[0][2]);//Vec(1,:)); 
fprintf(fid2," %f %f %f \n", B[1][0],B[1][1],B[1][2]); 
fprintf(fid2," %f %f %f \n", B[2][0],B[2][1],B[2][2]); 

int ik = 0; 
for(int i=0; i < Nf; i++) 
{ 
fprintf(fid2, " BAND: %d \n",nstore[i]+1); 
for(int ix=0 ; ix < nkx; ix ++) 
{ 
for(int iy=0 ; iy < nky; iy ++) 
{ 
for(int iz=0 ; iz <nkz; iz++) 
{ 
fprintf(fid2," %f ",Egvf[ik++][i]); 
} 
fprintf(fid2,"\n"); 
} 
fprintf(fid2,"\n"); 
} 

} 

fprintf(fid2," END_BANDGRID_3D\n"); 
fprintf(fid2,"END_BLOCK_BANDGRID_3D\n"); 


fclose(fid2); 

} 
