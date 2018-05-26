#ifndef FIXED_ARRAY_HHH
#define FIXED_ARRAY_HHH

#include <stdio.h>
// todo : change to google style
template <class T, unsigned int SIZE>
class CFixedArray
{
public:
    CFixedArray();
    CFixedArray(const CFixedArray &right);
    CFixedArray(const T *pData, unsigned int iSize);
    ~CFixedArray();

    CFixedArray &   operator = (const CFixedArray &right);
    bool            operator == (const CFixedArray &right) const;
    bool            operator != (const CFixedArray &right) const;
    bool            operator <  (const CFixedArray &right) const;

public:
    T   m_data[SIZE];
};

template <class T, unsigned int SIZE>
CFixedArray<T, SIZE>::CFixedArray()
{
    memset(m_data, 0, sizeof(m_data));
}

template <class T, unsigned int SIZE>
CFixedArray<T, SIZE>::CFixedArray(const CFixedArray<T, SIZE> &right)
{
    memcpy(m_data, right.m_data, sizeof(m_data));
}

template <class T, unsigned int SIZE>
CFixedArray<T, SIZE>::CFixedArray(const T *pData, unsigned int iSize)
{
    if (pData != NULL)
    {
        if (iSize < SIZE)
        {
            memcpy(m_data, pData, iSize * sizeof(T));
            memset(m_data + iSize, 0, (SIZE - iSize) * sizeof(T));
        }
        else
        {
            memcpy(m_data, pData, sizeof(m_data));
        }
    }
    else
    {
        memset(m_data, 0, sizeof(m_data));
    }
}

template <class T, unsigned int SIZE>
CFixedArray<T, SIZE>::~CFixedArray()
{

}

template <class T, unsigned int SIZE>
CFixedArray<T, SIZE> & CFixedArray<T, SIZE>::operator = (const CFixedArray<T, SIZE> &right)
{
    memcpy(&m_data, &right.m_data, sizeof(m_data));
    return *this;
}

template <class T, unsigned int SIZE>
bool CFixedArray<T, SIZE>::operator == (const CFixedArray<T, SIZE> &right) const
{
    return (memcmp(m_data, right.m_data, sizeof(m_data)) == 0);
}

template <class T, unsigned int SIZE>
bool CFixedArray<T, SIZE>::operator != (const CFixedArray<T, SIZE> &right) const
{
    return (memcmp(m_data, right.m_data, sizeof(m_data)) != 0);
}

template <class T, unsigned int SIZE>
bool CFixedArray<T, SIZE>::operator < (const CFixedArray<T, SIZE> &right) const
{
    for (unsigned int i = 0; i < SIZE; ++i)
    {
        if (m_data[i] < right.m_data[i])
        {
            return true;
        }
    }

    return false;
}

struct filesha1_hash_functor
{
    // super hash
    size_t operator()(const uint8 * const &pFileSha1) const
    {
        // supper fast hash    

#define get16bits(d) (*((const uint16 *)(d)))

        char *data = (char *)pFileSha1;
        uint32 len = SHA1_LEN;

        uint32 hash = len;
        uint32 tmp = 0;
        int rem;

        if (len <= 0 || data == NULL) return 0;

        rem = len & 3;
        len >>= 2;

        /* Main loop */
        for (; len > 0; len--) 
        {
            hash  += get16bits(data);
            tmp    = (get16bits(data+2) << 11) ^ hash;
            hash   = (hash << 16) ^ tmp;
            data  += 2*sizeof (uint16);
            hash  += hash >> 11;
        }

        /* Handle end cases */
        switch (rem) 
        {
        case 3: 
            hash += get16bits (data);
            hash ^= hash << 16;
            hash ^= data[sizeof(uint16)] << 18;
            hash += hash >> 11;
            break;

        case 2: 
            hash += get16bits (data);
            hash ^= hash << 11;
            hash += hash >> 17;
            break;

        case 1: 
            hash += *data;
            hash ^= hash << 10;
            hash += hash >> 1;
            break;

        default:
            break;
        }

        /* Force "avalanching" of final 127 bits */
        hash ^= hash << 3;
        hash += hash >> 5;
        hash ^= hash << 2;
        hash += hash >> 15;
        hash ^= hash << 10;

        return hash;
    }
};

struct filesha1_eq_functor
{
    bool operator()(const uint8 * const &pLeftFileSha1, const uint8 * const &pRightFileSha1) const
    {
        return memcmp(pLeftFileSha1, pRightFileSha1, SHA1_LEN) == 0;
    }
};

struct  filesha1_less_functor
{
    bool operator()(const uint8 * const &pLeftFileSha1, const uint8 * const &pRightFileSha1) const
    {
        for (uint32 i=0; i<SHA1_LEN; i++)
        {
            if (pLeftFileSha1[i]>=pRightFileSha1[i])
            {
                return false;
            }
        }
        return true;
    }
};
#endif //CFixedArray 