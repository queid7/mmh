#pragma once
 
//! BitArray의 Fast Version
/*! 다른 종류의 BitArray는 BitProxy class를 사용하여 a[10]=0 처럼 lvalue로 사용되는 것이 가능하지만,
이 BitArray는 lvalue로 사용되는 경우 Assign 또는 SetAt, ClearAt을 사용하여야 한다. 
또한 32보다 큰 크기의 array로 사용할 수 없다.  
그 대신 직접 bitwise operation을 수행하는 것에 비해 전혀 performance overhead가 없다.
*/
class BitArray  
{
public:
	BitArray(const BitArray& other) { m_Bits=other.m_Bits; };
	BitArray() { m_Bits=0; };
	~BitArray()	{};
	//! b=a[10] 처럼 사용할 수 있다.
	inline bool operator[](int nIndex) const				{ return (m_Bits>>nIndex &1);}
	//! SetAt(1)은 a[1]=true에 해당 
	inline void SetAt(int nIndex)							{ m_Bits|=1<<nIndex; }
	inline void SetAt(int nIndex,int value)			{ if(value)m_Bits|=1<<nIndex;else m_Bits&=~(1<<nIndex); }
	//! ClearAt(1)은 a[1]=false에 해당 
	inline void ClearAt(int nIndex)							{ m_Bits&=~(1<<nIndex); }

	inline void Assign(int nIndex, bool bit)				{ if(bit) SetAt(nIndex); else ClearAt(nIndex);}

	inline void ClearAll()									{ m_Bits=0;}
	int GetCount() const				{	int count=0; for(int i=0; i<32; i++) if(m_Bits>>i&1)	count++; return count; }

	// bitwise or
	friend BitArray operator|(const BitArray& a, const BitArray& b)	{ BitArray c; c.m_Bits=a.m_Bits|b.m_Bits; return c;}
	// bitwise and
	friend BitArray operator&(const BitArray& a, const BitArray& b)	{ BitArray c; c.m_Bits=a.m_Bits&b.m_Bits; return c;}

	BitArray& operator|=( const BitArray & other)					{ m_Bits|=other.m_Bits; return *this;}
	BitArray& operator&=( const BitArray & other)					{ m_Bits&=other.m_Bits; return *this;}
	void operator=(const BitArray& other)							{ m_Bits=other.m_Bits;}
	unsigned int m_Bits;
};

class vectorn;
class intvectorn;
#include "../utility/TArray.h"
/**
 * \ingroup group_math
 *
 * bit의 vector
 *
 */
class bitvectorn
{
public:
    bitvectorn(void);
	bitvectorn(const bitvectorn& other);
	bitvectorn(const vectorn& other);
	~bitvectorn(void);

	void setSize(int size);
	void resize(int size);

	//! b=a[10] 처럼 사용할 수 있다.
	inline bool operator[](int nIndex) const				{ ASSERT(nIndex<m_nSize); return m_aBitArrays[calcBitArrayPos(nIndex)][calcBitArrayIndex(nIndex)];};
	inline bool operator()(int nIndex) const				{ return (*this)[nIndex];}

	bool getValue(int nIndex) const							{ return this->operator [](nIndex);};
	bool value(int nIndex) const							{ return this->operator [](nIndex);};

	//! SetAt(1)은 a[1]=true에 해당 
	void setAt(int nIndex)									{ ASSERT(nIndex<m_nSize); m_aBitArrays[calcBitArrayPos(nIndex)].SetAt(calcBitArrayIndex(nIndex));};
	void setAt(const intvectorn& aIndex);
	
	//! ClearAt(1)은 a[1]=false에 해당 
	void clearAt(int nIndex)								{ ASSERT(nIndex<m_nSize); m_aBitArrays[calcBitArrayPos(nIndex)].ClearAt(calcBitArrayIndex(nIndex));};
	void clearAt(const intvectorn& aIndex);
    
