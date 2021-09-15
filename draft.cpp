/// 


#include<stdio.h>
#include "stdafx.h"
#include<iostream>
#include<fstream>
#include<string>
#include<vector>


using namespace std;


#define ld long double
#define threshhold 5000


void calc_ai_ci(string,long int);
void calc_ci_and_store_in_vector(string);
string dc_shift_normalization(string s);
void hamming_raisedsine();
void average(string);
void tokhura(vector<vector<ld>>, vector<vector<ld>>);
void testing(string s);
void store_ref_in_vector(string);
void training();


ld signal[320],r[13],k[13],e[13],a[13][13],amp,temp=0,A[13],c[13];
ld raisedsine[13],hamming_w[320];
ld distance;

vector<vector<ld>> ci;
vector<pair<int,int>> marker;
vector<vector<ld>> reference;



/// driver function
int _tmain(int argc, _TCHAR* argv[])
{	
	string testfile;

	hamming_raisedsine();
	
	//training();											/// comment this part if you dont want to perform training
	cout<<"Training finished"<<endl;

	/// testing 5 different files !!!!
	string vowels[5] = {"a","e","i","o","u"};
	for(int i=0;i<5;i++)
	{
		testfile = "204101058_"+vowels[i]+"_test.txt";
		testing(testfile);
	}
	
	//testing("aeiou.txt");
	return 0;
}



/**	This function reads 5 files for training applies dc shift and normalization on each file
  * Then in each file there are 10 utterances of a vowel
  * For each vowel we call a function for calculating ci for each utterance, averaging it and storing it in a reference file 
  * 
  */
void training()
{

	string s,ref,tempstring;
	int vowel_no=0;
	
	while(vowel_no<5)
	{
		if(vowel_no==0)
		{
			s="204101058_a.txt";
			ref = "204101058_a_ref.txt";
		}
		else if(vowel_no==1)
		{
			s="204101058_e.txt";
			ref = "204101058_e_ref.txt";
		}
		else if(vowel_no==2)
		{
			s="204101058_i.txt";
			ref = "204101058_i_ref.txt";
		}
		else if(vowel_no==3)
		{
			s="204101058_o.txt";
			ref = "204101058_o_ref.txt";
		}
		else if(vowel_no==4)
		{	
			s="204101058_u.txt";
			ref = "204101058_u_ref.txt";
		}
		cout<<"Training "<<s<<endl;

		tempstring = dc_shift_normalization(s);
		
		calc_ci_and_store_in_vector(tempstring);
		
		reference = ci;

		average(ref);						/// calculates avg and stores it in a reference file
		
		vowel_no++;

		ci.clear();
	}
	return;
}



/** this function takes a vowel file as input and calculates ci's for 5 frames of each of the 10 utterances 
  * firstly we detect the  boundary of each utterance and then pick 5 most stable frames of that utterance and
  * calls a fn to calculate its ci's and store it in a vector of vector(each of size 12)
  */
void calc_ci_and_store_in_vector(string s)
{
	ifstream ip;										/// creating object of ifstream
	//ofstream op;
	//pair<int,int> p;
	ip.open(s);											/// opening file
	ld amplitude,ste=0,max_ste=0;
	bool flag=false;
	int frame_count=0,sample_count=0,count=0,max_ste_frame_no=0;
	long int start_stable = ip.tellg(),start_current=ip.tellg(),start_prev=ip.tellg();

	/** Three pointer approach is used here to point to start of stable part of utterance (2 frames before highest ste frame)
	  *	Start_stable points to the start of stable part encountered till now
	  */
	while(ip)
	{	
		if(max_ste == ste)
		{
			start_stable = start_prev;
		}
		start_prev = start_current;
		start_current = ip.tellg();
		ste = 0;
		while(count<320)											
		{
			count++;
			sample_count++;
			ip>>amplitude;
			ste+=amplitude*amplitude;
		}
		frame_count++;
		ste/=320;
		//cout<<" STE "<<ste<<" ";
		if(ste>max_ste)								
		{
			max_ste=ste;
			max_ste_frame_no = frame_count;
		}
		if(ste>20000 && flag == false)					/// starting of a word(utterance) is detected and therefore flag is set true
		{
			flag =true;
		}
		if(ste<20000 && flag == true)					/// Ending of the word(utterance) is marked here.. flag is set to false
		{												
			flag = false;
			calc_ai_ci(s,start_stable);					/// for this utterance ci are calculated and stored in a vector
			max_ste = 0;
		}
		count=0;
	}
	ip.close();											/// Closing file
}


