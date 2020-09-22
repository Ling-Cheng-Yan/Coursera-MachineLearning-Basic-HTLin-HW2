
#include<iostream>
#include<stdlib.h>
#include<vector>
#include<algorithm>
#include<math.h>
using namespace std;
 
#define DATASIZE 20 //定义[-1,1]的内点的数目
 
//训练样本结构体
struct record{
	double x;
	int y;
};
 
//hyphothesis的结构体，s为+1或-1，theta在20个点分隔的21个区间取值
struct hyphothesis{
	int s;
	double theta;
};
 
//sign函数
int sign(double x){
	if(x <= 0)return -1;
	else return 1;
}
 
//随机在[-1,1]内生成DATASIZE个点的x，并计算对应的y
void getRandData(vector<record> &trainingData){
	int i;
	for(i = 0; i < DATASIZE; i++){
		record temp;
		temp.x = 2.0 * rand() / double(RAND_MAX) - 1.0;
		temp.y = sign(temp.x);
		trainingData.push_back(temp);
	}
}
 
//添加噪声，即把20%的点的y值正负号颠倒。这里的20%可以通过随机方法得到[0,1]的数，若小于0.2则认为这个点加噪声
void getNoise(vector<record> &trainingData){
	int i;
	for(i = 0; i < DATASIZE; i++){
		double randnum = rand() / double(RAND_MAX);
		if(randnum < 0.2)
			trainingData[i].y = -1 * trainingData[i].y;
	}
}
 
//自己定义的比较方法，用于sort
bool myCompare( record &v1, record &v2){
	return v1.x < v2.x;
}
 
//对MAXSIZE = 20个样本按X进行排序，这里直接调用自带sort函数，第三个参数是自己定义的比较方法（C++并不认识record,不知道怎么比较，我们要定义mycompare告诉他）
void sortTrainingData(vector<record> &trainingData){
	sort(trainingData.begin(),trainingData.end(),myCompare);
}
 
//给定输入集合和指定的hyphothesis计算对应的错误率
double calculateError(vector<record> &trainingData,hyphothesis &h){
	int i;
	int error = 0;
	for(i = 0; i < DATASIZE; i++){
		int temp = h.s * sign(trainingData[i].x - h.theta);
		if(temp != trainingData[i].y)error++;
	}
	return error/double(DATASIZE);
}
 
//由于S = 1 or -1,theta取值有21中，共有42中hyphothesis，我们计算得42种中最小的为E_in，并记录此时最小错误和对应的hyphothesis
double E_in(vector<record> &trainingData,hyphothesis &bestH){
	hyphothesis temp;
	double min_errorRate = 1.0;
	int i;
	//s = 1时
	for(i = 0; i < DATASIZE+1; i++){
		temp.s = 1;
		if(i == 0)temp.theta = trainingData[0].x -1.0;//theta取值，theta小于最小
		else if(i == DATASIZE) temp.theta = trainingData[DATASIZE - 1].x + 1.0;//theta取值在两点之间
		else temp.theta = (trainingData[i-1].x + trainingData[i].x) / 2.0;//theta取值大于最大
 
		double errorRate = calculateError(trainingData,temp);//如果此hyphothesis的错误更小，则替代
		if(errorRate < min_errorRate){
			bestH = temp;
			min_errorRate = errorRate;
		}
	}
	//s = -1时
	for(i = 0; i < DATASIZE+1; i++){
		temp.s = -1;
		if(i == 0)temp.theta = trainingData[0].x -1.0;
		else if(i == DATASIZE) temp.theta = trainingData[DATASIZE - 1].x + 1.0;
		else temp.theta = (trainingData[i-1].x + trainingData[i].x) / 2.0;
 
		double errorRate = calculateError(trainingData,temp);
		if(errorRate < min_errorRate){
			bestH = temp;
			min_errorRate = errorRate;
		}
	}
	return min_errorRate;
}
 
//利用16题得到的公式计算E_out，注意，浮点数求绝对值用fabs,而不是abs
double E_out(hyphothesis &bestH){
	return 0.5 + 0.3 * double(bestH.s) * (double)(fabs(bestH.theta) - 1.0);
}
 
int main(){
	int i;
	double totalE_inRate = 0.0;
	double totalE_outRate = 0.0;
 
	int seed[5000];//由于要进行5000次求平均，所以要有5000个种子用于求随机数
	int j;
	for( j = 0; j < 5000; j++){
		seed[j] = rand();	//这5000个种子通过一次取随机数产生
	}
 
	for(i = 0; i < 5000; i ++){
		srand(seed[i]);//每次取一个种子，那么每次产生的随机数序列就不一样了
	    vector<record> trainingData;
	    getRandData(trainingData);//随机生成训练样本
	    getNoise(trainingData);//加噪声
	    sortTrainingData(trainingData);//样本排序
		hyphothesis bestH = {0,0};
        double min_errorRate = E_in(trainingData,bestH);//计算最优hyphothesis并记录最小错误率
	    cout<<"mininum E_in:"<<min_errorRate<<endl;
		totalE_inRate += min_errorRate;
		totalE_outRate += E_out(bestH);
		cout<<"E_out:"<<E_out(bestH)<<endl;//利用这次得到的最优hyphothesis求E_out
	}
	cout<<"average E_in:"<<totalE_inRate / 5000<<endl;//得平均E_in
	cout<<"E_out:"<<totalE_outRate / 5000<<endl;//得平均E_out
}