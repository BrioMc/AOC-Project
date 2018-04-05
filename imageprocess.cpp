/***************************************************************************
 *   Copyright (C) 2011 by pilar   *
 *   pilarb@unex.es   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "imageprocess.h"


void imageprocess::copiar(uchar * imgO, uchar * imgD)
{
	asm volatile(
        "movl %0, %%esi ;"
        "movl %1, %%edi ;"
        "movl $320*240, %%ecx;"
		"bcopia:"
        "movb (%%esi), %%al ;"
        "movb %%al, (%%edi) ;"
        "incl %%esi ;"
        "incl %%edi ;"
        "loop bcopia ;"
		: 
		: "m" (imgO),	"m" (imgD)
		: "%eax", "%ecx", "%esi", "%edi", "memory"

);


}


void imageprocess::negar(uchar * imgO, uchar * imgD)
{
	asm volatile(
        "movl %0, %%esi ;"          //  dirOrig=esi=imgO
        "movl %1, %%edi ;"          //  dirDest=edi=imgD
        "movl $320*240, %%ecx;"     //  para p<320*240
        "bnegar:"
        "movb $255, %%al ;"         //  255
        "subb (%%esi),%%al ; "      //  255-[dirOrig]
        "movb %%al, (%%edi) ;"      //  [dirDest]=255-[dirOrig]
        "incl %%esi ;"              //  dirOrig++
        "incl %%edi ;"              //  dirDest++
        "loop bnegar ;"
        :
        : "m" (imgO),	"m" (imgD)
        : "%eax", "%ecx", "%esi", "%edi", "memory"
);


}

void imageprocess::recortar(uchar * imgO, uchar * imgD, int cI, int fI, int w, int h)
{
    int f,c;
    asm volatile(
                "movl %0, %%esi ;"   	//  disOri = imgo
                "movl %1, %%edi ;"   	//  disDes = imgd

                "movl $320, %%eax ;"
                "imull %3, %%eax ;"
                "addl %2, %%eax ;"
                "addl %%eax, %%esi ;"    // dirOri= dirOri+320*fI+cI

                "movl $240, %%eax ;"     //
                "subl %5, %%eax ;"       //
                "shrl $1, %eax ;"         //  ebx=(240-h)/2 =despFila
                "movl %%eax, %%ebx ;"    //

                "movl $320, %%eax ;"     //
                "subl %4, %%eax ;"       //
                "shrl $1, %eax ;"        //  ecx=(320-w)/2 =despColum
                "movl %%eax, %%ecx ;"    //
                "movl $0, %%eax ;"     //    0   eax


                "movl $0, %6 ;"          //  f=0
                "LR1:"                   	//  para f=0,

                "movl $0, %7 ;"          //  c=0
                "cmpl %6, %%ebx ;"       //  if(
                "jg LR3 ;"               //  despFila<=f )
                "movl $240, %%eax ;"      //
                "movl %6, %%edx ;"      //

                "subl %%edx, %%eax ;"       //
                "cmpl %%eax, %%ebx ;"    //  if(
                "jge LR3 ;"              //  despFila<240-f

                "movl $0, %7 ;"          //  c=0
                "LR2:"                      //  para c=0

                "cmpl %7, %%ecx ;"       //  if(
                "jg NotI ;"              //  despColm<c )
                "movl $320, %%eax ;"      //
                "subl %7, %%eax ;"       //
                "cmpl %%eax, %%ecx ;"       //  if(
                "jge NotI ;"             //  despColm<320-c
                "movl $0, %%eax ;"
                "movb (%%esi), %%al ;"   //  al=[distOrig]
                "movb %%al ,(%%edi) ;"   //  [distDes]=al
                "incl %%esi ;"           //  dirOrig++

                "jmp FinIf ;"
                "NotI:"                     //  else
                "movb $0, (%%edi) ;"     //  [dirDest]=0

                "FinIf:"
                "incl %%edi ;"           //  dirDest++

                "incw %7 ;"            //  c++
                "cmpw $320, %7 ;"      //  320>c
                "jl LR2 ;"

                "movl %%ecx, %%eax ;"    //  eax=despCol
                "imull $2, %%eax ;"      //  eax*2
                "addl %%eax, %%esi ;"    //  dirOrig+=eax
                "jmp FinR1 ;"

                "LR3:"                      //  para c=0

                "movb $0,(%%edi) ;"      //  [distDest]=0
                "incl %%edi ;"           //  disDest++

                "incl %7 ;"            //  c++
                "cmpl $320, %7 ;"      //  320>c
                "jl LR3 ;"

                "FinR1:"
                "incl %6 ;"            //  f++
                "cmpl $240, %6 ;"      //  240>f
                "jl LR1 ;"

        :
        : "m" (imgO),	"m" (imgD), "m" (cI), "m" (fI), "m" (w), "m" (h), "m" (f), "m" (c)
        : "%eax","%ebx", "%ecx","%edx", "%esi", "%edi", "memory"

);


}


void imageprocess::voltearHorizontal(uchar * imgO, uchar * imgD)
{
	asm volatile(
                "movl %0, %%esi ;"   	// disOri = imgo
                "movl %1, %%edi ;"   	// disDes = imgd
                "addl $319, %%esi ;"     // disOri= disOri+319
                "movl $0, %%ebx ;"       // f=0
                "LH1:"
                "movl $0, %%ecx ;"       // c=0
                    "LH2:"
                        "movb (%%esi),%%al ;"    //al=[dirOri]
                        "movb %%al,(%%edi) ;"    //[dirDes]=al
                        "decl %%esi ;"           //dirOrig--
                        "incl %%edi ;"           //dirDest++
                        "incl %%ecx ;"           //c++
                        "cmp $320,%%ecx ;"       //c<320
                    "jl LH2 ;"
                "addl $640, %%esi ;"             //dirOri=dirOri+640
                "incl %%ebx ;"                   //f++
                "cmp $240,%%ebx ;"               //f<240
                "jl LH1 ;"

        :
	: "m" (imgO),	"m" (imgD)
        : "%eax","%ebx", "%ecx", "%esi", "%edi", "memory"

);


}

void imageprocess::voltearVertical(uchar * imgO, uchar * imgD)
{
	asm volatile(
                "movl %0, %%esi ;"   	// disOri = imgo
                "movl %1, %%edi ;"   	// disDes = imgd
                "movl $239*320, %%eax ;"     // eax=239*320
                "addl %%eax, %%esi ;"     // disOri= disOri+eax
                "movl $0, %%ebx ;"       // f=0
                "LV1:"
                "movl $0, %%ecx ;"       // c=0
                    "LV2:"
                        "movb (%%esi),%%al ;"    //al=[dirOri]
                        "movb %%al,(%%edi) ;"    //[dirDes]=al
                        "incl %%esi ;"           //dirOrig++
                        "incl %%edi ;"           //dirDest++
                        "incl %%ecx ;"           //c++
                        "cmp $320,%%ecx ;"       //c<320
                    "jl LV2 ;"
                "subl $640, %%esi ;"             //dirOri=dirOri-640
                "incl %%ebx ;"                   //f++
                "cmp $240,%%ebx ;"               //f<240
                "jl LV1 ;"

        :
    : "m" (imgO),	"m" (imgD)
        : "%eax","%ebx", "%ecx", "%esi", "%edi", "memory"

);


}


void imageprocess::girarIzquierda(uchar * imgO, uchar * imgD)
{

	asm volatile(
                "movl %0, %%esi ;"   	// disOri = imgo
                "movl %1, %%edi ;"   	// disDes = imgd
                "movl $-120, %%ebx ;"       // fr=-120
                "LI1:"
                "movl $-160, %%ecx ;"       // cr=-160
                "movl $-40*320, %%eax ;"
                "addl $160, %%eax ;"
                "subl %%ebx, %%eax ;"
                                            // posPixel= -40*320+160-fr
                    "LI2:"
                        "cmp $0, %%eax ;"
                        "jl LINot ;"
                        "movl $320*240, %%edx ;"
                        "cmp %%edx, %%eax ;"
                        "jge LINot ;"
                            "movb (%%esi,%%eax),%%dl ;"  //
                            "movb %%dl, (%%edi) ;"            //[dirDest]=[dirOrig+postPixel]
                        "jmp LIFin ;"
                        "LINot:"
                        "movb $0,(%%edi) ;"    //[dirDes]=0
                        "LIFin:"
                        "incl %%edi ;"           //dirOrig++
                        "addl $320, %%eax ;"
                        "incl %%ecx ;"           //cr++

                        "cmp $160,%%ecx ;"       //c<160
                    "jl LI2 ;"

                "incl %%ebx ;"                   //f++
                "cmp $120,%%ebx ;"               //f<120
                "jl LI1 ;"

        :
	: "m" (imgO),	"m" (imgD)
        : "%eax","%ebx", "%ecx", "%esi", "%edi", "memory"

);


}

void imageprocess::girarDerecha(uchar * imgO, uchar * imgD)
{

	asm volatile(
                "movl %0, %%esi ;"   	// disOri = imgo
                "movl %1, %%edi ;"   	// disDes = imgd
                "movl $-120, %%ebx ;"       // fr=-120
                "LD1:"
                "movl $-160, %%ecx ;"       // cr=-160
                "movl %%ecx, %%eax ;"
                "neg %%eax ;"
                "addl $120, %%eax ;"
                "imull $320, %%eax ;"
                "addl $160, %%eax ;"
                "addl %%ebx, %%eax ;"
                                            // posPixel= (-cr+120)*320+160+fr
                    "LD2:"
                        "cmp $0, %%eax ;"            // posPixel<0
                        "jl LDNot ;"                 //
                        "movl $320*240, %%edx ;"         //
                        "cmp %%edx, %%eax ;"         // posPixel<320*240
                        "jge LDNot ;"                    //If
                            "movb (%%esi,%%eax),%%dl ;"  //
                            "movb %%dl, (%%edi) ;"            //[dirDest]=[dirOrig+postPixel]
                        "jmp LDFin ;"
                        "LDNot:"                                //Else
                        "movb $0,(%%edi) ;"           //[dirDes]=0
                        "LDFin:"
                        "incl %%edi ;"           //dirDest++
                        "subl $320, %%eax ;"     // posPixel-=320
                        "incl %%ecx ;"           //cr++

                        "cmp $160,%%ecx ;"       //c<160
                    "jl LD2 ;"

                "incl %%ebx ;"                   //f++
                "cmp $120,%%ebx ;"               //f<120
                "jl LD1 ;"

        :
    : "m" (imgO),	"m" (imgD)
        : "%eax","%ebx", "%ecx", "%esi", "%edi", "memory"

);


}

void imageprocess::calcularHistograma(uchar * imgO, int * histoO)
{
    asm volatile(

                "movl %0, %%esi ;"   //  esi=imgO=dirOrig
                "movl %1, %%edi ;"   //  edi=histoO=dirHOrig


                "movl $0, %%ecx ;"   //  ecx=0=g
                "LCH1:"

                "movl $0, (%%edi, %%ecx,4) ;"  //  [dirHOrig+g]=0

                "incl %%ecx ;"       //  c++
                "cmpl $256, %%ecx ;" //  if
                "jl LCH1 ;"          //  c<256

                "movl $320*240, %%ecx ;"     //
                "LCH2:"                         //  loop if p<ecx

                "movzbl (%%esi), %%eax ;"    //  gOrig=[dirOrig]
                "incl (%%edi, %%eax,4) ;"      //  [dirHOrig+gOrig]++
                "incl %%esi ;"               //  dirOrig++

                "loop LCH2 ;"                //

        :
        : "m" (imgO),	"m" (histoO)
        :  "%eax","%ebx", "%ecx", "%esi", "%edi", "memory"

);

}

void imageprocess::calcularUmbral(int * histoO, uchar & umbral)
{

    int u,i,n1,n2,m1,m2,nt,mt;
    asm volatile(


                "movl $0, %0 ;"      //  umbral =0
                "movl %1, %%esi ;"   //  dirHOrig=esi=histo0
                "movl $0, %%edi ;"   //  maxDisp=0
                "movl $0, %2 ;"     //  u=0

                                    //para (u=0; u<256; u++)

                /************Bucle1************/

                "LU1:"

                //n1=0
                "movl $0, %4 ;"
                //media1=0
                "movl $0, %6 ;"

                //i=0
                "movl $0, %3 ;"

                /************Bucle2************/
                //para (i=0; i<=u; i++)
                "LU2:"

                "movl %3, %%eax ;"               //
                "movl (%%esi,%%eax,4), %%eax ;"  //  eax=[dirHOrig+i]
                "addl %%eax, %4 ;"               //  n1 =n1+eax

                "imull %3, %%eax ;"           //  [dirHOrig+i]*i
                "addl %%eax, %6 ;"               //  media1=media1+eax

                "incl %3 ;"
                "movl %3, %%eax ;"
                "cmpl %2, %%eax ;"
                "jle LU2 ;"
                /************FinBucle2************/


                "movl $0, %5 ;"                  //  n2=0
                "movl $0, %7 ;"                  //  media2=0

                "movl $0, %3 ;"                  //  i=0
                "movl %2, %%eax ;"               //  ecx=u
                "movl %%eax, %3 ;"               //  i=u
                "incl %3 ;"                      //  i=u+1


                /************Bucle3************/
                //para (i=u+1; i<256; i++)
                "LU3:"

                "movl %3, %%eax ;"               //
                "movl (%%esi,%%eax,4), %%eax ;"  //  eax=[dirHOrig+i]
                "addl %%eax, %5 ;"               //  n2=n2+eax

                "imull %3, %%eax ;"           //  [dirHOrig+i]*i
                "addl %%eax, %7 ;"               //  media2=media2+eax

                "incl %3 ;"                      //  i++
                "cmpl $256, %3 ;"
                "jl LU3 ;"
                /************FinBucle3************/


                /************If( n1>0 y n2>0 )************/

                "cmpl $0, %4 ;"
                "jle NotCU1 ;"                   //  n1>0
                "cmpl $0, %5 ;"
                "jle NotCU1 ;"                   //  n2>0

                /****nt=n1+n2****/

                "movl %4, %%eax ;"               //  n1
                "addl %5, %%eax ;"               //  n1+n2
                "movl %%eax, %8 ;"               //  nt=n1+n2

                /****mediat=(media1+media2)/nt****/


                "movl %6, %%eax ;"               //  media1
                "addl %7, %%eax ;"               //  media1+media2
                "cltd;"
                "divl %8 ;"                     //  (media1+media2)/nt
                "movl %%eax, %9 ;"               //  mediaT=(media1+media2)/nt
                /****media1=media1/n1****/

                "movl %6, %%eax ;"               //  media1
                "cltd;"
                "divl %4 ;"                     //  media1/n1
                "movl %%eax, %6 ;"               //  media1=media1/n1

                /****media2=media2/n2****/

                "movl %7, %%eax ;"               //  media2
                "cltd;"
                "divl %5 ;"                     //  media2/n2
                "movl %%eax, %7 ;"               //  media2=media2/n2

                /****dispersion=(n1*(media1-mediaT)²+n2*(media2-mediaT)²)/nT****/

                "movl $0, %%ecx ;"                 //  dis=0

                /**(media1-mediaT)²=(media1²-2media1mediaT+mediaT²)**/
                /**n1*(media1-mediaT)²=(n1*media1²+(n1*(-2media1mediaT)+n1*mediaT²) **/

                /*n2*mediaT²+n1*mediaT²-2*n2*media2*mediaT-2*n1*media1*mediaT+n2*media2²+n1*media1²*/


                "movl %6, %%eax;"               // eax=media1
                "subl %9, %%eax;"               // eax=media1-mediaT
                "imull %%eax;"                  // eax=eax*eax
                "imull %4, %%eax;"              // eax=n1*(media1-mediaT)²

                "movl %%eax, %%ecx ;"              //  dis=n1*(media1-mediaT)²

                "movl %7, %%eax;"               // eax=media2
                "subl %9, %%eax;"               // eax=media2-mediaT
                "imull %%eax;"                  // eax=eax*eax
                "imull %5, %%eax;"              // eax=n2*(media2-mediaT)²

                "addl %%eax, %%ecx;"            // dis=n1*(media1-mediaT)²+n2*(media2-mediaT)²

                "movl %%ecx, %%eax ;"
                "cltd;"
                "idivl %8 ;"                    // dis=(n1*(media1-mediaT)²+n2*(media2-mediaT)²)/nT
                "movl %%eax, %%ecx ;"



                /************If(dispersion>=MaxDisp************/

                "cmpl %%ecx, %%edi ;"              //dispersion>=MaxDisp
                "jg NotCU1 ;"                    // if(maxDisp<=disp)

                "movl %%ecx, %%edi ;"              //  maxDisp=disp

                "movb %2, %%al ; "               //  u
                "movb %%al, %0 ;"                //  umbral=u


                /************FinIf(************/

                /************FinIf(************/
                "NotCU1:"


                "incl %2 ;"          //  u++
                "cmpl $255, %2 ;"
                "jle LU1 ;"



