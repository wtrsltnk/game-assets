#ifndef _TEXTURE_H
#define	_TEXTURE_H

#include <string>

class Texture
{
    std::string _name;
    int _width;
    int _height;
    int _bpp;
    int _format;
    bool _repeat;
    unsigned char* _data;
    unsigned int _glIndex;

public:
    Texture();
    virtual ~Texture();

    const std::string& Name() const;
    int Width() const;
    int Height() const;
    int Bpp() const;
    int DataSize() const;
    unsigned char* Data();
    unsigned int GlIndex() const;

    unsigned int UploadToGl();
    void DeleteFromGl();
    
    Texture* Copy() const;
    void CopyFrom(const Texture& from);
    void DefaultTexture();

    void SetData(int w, int h, int bpp, unsigned char* data, bool repeat = true);
    void SetName(const std::string& _name);
    void SetDimentions(int _width, int _height, int _bpp = 3, unsigned int _format = -1);

    void CorrectGamma(float gamma);

};

#endif	/* _TEXTURE_H */

