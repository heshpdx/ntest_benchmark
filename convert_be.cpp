#include <arpa/inet.h>
#include <cassert>
#include <cstdio>
#include <cinttypes>
// Tool to convert coff file to big endian
int main(int argc, const char* argv[]) {
  assert(argc == 3);
  auto* finput = fopen(argv[1], "rb");
  assert(finput);

  auto* foutput = fopen(argv[2], "wb");
  assert(foutput);
  int iVersion;
  uint32_t fParams;

  assert(fread(&iVersion, sizeof(iVersion), 1, finput) == 1);
  assert(fread(&fParams, sizeof(fParams), 1, finput) == 1);
  assert(iVersion == 1 && fParams == 100);
  iVersion = htonl(iVersion);
  fParams = htonl(fParams);
  assert(fwrite(&iVersion, sizeof(iVersion), 1, foutput) == 1);
  assert(fwrite(&fParams, sizeof(fParams), 1, foutput) == 1);
  uint16_t coeff;
  for (int i = 0; i < 287310; ++i) {
    assert(fread(&coeff, sizeof(coeff), 1, finput) == 1);
    coeff = htons(coeff);
    assert(fwrite(&coeff, sizeof(coeff), 1, foutput) == 1);
  }
  fread(&coeff, sizeof(coeff), 1, finput);
  assert(feof(finput));

  fclose(finput);
  fclose(foutput);
}
