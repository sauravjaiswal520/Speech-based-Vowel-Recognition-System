#include<stdio.h>
#include "stdafx.h"
#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<utility>

#define ld long double 


void dc_shift_normalization(const char* s);
//void normalization(const char* s);

using namespace std;


int _tmain(int argc, _TCHAR* argv[])
{
	ld dc,normal;
	const char* s="a.txt";

	dc_shift_normalization(s);

	//normalization(s);
}

void dc_shift_normalization(const char* s)
{
	string name1="NDC",new_name;
	new_name = name1 + s;
	const char* new_string = new_name.c_str();
	int count=0,frames=0;
	ld temp,sum=0;ld max=0,temp1,factor;
	ifstream ip;
	ofstream op;
	ip.open(s);
	while(count<320*3)
	{
		ip>>temp;
		sum+=temp;
		count++;
	}
	sum /= 320*3;

	/// Normalization 
	//ip.seekg(0);
	ip.close();
	ip.open(s);
	while(ip)
	{
		ip>>temp;
		if(abs(temp)>max)
			max = abs(temp);
	}
	factor = (ld)10000/max;
	//ip.seekg(0);
	ip.close();
	ip.open(s);
	op.open(new_string);
	while(ip)
	{
		ip>>temp;
		temp1  = (temp-sum)*factor;
		op<<temp1<<endl;
	}
	op.close();
	//remove(s);
	//rename("dc.txt",s);
	cout<<" dc factor :"<<sum<<endl;
	
	return;
}