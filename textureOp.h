#define LEVEL 256
#define mod(x) (x>0)?x:(-x)


void computeProb(GrayImage*,float **,int,int);
float constrast(float **);
float uniformity(float **);
float homogeneity(float **);
float entropy(float **);
float correlation(float **);
float diffMoment(float **promatrix);
void calProSum(float **promatrix,float *sum);
float sumAvg(float **promatrix);
float sumEntropy(float **promatrix);
float sumVariance(float **promatrix);
void calProDiff(float **promatrix,float *diff);
float diffEntropy(float **promatrix);
float diffVariance(float **promatrix);
