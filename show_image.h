#define HI(num) (((num) & 0x0000FF00) >> 8)
#define LO(num) ((num) & 0x000000FF)

void readPGM(char *,GrayImage *);
void savePGM(GrayImage ,char  *);
void imshow(GrayImage);
void label2rgb(GrayImage *img);
void readPPM(char *,ColorImage *);
void savePPM(ColorImage *,char *);
void cimshow(ColorImage);
void readPBM(char *,GrayImage *);
