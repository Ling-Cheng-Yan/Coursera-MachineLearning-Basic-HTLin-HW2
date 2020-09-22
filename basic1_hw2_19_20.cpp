
#include<iostream>
#include<fstream>
#include<vector>
#include<math.h>
#include<algorithm>
using namespace std;
 
#define DEMENSION 9 //数据维度
 
//数据结构体
struct Record{
	double x[DEMENSION];
	int y;
};
 
//取数据某一维度的x和数据的y组成的单维度，因为我们前面写的是单维度x的错误率计算，计算多维度只要分别取出每个维度计算就可以了
struct SingleDemension{
	double x;
	int y;
};
 
//hyphothesis格式
struct Hyphothesis{
	int s;
	double theta;
};
 
//从文件读入数据到Record向量中
void getData(fstream &datafile,vector<Record> &data){
	while(!datafile.eof()){
		Record temp;
		int i;
		for(i = 0; i < DEMENSION; i++)
			datafile>>temp.x[i];
		datafile>>temp.y;
		data.push_back(temp);
	}
	datafile.close();
}
 
//sign
int sign(double x){
	if(x <= 0)return -1;
	else return 1;
}
 
//定义比较方式
bool myCompare( SingleDemension &v1, SingleDemension &v2){
	return v1.x < v2.x;
}
 
//将输入的单维度X的数据进行排序，比较方法是我们自己定义的
void sortData(vector<SingleDemension> &data){
	sort(data.begin(),data.end(),myCompare);
}
 
//输入单维度X的数据进行计算其错误率
double calculateError(vector<SingleDemension> &data,Hyphothesis &h,int n){
	int i;
	int error = 0;
	for(i = 0; i < n; i++){
		int temp = h.s * sign(data[i].x - h.theta);
		if(temp != data[i].y)error++;
	}
	return error/double(n);
}
 
//分别对每个维度的每个hyphothesis计算错误率，记录最小错误率和最优hyphothesis，以及其对应的维度
double E_in(vector<Record> &trainingData,Hyphothesis &bestH,int &bestDemension,int trainingDataSize){
	int i;
	double min_errorRate = 1.0;
	Hyphothesis temp = {0,0};
	for(i = 0; i < DEMENSION; i++){       //分别对每个维度计算         
		vector<SingleDemension> singleDemension;
		int j;
		for(j = 0; j < trainingDataSize; j++){   //取出此维度的x，取出y组合形成新的单维度x数据
			SingleDemension temp = {0,0};
			temp.x = trainingData[j].x[i];
			temp.y = trainingData[j].y;
			singleDemension.push_back(temp);
		}
		sort(singleDemension.begin(),singleDemension.end(),myCompare);//排序
 
		//s = 1时候计算此维度最优hyphothesis以及最小error（此时theta取值在变）
		for(j = 0; j < trainingDataSize+1; j++){ 
		    temp.s = 1;
		    if(j == 0)temp.theta = singleDemension[0].x -1.0;
		    else if(j == trainingDataSize) temp.theta = singleDemension[trainingDataSize - 1].x + 1.0;
		    else temp.theta = (singleDemension[j-1].x + singleDemension[j].x) / 2.0;
 
		    double errorRate = calculateError(singleDemension,temp,trainingDataSize);
			if(errorRate < min_errorRate){   //如果这个hyphothesis比口袋里的更优秀，则将他替代口袋里的hyphothesis，并记录此时全局最小错误，并记录此时的维度
			    bestH = temp;
			    min_errorRate = errorRate;
				bestDemension = i+1;
			}
		}
 
		//s = -1时候计算此维度最优hyohothesis以及最优error（theta取值在变）
	    for(j = 0; j < trainingDataSize+1; j++){
		    temp.s = -1;
		    if(j == 0)temp.theta = singleDemension[0].x -1.0;
		    else if(j == trainingDataSize) temp.theta = singleDemension[trainingDataSize - 1].x + 1.0;
		    else temp.theta = (singleDemension[j-1].x + singleDemension[j].x) / 2.0;
 
		    double errorRate = calculateError(singleDemension,temp,trainingDataSize);
		    if(errorRate < min_errorRate){
			    bestH = temp;
			    min_errorRate = errorRate;
				bestDemension = i+1;
			}
		}
 
	}
	return min_errorRate;
}
 
//利用测试数据testData,已经得到的最优hyphothesis，其所在的维度计算此维度的E_out
double E_out(vector<Record> testData,int bestDemension,Hyphothesis bestH,int testDataSize){
		int j;
		vector<SingleDemension> bestDemensionData;
		for(j = 0; j < testDataSize; j++){
			SingleDemension temp = {0,0};
			temp.x = testData[j].x[bestDemension-1];
			temp.y = testData[j].y;
			bestDemensionData.push_back(temp);
		}
		
		return calculateError(bestDemensionData,bestH,testDataSize);
}
 
 
int main(){
	vector<Record> trainingData;//训练样本向量
	vector<Record> testData;//测试样本向量
	fstream datafile1("hw2_train.dat.txt");
	fstream datafile2("hw2_test.dat.txt");
	if(datafile1.is_open() && datafile2.is_open()){
		getData(datafile1,trainingData);//读取训练样本
		getData(datafile2,testData);//读取测试样本
	}
	else{
		cout<<"Fail open file!"<<endl;
		exit(1);
	}
	int trainingDataSize = trainingData.size();//训练样本数
	int testDataSize = testData.size();//测试样本数
	Hyphothesis bestH = {0,0};//记录全局最优hyphothesis
	int bestDemension = 0;//记录对应的维度
	double Error_in = E_in(trainingData,bestH,bestDemension,trainingDataSize);//利用训练样本计算E_in
	double Error_out = E_out(testData,bestDemension,bestH,testDataSize);//利用测试样本计算E_out
	cout <<"--"<<bestDemension <<"--" <<Error_in <<"--" <<Error_out <<endl;
}