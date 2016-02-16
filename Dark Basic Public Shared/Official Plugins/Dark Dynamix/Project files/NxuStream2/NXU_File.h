#ifndef NXU_FILE_H

#define NXU_FILE_H

namespace NXU
{

class NXU_FILE;

NXU_FILE * nxu_fopen(const char *fname,const char *spec,void *mem=0,int len=0);
void       nxu_fclose(NXU_FILE *file);
size_t     nxu_fread(void *buffer,size_t size,int count,NXU_FILE *fph);
size_t     nxu_fwrite(const void *buffer,size_t size,int count,NXU_FILE *fph);
size_t     nxu_fprintf(NXU_FILE *fph,const char *fmt,...);
int        nxu_fflush(NXU_FILE *fph);
size_t     nxu_fseek(NXU_FILE *fph,long loc,int mode);
size_t     nxu_ftell(NXU_FILE *fph);
size_t     nxu_fputc(char c,NXU_FILE *fph);
size_t     nxu_fputs(const char *str,NXU_FILE *fph);
int        nxu_feof(NXU_FILE *fph);
int        nxu_ferror(NXU_FILE *fph);
void *     nxu_getMemBuffer(NXU_FILE *fph,size_t &outputLength);

} // end of NXU namespace

#endif
