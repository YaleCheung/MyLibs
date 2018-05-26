#ifndef OBJECT_POOL_HHH
#define OBJECT_POOL_HHH

#include <assert.h>

template <class T>
class CObjectPool
{
public:
    CObjectPool();
    ~CObjectPool();

    // init
    sint32  Init(void *p, uint32 iSize, uint32 iObjectNum);
    sint32  LoadFromMem(void *p, uint32 iSize);

    // op
    T *     Alloc();
    void    Free(T *p);

    //
    sint32  GetIdxByPtr(const T *p) const;
    T *     GetPtrByIdx(sint32 iIdx) const;

    bool    InUse(sint32 iIdx) const;
    bool    InUse(const T *p) const;

    // foreach
    T *     GetFirstInUseObject() const;
    T *     GetNextInUseObject(const T *p) const;

    //
    uint32  GetTotalNum() const;
    uint32  GetUsingNum() const;
    uint32  GetFreeNum() const;

    static uint32  CalcPoolNeedSize(uint32 iObjectNum);

private:

    class CFreeLink
    {
    public:
        sint32      m_iNextFreeIdx:31;
        uint32      m_iCurInUse:1;
    };

    enum EFreeLinkFlag
    {
        FREE_LINK_FLAG_NOT_INUSE    = 0,
        FREE_LINK_FLAG_INUSE        = 1,
    };

    class CPoolHead
    {
    public:
        uint32      m_iEachObjectSize;

        uint32      m_iTotalNum;
        uint32      m_iFreeNum;

        sint32      m_iHeadFreeIdx;
        sint32      m_iTailFreeIdx;

        CFreeLink   m_freeLinks[0];
    };

private:

    T *             GetObjectsArr() const;

private:

    CPoolHead *     m_pPoolHead;
};

template <class T>
CObjectPool<T>::CObjectPool()
: m_pPoolHead(NULL)
{

}

template <class T>
CObjectPool<T>::~CObjectPool()
{

}

template <class T>
sint32 CObjectPool<T>::Init(void *p, uint32 iSize, uint32 iObjectNum)
{
    uint32 iNeedSize = CalcPoolNeedSize(iObjectNum);
    if (iSize < iNeedSize)
    {
        // space not enough
        return -1;
    }

    // init pool head
    m_pPoolHead = static_cast<CPoolHead *>(p);
    m_pPoolHead->m_iTotalNum = iObjectNum;
    m_pPoolHead->m_iEachObjectSize = sizeof(T);

    // init free links
    m_pPoolHead->m_iFreeNum = m_pPoolHead->m_iTotalNum;
    m_pPoolHead->m_iHeadFreeIdx = 0;
    m_pPoolHead->m_iTailFreeIdx = m_pPoolHead->m_iTotalNum - 1;

    CFreeLink *pFreeLinksArr = m_pPoolHead->m_freeLinks;
    uint32 i = 0;
    for (; i < (m_pPoolHead->m_iTotalNum - 1); ++i)
    {
        pFreeLinksArr[i].m_iCurInUse = FREE_LINK_FLAG_NOT_INUSE;
        pFreeLinksArr[i].m_iNextFreeIdx = i + 1;
    }
    pFreeLinksArr[i].m_iCurInUse = FREE_LINK_FLAG_NOT_INUSE;
    pFreeLinksArr[i].m_iNextFreeIdx = -1;

    // init Objects ptr

    return 0;
}