/*int i=2,u,n1,n2,m1,m2,nt,dispersion;*/
        : "=m" (umbral)
        : "m" (histoO), "m"(u), "m"(i), "m"(n1), "m"(n2), "m"(m1), "m"(m2), "m"(nt), "m"(mt)
        :  "%eax","%ecx", "%edx","%esi","%edi", "memory"
    );

}

void imageprocess::umbralizar(uchar * imgO, uchar umbral, uchar * imgD)
{
    asm volatile(
                "movl %0, %%esi ;"   //  dirOrig=esi=imgO
                "movl %2, %%edi ;"   //  dirDest=edi=imgD
                "movl $320*240, %%ecx ;"
                "LU:"
                "movb %1, %%al ;"
                "cmpb %%al, (%%esi) ; "
                "jbe NotU ;"         //  if umbral<(dirOrig)

                "movb $255, (%%edi) ;"


                "jmp FinIU ;"
                "NotU:"                  //  else

                "movb $0, (%%edi) ;"

                "FinIU:"
                "incl %%esi ;"       //  dirOrig++
                "incl %%edi ;"       //  dirDest++

                "loop LU ;"


        :
        : "m" (imgO), "m" (umbral), "m" (imgD)
        :  "%eax","%ebx", "%ecx", "%esi", "%edi", "memory"
    );

}