/**	Calculates values of ai and ci's for a frame(320 values)
  * Applies hamming window on signal before calculating ai's
  * then after calculating ci's applying raised sine window and updating the values of ci's
  * finally storing it inside a vector ci
  */
void calc_ai_ci(string s,long int pos)
{	
	vector<ld> vctr;
	/// Calculating ai's
	int no_of_frame=0;
	ifstream ip;
	ip.open(s);
	ip.seekg(pos,ios::beg);
	while(no_of_frame<5)
	{	
		no_of_frame++;		
		int i=0,count=0;
		while(ip>>amp && count!=320)
		{
			signal[i] = amp*hamming_w[i];
			//signal[i] = amp;
			i++;
			count++;
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



/**	Takes a name(string) as parameter, opening that text file and applying dc shift and normalisation on it
  * After appyling dc shit and normalization on it we create new txt file for storing the normalized values
  * Name of new file starts from "NDC" returns this filename as this file will be used instead of orignial 
    training file.
  */
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


/// Storing values of hamming window an raised sine window in an array
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


/// Averages vector ci and finally stores 5 rows(each having 12 values) in a reference file.
void average(string ref)
{
	int temp =0;
	ofstream op;
	while(temp<=4)							/// storing avg values of ci's in first 5 rows
	{
		for(int j=0;j<12;j++)
		{
			for(int i=temp+5;i<50;i+=5)
			{
				ci[temp][j]+=ci[i][j];
			}
		}
		for(int j=0;j<=11;j++)
		{
			ci[temp][j]/=10;
		}
		temp++;
	}

	op.open(ref);
	for(int i=0;i<=4;i++)					/// storing avg ci values of a vowel in a file
	{
		for(int j=0;j<=11;j++)
		{
			op<<ci[i][j]<<" ";
		}
		//while(count<320)
		op<<endl;
	}
	op.close();
	return;
}



void testing(string s)
{
	cout<<"File being curently tested is "<<s<<endl;
	vector<vector<ld>> ci_test;
	string new_s;
	new_s = dc_shift_normalization(s);
	s = new_s;
	calc_ci_and_store_in_vector(s);/// Now my ci vector has ci of 5frames for each utterance(10)
	string vowel[] = {"a","e","i","o","u"};
	string output="F";
	ld min=100000000000000;
	for(int j=0;j<10;j++)
	{	
		for(int k=5*j;k<5*j+5;k++)
		{
			ci_test.push_back(ci[k]);
		}

		for(int i=0;i<5;i++)
		{
			string s = vowel[i]+"_ref.txt";
			
			store_ref_in_vector(s);	/// stores the values of 
			
			tokhura(reference,ci_test);

			cout<<::distance<<" from "<<vowel[i]<<"\t"<<endl;
			if(::distance<min)
			{
				min = ::distance;
				output = vowel[i];
			}
			reference.clear();
		}
		ci_test.clear();
		cout<<output<<endl<<endl;
	}
	ci.clear();
	return;
}



void tokhura(vector<vector<ld>> ci_training, vector<vector<ld>> ci_testing)
{
	ld difference,tokhura_distance,final_distance=0;
	ld weight_t[] = {1.0,3.0,7.0,13.0,19.0,22.0,25.0,33.0,42.0,50.0,56.0,61.0};
	for(int i=0;i<ci_training.size();i++)
	{
		tokhura_distance = 0;

		for(int j=0;j<12;j++)
		{
			difference = ci_training[i][j]-ci_testing[i][j];
			tokhura_distance += (difference*difference*weight_t[j]);
		}
		final_distance += tokhura_distance;
	}
	::distance = final_distance;
}



void store_ref_in_vector(string s)
{
	reference.clear();
	ld w;
	ifstream ip;
	ip.open(s);
	vector<ld> vctr;
	for(int i=0;i<5;i++)
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