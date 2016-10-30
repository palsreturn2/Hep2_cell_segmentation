GrayImage imfilter(GrayImage *originalImage,double filter[3][3],int filterX,int filterY);
void gradientSobel(GrayImage *,GrayImage *,GrayImage *);
GrayImage imgradient(GrayImage *originalImage);
void alphaTMFilter(GrayImage *img,float alpha,GrayImage *);
void comboSpatialFilter(GrayImage *img);
GrayImage histeq(GrayImage *);
GrayImage cannyedge(GrayImage *);
GrayImage filterGauss(GrayImage *);
void medfilt2(GrayImage *,int,GrayImage *);
void medfilt(GrayImage *img,GrayImage *);
void meanfilt(GrayImage *img,GrayImage *);