	inline void setValue(int nIndex, bool bit)				{ if(bit) setAt(nIndex); else clearAt(nIndex);};
	inline void setAllValue(bool bit)						{ if(bit) setAll(); else clearAll();}
	void setValue(int start, int end, bool bit);
	inline void setAll()									{ setValue(0, size(), true); }
	inline void clearAll()									{ int index=calcBitArrayPos(m_nSize)+1; for(int i=0; i<index; i++) m_aBitArrays[i].ClearAll();};
	int count()	const										{ if(m_nSize==0) return 0; int index=calcBitArrayPos(m_nSize)+1; int count=0; for(int i=0; i<index; i++) count+=m_aBitArrays[i].GetCount(); return count;};
	int size() const										{ return m_nSize;};
	bitvectorn& operator=( bitvectorn const& );
	bitvectorn& operator=( vectorn const& );

	bitvectorn& op(int (*s2_func)(int,int), const intvectorn& source, int value, int start=0, int end=INT_MAX);
	bitvectorn& op(m_real (*s2_func)(m_real,m_real), const vectorn& source, m_real value, int start=0, int end=INT_MAX);

	// primary functions
	// binary OP
	void or(const bitvectorn& a, const bitvectorn& b);
	void and(const bitvectorn& a, const bitvectorn& b);
	// unary OP
	bitvectorn& operator|=(bitvectorn const& a) { (*this).or((*this),a); return *this;};
	bitvectorn& operator&=(bitvectorn const& a) { (*this).and((*this),a); return *this;};
	bool operator==(bitvectorn const& other) const;
	bool operator!=(bitvectorn const& other) const	{ return !operator==(other);};
	// utility functions
	enum zeroCrossingMode { ZC_MIN, ZC_MAX, ZC_ALL};	
	//! zero-crossing을 찾는다.	ZC_MIN: negative to positive crossing, ZC_MAX: positive to negative crossing
	void findZeroCrossing(const vectorn& signal, zeroCrossingMode mode=ZC_ALL);
	//! local optimum을 찾는다. ZC_MIN: local minimum, ZC_MAX: local maximum
	void findLocalOptimum(const vectorn& signal, zeroCrossingMode mode=ZC_MIN);
	//! 일종의 windowed global optimum이다. 단 candidate는 localOptimum이다. 소스코드 참고.
	void refineLocalOptimum(const vectorn& signal, const bitvectorn& localOptimum, int windowSize, zeroCrossingMode mode=ZC_MIN);
	//! true 밸류들을 clustering한다. 즉 가까이 있는 것들끼리 하나로 묶어 center를 구한다. (kmean사용)
	void cluster(int windowSize);
	void makeJumpIndex(intvectorn& jumpIndex) const;
	//! output 크기는 (end-start)*2-1이다. true인 군집의 center위치에 true로 assign한다.
	/// input    1 0 1 1 1 0 1 1 0 0
	/// centers  1000001000000100000 
	bitvectorn& centers(int start, int end, const intvectorn& jumpIndex, const bitvectorn& bits, bool bLocal=true);

	/// if there is no bValue return end;
	int find(int start, int end, bool bValue=true) const;
	int find(int start, bool bValue=true) const;
	int findPrev(int start, bool bValue=true) const;
	
	// find bValue which is nearest to i. if fail, return -1;
    int findNearest(float i, bool bValue=true) const;
	float distanceToNearest(float i, bool bValue=true) const;

	// deprecated.
	void output(TString& id, int start=0, int end=INT_MAX) const;
	TString output(int start=0, int end=INT_MAX) const;

	void save(const char* filename);	// text file에 쓰기.ex) a.txt = 1 3 6 12
	void load(int size, const char* filename);	// text file에서 읽기.ex) a.txt = 1 3 6 12
private:
	inline int calcBitArrayPos(int nIndex) const			{ return nIndex/32;};
	inline int calcBitArrayIndex(int nIndex) const			{ return nIndex%32;};
	int m_nSize;
	std::vector<BitArray> m_aBitArrays;
	friend class BinaryFile;
};


