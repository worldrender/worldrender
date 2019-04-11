#include <stdlib.h>
#define QTDTEXTURAS 3

using namespace std;

int width, height, nrChannels;
unsigned char *data;
GLuint textures[QTDTEXTURAS];

const char* filenames[QTDTEXTURAS] = {"texture.png",
                                    "detail.png",
                                    "normalmap.png"
                                    };
