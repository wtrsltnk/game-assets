#ifndef _HL1TYPES_H
#define	_HL1TYPES_H

#include <string>
#include <map>
#include <glm/glm.hpp>

typedef std::map<std::string, std::string> KeyValueList;

typedef unsigned char byte;

template<typename T> class Array
{
    bool _deleteOnDestruct;
public:
    Array() : count(0), data(nullptr), _deleteOnDestruct(false) { }
    Array(int count) : _deleteOnDestruct(true) { this->Allocate(count); }
    Array(int count, T* data) : count(count), data(data), _deleteOnDestruct(false) { }
    virtual ~Array() { if (this->_deleteOnDestruct) this->Delete(); }

    int count;
    T* data;

    operator T*(void) const { return data; }
    const T& operator[] (int index) const { return this->data[index]; }
    T& operator[] (int index) { return this->data[index]; }

    virtual void Allocate(int count)
    {
        this->count = count;
        this->data = this->count > 0 ? new T[this->count] : nullptr;
    }

    void Map(int count, T* data)
    {
        this->count = count;
        this->data = data;
    }

    virtual void Delete()
    {
        if (this->data != nullptr) delete []this->data;
        this->data = nullptr;
        this->count = 0;
    }
};

#define CHUNK   (4096)

template<class T> class List
{
    int size;
    int count;
    T* data;

public:
    List() : size(CHUNK), data(new T[CHUNK]), count(0) { }
    virtual ~List() { this->Clear(); }

    operator T*(void) const { return data; }
    const T* operator[](int i) const { return data + i; }

    int Count() const { return count; }

    void Add(T& src)
    {
        if(count >= size)
        {
            //resize
            T* n = new T[size + CHUNK];
            for(int i = 0; i < size; i++)
                n[i] = data[i];
            delete []data;
            data = n;
            size += CHUNK;
        }

        data[count] = src;
        count++;
    }

    void Clear()
    {
        if (this->data != nullptr)
            delete this->data;
        this->data = nullptr;
        this->size = this->count = 0;
    }
};

typedef Array<byte>& (DataFileLoader)(const std::string& filename);

class Hl1Instance
{
public:
    virtual ~Hl1Instance() { }

    virtual void Update(float dt) = 0;
    virtual void Render(const glm::mat4& proj, const glm::mat4& view) = 0;

};

class Hl1Asset
{
protected:
    DataFileLoader& _loader;

public:
    Hl1Asset(DataFileLoader& loader) : _loader(loader) { }
    virtual ~Hl1Asset() { }

    virtual bool Load(const std::string& filename) = 0;
    virtual Hl1Instance* CreateInstance() = 0;
};

#endif	/* _HL1TYPES_H */

