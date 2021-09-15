#include "stdafx.h"
#include<stdio.h>
#include<iostream>
#include<fstream>

using namespace std;

void calc_ai();

long double signal[320],r[13],k[13],e[13],a[13][13],amp,temp=0,A[13],c[13];


int main()
{
	calc_ai();
	return 0;
}



void calc_ai()
{
	ifstream fp;
	fp.open("test.txt");
	
	//int temp=0;

	/// Calculating ai's
	int i=0;
	while(fp>>amp)
	{
		signal[i] = amp;
		i++;
	}
	for(i=0;i<13;i++)
	{
		r[i]=0;
		for(int j=0;j<=319-i;j++)
		{
			r[i] += signal[j]*signal[j+i];
		}
	}

	for(i=0;i<13;i++)
	{
		cout<<"R["<<i<<"] = ";
		printf("%.6Lf\n",r[i]);
	}

	e[0] = r[0];
	k[0]=0;
	a[0][0] = 0;
	for(int i=1;i<=12;i++)
	{
		temp=0;
		for(int j=1;j<=i-1;j++)
			temp+=a[j][i-1] * r[i-j];
		k[i] = (r[i]-temp)/e[i-1];
		a[i][i]=k[i];
		for(int j=1;j<=i-1;j++)
		{
			if(j!=i)
			{
				a[j][i] = a[j][i-1]-(k[i] * a[i-j][i-1]);
			}
		}
		e[i] = (1-k[i]*k[i])*e[i-1];
	}
	
	for(int i=1;i<=12;i++)
		A[i] = a[i][12];

	for(int i=1;i<=12;i++)
		printf("A%d = %Lf\n",i,A[i]);
	for(int i=1;i<=12;i++)
		A[i] = (-1)*A[i];

	for(int m=1;m<=12;m++)
	{
		temp = 0;
		for(int k=1;k<=m-1;k++)
		{
			temp += (k*c[k]*A[m-k])/m;
		}
		c[m] = A[m] + temp;
	}

	for(int i=1;i<=12;i++)
		printf("c%d = %Lf\n",i,c[i]);
	return ;
}