template <class T>
sint32 CObjectPool<T>::LoadFromMem(void *p, uint32 iSize)
{
    CPoolHead *pPoolHead = static_cast<CPoolHead *>(p);
    if (CalcPoolNeedSize(pPoolHead->m_iTotalNum) > iSize
        || pPoolHead->m_iEachObjectSize != sizeof(T))
    {
        return -1;
    }

    m_pPoolHead = pPoolHead;

    // rebuild free links
    uint32 iOldFreeNum = m_pPoolHead->m_iFreeNum;
    m_pPoolHead->m_iFreeNum = 0;
    m_pPoolHead->m_iHeadFreeIdx = -1;
    m_pPoolHead->m_iTailFreeIdx = -1;

    CFreeLink *pFreeLinksArr = m_pPoolHead->m_freeLinks;
    sint32 i = 0;
    sint32 j = -1;
    for (; i < (sint32)(m_pPoolHead->m_iTotalNum); ++i)
    {
        if (FREE_LINK_FLAG_NOT_INUSE == pFreeLinksArr[i].m_iCurInUse)
        {
            if (-1 == m_pPoolHead->m_iHeadFreeIdx)
            {
                m_pPoolHead->m_iHeadFreeIdx = i;
            }
            else
            {
                pFreeLinksArr[j].m_iNextFreeIdx = i;
            }

            j = i;
            m_pPoolHead->m_iFreeNum++;
        }
    }

    if (j != -1)
    {
        pFreeLinksArr[j].m_iNextFreeIdx = -1;
        m_pPoolHead->m_iTailFreeIdx = j;
    }

    if (iOldFreeNum != m_pPoolHead->m_iFreeNum)
    {
        // TODO: log
    }

    return 0;
}

template <class T>
T * CObjectPool<T>::Alloc()
{
    assert(m_pPoolHead != NULL);

    if (0 == m_pPoolHead->m_iFreeNum)
    {
        return NULL;
    }

    // unlink from the front of free list
    CFreeLink *pFreeLinkArr = m_pPoolHead->m_freeLinks;
    sint32 iIdx = m_pPoolHead->m_iHeadFreeIdx;

    m_pPoolHead->m_iHeadFreeIdx = pFreeLinkArr[iIdx].m_iNextFreeIdx;
    m_pPoolHead->m_iFreeNum--;

    if (0 == m_pPoolHead->m_iFreeNum)
    {
        m_pPoolHead->m_iHeadFreeIdx = m_pPoolHead->m_iTailFreeIdx = -1;
    }

    // inuse flag
    pFreeLinkArr[iIdx].m_iCurInUse = FREE_LINK_FLAG_INUSE;
    pFreeLinkArr[iIdx].m_iNextFreeIdx = 0;

    T *pObjectArr = GetObjectsArr();

    return pObjectArr + iIdx;
}

template <class T>
void CObjectPool<T>::Free(T *p)
{
    assert(m_pPoolHead != NULL);

    sint32 iIdx = GetIdxByPtr(p);
    assert(iIdx >= 0);

    CFreeLink *pFreeLinkArr = m_pPoolHead->m_freeLinks;
    if (FREE_LINK_FLAG_INUSE == pFreeLinkArr[iIdx].m_iCurInUse)
    {
        pFreeLinkArr[iIdx].m_iCurInUse = FREE_LINK_FLAG_NOT_INUSE;
        pFreeLinkArr[iIdx].m_iNextFreeIdx = -1;

        // link to the end of free list
        if (0 == m_pPoolHead->m_iFreeNum)
        {
            m_pPoolHead->m_iHeadFreeIdx = iIdx;
        }
        else
        {
            assert(m_pPoolHead->m_iHeadFreeIdx != -1
                && m_pPoolHead->m_iTailFreeIdx != -1);

            pFreeLinkArr[m_pPoolHead->m_iTailFreeIdx].m_iNextFreeIdx = iIdx;
        }

        m_pPoolHead->m_iTailFreeIdx = iIdx;

        m_pPoolHead->m_iFreeNum++;
    }
}

template <class T>
sint32 CObjectPool<T>::GetIdxByPtr(const T *p) const
{
    assert(m_pPoolHead != NULL);

    const T *pBegin = GetObjectsArr();
    const T *pEnd = pBegin + (m_pPoolHead->m_iTotalNum - 1);

    if (p >= pBegin && p <= pEnd)
    {
        return static_cast<sint32>(p - pBegin);
    }
    else
    {
        return -1;
    }
}

