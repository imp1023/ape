

#ifndef __RAND_H__
#define __RAND_H__


//随机数生成类

class RandGen
{
public:
	typedef unsigned long SeedType;
private:
	SeedType m_Seed[3];
	static const SeedType Max32BitLong = 0xFFFFFFFFLU;
public:
	static const SeedType RandomMax = Max32BitLong;

	RandGen(const SeedType p_Seed = 0)
	{
		Reset(p_Seed);
	}
	//ReSeed the random number generator
	//种子处理
	void Reset(const SeedType p_Seed = 0)
	{		
		m_Seed[0] = (p_Seed ^ 0xFEA09B9DLU) & 0xFFFFFFFELU;
		m_Seed[0] ^= (((m_Seed[0] << 7) & Max32BitLong) ^ m_Seed[0]) >> 31;

		m_Seed[1] = (p_Seed ^ 0x9C129511LU) & 0xFFFFFFF8LU;
		m_Seed[1] ^= (((m_Seed[1] << 2) & Max32BitLong) ^ m_Seed[1]) >> 29;

		m_Seed[2] = (p_Seed ^ 0x2512CFB8LU) & 0xFFFFFFF0LU;
		m_Seed[2] ^= (((m_Seed[2] << 9) & Max32BitLong) ^ m_Seed[2]) >> 28;

		RandUInt();
	}

	//Returns an unsigned integer from 0..RandomMax
	//0~RandMax UINT 随机数
	unsigned long RandUInt(void)
	{
		m_Seed[0] = (((m_Seed[0] & 0xFFFFFFFELU) << 24) & Max32BitLong)
			^ ((m_Seed[0] ^ ((m_Seed[0] << 7) & Max32BitLong)) >> 7);

		m_Seed[1] = (((m_Seed[1] & 0xFFFFFFF8LU) << 7) & Max32BitLong)
			^ ((m_Seed[1] ^ ((m_Seed[1] << 2) & Max32BitLong)) >> 22);

		m_Seed[2] = (((m_Seed[2] & 0xFFFFFFF0LU) << 11) & Max32BitLong)
			^ ((m_Seed[2] ^ ((m_Seed[2] << 9) & Max32BitLong)) >> 17);

		return (m_Seed[0] ^ m_Seed[1] ^ m_Seed[2]);
	}
	//Returns a double in [0.0, 1.0)
	//返回0.0~1.0之间的双精度浮点
	double RandDouble(void)
	{
		return static_cast<double>(RandUInt())
			/ (static_cast<double>(RandomMax) );
	}
	
	static int		GetRand(int nStart, int nEnd);
		
};

extern	RandGen	g_RandGen;


#endif
