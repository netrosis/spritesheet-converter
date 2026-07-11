#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>
#include <vector>
#include <unordered_map>
#include <fstream>

std::unordered_map<char, char> palette;
char currentIndex = 0;

unsigned char getIndex(char color){
    if(palette.find(color)!=palette.end()){
        return palette[color];
    }else{
        currentIndex++;
        palette[color]=currentIndex;
        return currentIndex;
    }
}

int main(int argc, char* argv[]) {

    if(argc<2){
        printf("Please provide a file to convert to a 2bpp spritesheet.\n");
        return EXIT_FAILURE;
    }
    
    int width, height, channels;
    
    unsigned char* pixels = stbi_load(argv[1],&width,&height,&channels,1);
    
    if(width*height!=16384){
        printf("Program only works with 128x128 mono-channel images. Please try again with another image.");
        return EXIT_FAILURE;
    }
    std::vector<unsigned char> sprites;
    palette[0]=0;
    for(int i=0;i<256;i++){
        int indexX = (i%16)*8;
        int indexY = (i/16)*8;
        //printf("Index %d is at position %d, %d\n",i,indexX, indexY);
        for(int ty=0;ty<8;ty++){
            for(int tx=0;tx<8;tx+=4){
                unsigned char packed = 0;
                for(int a=0;a<4;a++){
                    int index = (indexY+ty)*width+indexX+tx+a;
                    unsigned char color = getIndex(pixels[index]);
                    //printf("%d",color);
                    packed |= color<<((3-a)*2);
                }
                sprites.push_back(packed);
            }
            //printf("\n");
        }
    }
    
    std::string outputFile = argv[1];
    
    size_t lastdot = outputFile.find_last_of(".");
    size_t lastsep = outputFile.find_last_of("/\\");
    outputFile = outputFile.substr(0,lastdot);
    
    outputFile+=".bin";
    
    std::ofstream outFile(outputFile,std::ios::out|std::ios::binary);
    outFile.write(reinterpret_cast<const char*>(sprites.data()),sprites.size());
    outFile.close();
    
    return EXIT_SUCCESS;
}
