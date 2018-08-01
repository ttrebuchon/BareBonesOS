#ifndef INCLUDED_DRIVERS_CHS_H
#define INCLUDED_DRIVERS_CHS_H

#include <Common.h>

#define CHS_SEC_PER_HEAD 64
#define CHS_HEAD_PER_CYL 16
#define CHS_SEC_PER_CYL (CHS_SEC_PER_HEAD*CHS_HEAD_PER_CYL)

START_NS(Drivers)
C_CODE

typedef struct _CHS_type
{
	uint32_t cylinder : 10;
	uint8_t head : 4;
	uint8_t sector : 6;
	
	#ifdef __cplusplus
	
	constexpr _CHS_type(uint32_t cyl, uint8_t hd, uint8_t sec) noexcept : cylinder(cyl), head(hd), sector(sec)
	{
		
	}
	
	constexpr _CHS_type(uint32_t lba) noexcept : cylinder(lba/CHS_SEC_PER_CYL), head((lba % CHS_SEC_PER_CYL)/CHS_SEC_PER_HEAD), sector(lba % CHS_SEC_PER_HEAD)
	{
		
	}
	
	constexpr _CHS_type() noexcept : _CHS_type(0, 0, 0)
	{}
	
	_CHS_type operator+(const _CHS_type other) const noexcept
	{
		return _CHS_type(((uint32_t)*this) + (uint32_t)other);
	}
	
	_CHS_type operator-(const _CHS_type other) const noexcept
	{
		return _CHS_type(((uint32_t)*this) - (uint32_t)other);
	}
	
	_CHS_type operator+(const uint32_t off) const noexcept
	{
		return _CHS_type(((uint32_t)*this) + off);
	}
	
	_CHS_type operator-(const uint32_t off) const noexcept
	{
		return _CHS_type(((uint32_t)*this) - off);
	}
	
	
	
	_CHS_type& operator+=(const _CHS_type other) noexcept
	{
		return (*this = (*this + other));
	}
	
	_CHS_type& operator-=(const _CHS_type other) noexcept
	{
		return (*this = (*this - other));
	}
	
	
	_CHS_type& operator+=(const uint32_t n) noexcept
	{
		return (*this = (*this + n));
	}
	
	_CHS_type& operator-=(const uint32_t n) noexcept
	{
		return (*this = (*this - n));
	}
	
	
	explicit operator uint32_t() const noexcept
	{
		return (uint32_t(sector) + uint32_t(head)*CHS_SEC_PER_HEAD + uint32_t(cylinder)*CHS_SEC_PER_CYL);
	}
	
	#endif
} CHS_t;

C_END
END_NS

#endif