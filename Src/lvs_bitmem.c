#include "lvs_bitmem.h"

// Get single bit from bit array
LVS_BIT_T lvs_BitmemGet1(unsigned char* mem, unsigned int index)
{
	return ((mem[index / 8] >> (index % 8)) & 0x01) == 1;
}

// Put single bit to the bit array
void lvs_BitmemPut1(unsigned char* mem, unsigned int index, LVS_BIT_T bitval)
{
	if (bitval)
		mem[index / 8] |= 0x01 << (index % 8);
	else
		mem[index / 8] &= ~(0x01 << (index % 8));
}

// Retrieve given number of bits from the bits array
unsigned long long lvs_BitmemGet(unsigned char* mem, unsigned int index, unsigned char numbits)
{
	register unsigned long long result = 0;
	unsigned char i;
	for (i = 0; i < numbits; i++)
	{
		result |= lvs_BitmemGet1(mem, index++) << i;
	}
	return result;
}

// Put given number of bits to the bits array
void lvs_BitmemPut(unsigned char* mem, unsigned int index, unsigned long long val, unsigned char numbits)
{
	for (; numbits > 0; numbits--)
	{
		lvs_BitmemPut1(mem, index++, val & 0x01);
		val >>= 1;
	}
}

/////////////////////////////////////////////////////////////////////////

// Initialize ring buffer
void lvs_BitmemInitRingbuf(LVS_BIT_RINGBUF* brb, 
	unsigned char* mem, 
	unsigned long memsize_bytes, 
	LVS_BIT_CALLBACK overflow_cb, 
	LVS_BIT_CALLBACK underflow_cb)
{
	brb->memory = mem;
	brb->size = memsize_bytes * 8;
	brb->free = 0;
	brb->bit_read_ptr = 0;
	brb->bit_write_ptr = 0;
	brb->overflow_cb = overflow_cb;
	brb->underflow_cb = underflow_cb;
}

// Write bits to the ring buffer
void lvs_BitmemWriteRingbuf(LVS_BIT_RINGBUF* brb, unsigned long long val, unsigned char numbits)
{
	while (numbits > brb->free)
		brb->overflow_cb(brb);
	unsigned long remain = brb->size - brb->bit_write_ptr;
	if (remain > numbits)
	{
		lvs_BitmemPut(brb->memory, brb->bit_write_ptr, val, numbits);
		brb->bit_write_ptr += numbits;
	}
	else
	{
		lvs_BitmemPut(brb->memory, brb->bit_write_ptr, val, (unsigned char) remain);
		brb->bit_write_ptr = 0;
		val >>= remain;
		lvs_BitmemPut(brb->memory, brb->bit_write_ptr, val, (unsigned char) ((unsigned long)numbits - remain));
		brb->bit_write_ptr = numbits - remain;
	}
	brb->free -= numbits;
}

// Retrieve given number of bits from the ring buffer
unsigned long long lvs_BitmemReadRingbuf(LVS_BIT_RINGBUF* brb, unsigned char numbits)
{
	unsigned long long result;
	while (brb->size - brb->free < numbits)
		brb->underflow_cb(brb);
	unsigned long remain = brb->size - brb->bit_read_ptr;
	if (remain > numbits)
	{
		result = lvs_BitmemGet(brb->memory, brb->bit_read_ptr, numbits);
		brb->bit_read_ptr += numbits;
	}
	else
	{
		result = lvs_BitmemGet(brb->memory, brb->bit_read_ptr, (unsigned char) remain);
		brb->bit_read_ptr = 0;
		result |= lvs_BitmemGet(brb->memory, brb->bit_read_ptr, (unsigned char) ((unsigned long) numbits - remain)) << remain;
		brb->bit_read_ptr = numbits - remain;
	}
	brb->free += numbits;
	return result;
}
