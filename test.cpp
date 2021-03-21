#include <stdlib.h>
#include <stdio.h>
#include "bmp.h"

int main(void)
{
    Bitmap a(100,100);
    for(int i = 0; i<100; i++)
    {
        for(int j = 0; j<100; j++)
        {
        
            a.setColor(j, i, (unsigned char)255, (unsigned char)255, (unsigned char)255);
        }
    }
    a.save("white.bmp");
    a.~Bitmap();
    
    Bitmap b(100,100);
    for(int i = 0; i<100; i++)
    {
        for(int j = 0; j<100; j++)
        {
        
            b.setColor(j, i, (unsigned char)0, (unsigned char)0, (unsigned char)0);
        }
    }
    b.save("black.bmp");
    b.~Bitmap();
    
    return 0;
}
