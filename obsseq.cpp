#include<stdio.h>
#include "stdafx.h"
#include<iostream>
#include<fstream>
#include<string>
#include<vector>


using namespace std;


#define ld long double
#define threshhold 5000


void calc_ci(string);
void calc_ci_and_store_in_vector(string);
string dc_shift_normalization(string s);
void hamming_raisedsine();
void average(string);
void tokhura(vector<vector<ld>>, vector<vector<ld>>);
void testing(string s);
void store_ref_in_vector(string);
void training();


ld r[13],k[13],e[13],a[13][13],amp,temp=0,A[13],c[13];
ld raisedsine[13],hamming_w[320];
ld distance;

vector<ld> signal(320);
vector<vector<ld>> ci;                      // ci's of 85 frames(85 X 12)
vector<pair<int,int>> marker;
vector<vector<ld>> reference;               // contains codeboook (32 X 12)


int _tmain(int argc, _TCHAR* argv[])
{	
	hamming_raisedsine();
    calc_ci("194101035_1_01.txt");
    store_ref_in_vector("codebook.txt");
	tokhura(reference,ci);
	return 0;
}

void calc_ci(string s)
{	
	vector<ld> vctr;
	/// Calculating ai's
	int no_of_frame=0;
	ifstream ip;
	ip.open(s);
    bool flag = true;
	while(no_of_frame<85)
	{	
		no_of_frame++;	
		int i=0,count=0;
		while(ip>>amp && count!=320 && flag)
		{
			signal[i] = amp*hamming_w[i];
			//signal[i] = amp;
			i++;
			count++;
            flag = false;
		}
        signal.erase(signal.begin(),signal.begin()+81);
        while(!flag && count!=80 && ip>>amp)
        {
            signal.push_back(amp);
        }

		for(i=0;i<13;i++)
		{
			r[i]=0;
			for(int j=0;j<=319-i;j++)
			{
				r[i] += signal[j]*signal[j+i];
			}
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
			vctr.push_back(c[i]*raisedsine[i]);
			//vctr.push_back(c[i]);

		ci.push_back(vctr);
		vctr.clear();
	}
	return ;
}

string dc_shift_normalization(string s)
{
	string name1="NDC",new_name;
	new_name = name1 + s;
	int count=0,frames=0;
	ld temp,sum=0;ld max=0,temp1,factor;
	ifstream ip;
	ofstream op;
	ip.open(s);
	/// calculating dc shift for first 3 frames(silence)
	while(count<320*3)
	{
		ip>>temp;
		sum+=temp;
		count++;
	}
	sum /= 320*3;

	//Normalization
	ip.close();
	ip.open(s);
	while(ip)
	{
		ip>>temp;
		if(abs(temp)>max)
			max = abs(temp);
	}
	factor = (ld)10000/max;
	ip.close();
	ip.open(s);
	op.open(new_name);
	while(ip)
	{
		ip>>temp;
		temp1  = (temp-sum)*factor;
		op<<temp1<<endl;
	}
	op.close();

	return new_name;
}

void hamming_raisedsine()
{
	/// hamming window
	for(int j=0;j<320;j++)
	{
		hamming_w[j]=0.54-0.46*(cos(2*3.14*j)/(ld)319);
	}

	/// raised sine window
	for(int i=1;i<=12;i++)
	{
		raisedsine[i] = 1+ 6*sin((3.14*i)/12);
	}
	return;
}


void tokhura(vector<vector<ld>> ci_training, vector<vector<ld>> ci_testing)
{
	ld difference,tokhura_distance,final_distance=0;
	ld weight_t[] = {1.0,3.0,7.0,13.0,19.0,22.0,25.0,33.0,42.0,50.0,56.0,61.0};
    int count=0;
    int row_no;
    while(count++ <85)
    {
        ld min=INT_MAX;
        for(int i=0;i<ci_training.size();i++)
        {
            tokhura_distance = 0;

            for(int j=0;j<12;j++)
            {
                difference = ci_training[i][j]-ci_testing[count][j];
                tokhura_distance += (difference*difference*weight_t[j]);
            }
            if(tokhura_distance<min)
            {
                min = tokhura_distance;
                row_no = i+1;
            }
        }
        cout<<row_no<<"\t";
    }
}


void store_ref_in_vector(string s)
{
	reference.clear();
	ld w;
	ifstream ip;
	ip.open(s);
	vector<ld> vctr;
	for(int i=0;i<32;i++)
	{
		for(int j=0;j<12;j++)
		{
			ip>>w;
			vctr.push_back(w);
		}
		reference.push_back(vctr);
		vctr.clear();
	}
}