template <class T>
T * CObjectPool<T>::GetPtrByIdx(sint32 iIdx) const
{
    assert(m_pPoolHead != NULL);

    if (iIdx >=0 && iIdx < (sint32)(m_pPoolHead->m_iTotalNum))
    {
        return GetObjectsArr() + iIdx;
    }

    return NULL;
}

template <class T>
bool CObjectPool<T>::InUse(sint32 iIdx) const
{
    assert(m_pPoolHead != NULL);

    const CFreeLink *pFreeLink = m_pPoolHead->m_freeLinks + iIdx;
    if (NULL == pFreeLink)
    {
        return false;
    }

    return (FREE_LINK_FLAG_INUSE == pFreeLink->m_iCurInUse);
}

template <class T>
bool CObjectPool<T>::InUse(const T *p) const
{
    assert(m_pPoolHead != NULL);

    sint32 iIdx = GetIdxByPtr(p);
    return InUse(iIdx);
}

template <class T>
T * CObjectPool<T>::GetFirstInUseObject() const
{
    assert(m_pPoolHead != NULL);
    assert(m_pPoolHead->m_iTotalNum >= m_pPoolHead->m_iFreeNum);

    if (m_pPoolHead->m_iFreeNum == m_pPoolHead->m_iTotalNum)
    {
        return NULL;
    }

    const CFreeLink *pFreeLinksArr = m_pPoolHead->m_freeLinks;
    T *pObjectsArr = GetObjectsArr();
    for (uint32 i = 0; i < m_pPoolHead->m_iTotalNum; ++i)
    {
        if (FREE_LINK_FLAG_INUSE == pFreeLinksArr[i].m_iCurInUse)
        {
            return (pObjectsArr + i);
        }
    }

    // TODO: logerr
    return NULL;
}

template <class T>
T * CObjectPool<T>::GetNextInUseObject(const T *p) const
{
    assert(m_pPoolHead != NULL);
    assert(m_pPoolHead->m_iTotalNum >= m_pPoolHead->m_iFreeNum);

    sint32 iIdx = GetIdxByPtr(p);

    CFreeLink *pFreeLinksArr = m_pPoolHead->m_freeLinks;
    T *pObjectsArr = GetObjectsArr();
    for (uint32 i = iIdx + 1; i < m_pPoolHead->m_iTotalNum; ++i)
    {
        if (FREE_LINK_FLAG_INUSE == pFreeLinksArr[i].m_iCurInUse)
        {
            return (pObjectsArr + i);
        }
    }

    return NULL;
}

template <class T>
uint32 CObjectPool<T>::GetTotalNum() const
{
    assert(m_pPoolHead != NULL);

    return m_pPoolHead->m_iTotalNum;
}

template <class T>
uint32 CObjectPool<T>::GetUsingNum() const
{
    assert(m_pPoolHead != NULL);
    assert(m_pPoolHead->m_iTotalNum >= m_pPoolHead->m_iFreeNum);

    return (m_pPoolHead->m_iTotalNum - m_pPoolHead->m_iFreeNum);
}

template <class T>
uint32 CObjectPool<T>::GetFreeNum() const
{
    assert(m_pPoolHead != NULL);
    assert(m_pPoolHead->m_iTotalNum >= m_pPoolHead->m_iFreeNum);

    return m_pPoolHead->m_iFreeNum;
}

template <class T>
uint32 CObjectPool<T>::CalcPoolNeedSize(uint32 iObjectNum)
{
    return sizeof(CPoolHead) + ((sizeof(CFreeLink) + sizeof(T)) * iObjectNum);
}

template <class T>
T * CObjectPool<T>::GetObjectsArr() const
{
    assert(m_pPoolHead != NULL);

    return reinterpret_cast<T *>(m_pPoolHead->m_freeLinks + m_pPoolHead->m_iTotalNum);
}

#endif // OBJECT_POOL_HHH



