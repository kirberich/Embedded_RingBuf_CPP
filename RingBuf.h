#ifndef EM_RINGBUF_CPP_H
#define EM_RINGBUF_CPP_H

#include "RingBufHelpers.h"

template <typename Type, uint16_t MaxElements>
class RingBufCPP
{
public:

RingBufCPP()
{
     RB_ATOMIC_START
     {
         _numElements = 0;

         _head = 0;
     }
     RB_ATOMIC_END
}

/**
*  Add element obj to the buffer
* Return: true on success
*/
bool add(Type &obj)
{
    RB_ATOMIC_START
    {
        _buf[_head] = obj;
        _head = (_head + 1)%MaxElements;

        if (!isFull()) {
            _numElements++;
        }
    }
    RB_ATOMIC_END

    return true;
}


/**
* Remove last element from buffer, and copy it to dest
* Return: true on success
*/
bool pull(Type *dest)
{
    bool ret = false;
    uint16_t tail;

    RB_ATOMIC_START
    {
        if (!isEmpty()) {
            tail = getTail();
            *dest = _buf[tail];
            _numElements--;

            ret = true;
        }
    }
    RB_ATOMIC_END

    return ret;
}


/**
* Peek at num'th element in the buffer
* Return: a pointer to the num'th element
*/
Type* peek(uint16_t num)
{
    Type *ret = NULL;

    RB_ATOMIC_START
    {
        if (num < _numElements) //make sure not out of bounds
            ret = &_buf[(getTail() + num)%MaxElements];
    }
    RB_ATOMIC_END

    return ret;
}


/**
* Return: true if buffer is full
*/
bool isFull()
{
    bool ret;

    RB_ATOMIC_START
    {
        ret = _numElements >= MaxElements;
    }
    RB_ATOMIC_END

    return ret;
}


/**
* Return: number of elements in buffer
*/
uint16_t numElements()
{
    uint16_t ret;

    RB_ATOMIC_START
    {
        ret = _numElements;
    }
    RB_ATOMIC_END

    return ret;
}


/**
* Return: true if buffer is empty
*/
bool isEmpty()
{
    bool ret;

    RB_ATOMIC_START
    {
        ret = !_numElements;
    }
    RB_ATOMIC_END

    return ret;
}

protected:
/**
* Calculates the index in the array of the oldest element
* Return: index in array of element
*/
uint16_t getTail()
{
    return (_head + (MaxElements - _numElements))%MaxElements;
}


// underlying array
Type _buf[MaxElements];

uint16_t _head;
uint16_t _numElements;
private:

};

#endif
