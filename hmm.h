#ifndef __HMM_H__
#define __HMM_H__

#define N_STATE		3
#define N_PDF		2
#define N_DIMENSION	39

typedef struct {
  float weight;
  float mean[N_DIMENSION];
  float var[N_DIMENSION];
} pdfType;

typedef struct {
  pdfType pdf[N_PDF];
} stateType;

typedef struct {
  char *name;
  float tp[N_STATE+2][N_STATE+2];
  stateType state[N_STATE];
} hmmType;

extern hmmType phones[];

#endif