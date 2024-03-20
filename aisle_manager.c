/*
 * CSE 351 Lab 1b (Manipulating Bits in C)
 *
 * Name(s): Luke Sorvik
 * NetID(s): lukesorv
 *
 * ----------------------------------------------------------------------------
 * Overview
 * ----------------------------------------------------------------------------
 *  This is a program to keep track of the items in a small aisle of a store.
 *
 *  A store's aisle is represented by a 64-bit(8byte) long integer, which is broken
 *  into 4 16-bit(2byte) sections representing one type of item each. Note that since
 *  a section is 16-bits(2byte), it fits nicely into C's short datatype.
 *
 *  Aisle Layout:
 *
 *    Within an aisle, sections are indexed starting with the least-significant(right most section)
 *    section being at index 0 and continuing up until one less than the number
 *    of sections. (size - 1)
 *
 *    Example aisle:
 *
 *                MSB                                                       LSB
 *                  +-------------+-------------+-------------+-------------+
 *                  |  Section 3  |  Section 2  |  Section 1  |  Section 0  |
 *                  +-------------+-------------+-------------+-------------+
 *                  |             |             |             |             |
 *      bit offset: 64            48            32            16            0
 *
 *  Section Layout:
 *
 *    A section in an aisle is broken into 2 parts. The 6 most significant bits(6 far left bits)
 *    represent a unique identifier for the type of item stored in that
 *    section(item id). The rest of the bits in a section (10 least significant)
 *    indicate individual spaces for items in that section. For each of the 10
 *    bits/spaces, a 1 indicates that an item of the section's type is stored
 *    there and a 0 indicates that the space is empty.
 *
 *    each section is 16 bits (2bytes)
 *    Example aisle section: 0x651A
 *
 *                MSB                               LSB
 *                  +-----------+-------------------+
 *                  |0 1 1 0 0 1|0 1 0 0 0 1 1 0 1 0|
 *                  +-----------+-------------------+
 *                  | item id   | section spaces    |
 *      bit offset: 16          10                  0
 *
 *      In this example, the item id is 0b011001(six far left bits), and there are currently 4
 *      items stored in the section(4 1s in the section spaces) (at bit offsets 8, 4, 3, and 1) and 6
 *      vacant spaces.
 *
 *  Written by Porter Jones (pbjones@cs.washington.edu)
 */

#include "aisle_manager.h"
#include "store_util.h"
#include <stdio.h>

// the number of total bits in a section
#define SECTION_SIZE 16

// The number of bits in a section used for the item spaces
#define NUM_SPACES 10

// The number of bits in a section used for the item id
#define ID_SIZE 6

// The number of sections in an aisle
#define NUM_SECTIONS 4

// TODO: Fill in these with the correct hex values

// Mask for extracting a section from the least significant bits of an aisle.
//(aisle is 64 bits long, lest significan bits would be the last 16 bits(last section))
// (aisle & SECTION_MASK) should preserve a section's worth of bits at the
// lower end of the aisle and set all other bits to 0. This is essentially
// extracting section 0 from the example aisle shown above. (last 16 bits of aisle)
#define SECTION_MASK 0xFFFF
// MY EXPLANATION
// 0xFFff = 1111 1111 1111 1111 (16 bits)
// then when you and with an aisle since 0xFFFF has 48 bits of leading zeros it only gets the last 16 bits

// Mask for extracting the spaces bits from a section. (spaces is 10 bits long)
// (section & SPACES_MASK) should preserve all the spaces bits in a section and
// set all non-spaces bits to 0.
#define SPACES_MASK 0x3FF
// MY EXPLANATION
// ONLY NEED 10 1S IN HEX
// 0xF = 1111
// 0x3 = 0011
// 0x3FF = 001111111111 (10 1's)
// then when we and this mask with the section we only get the spaces bits

// Mask for extracting the ID bits from a section.
// (section & ID_MASK) should preserve all the id bits in a section and set all
// non-id bits to 0.
#define ID_MASK 0xFC00
// MY EXPLANATION
// need the FIRST 6 BITS of a 16 bit number
// something like 1111 1100 0000 0000
// 0xFC00 = 1111 1100 0000 0000
// then when you bitwise & this mask with the section you should only get the ids, and zeros for the rest

/* Given a pointer to an aisle and a section index, return the section at the
 * given index of the given aisle.
 *
 * Can assume the index is a valid index (0-3 inclusive).
 */
unsigned short get_section(unsigned long *aisle, int index)
{

  // input long pointer (8 bytes, 64 bits) and int index (4 bytes, 32 bits)
  // need a way to scale the mask based on the index given
  // need to move the mask index(each section is 16 bytes) 1 short * index bytes
  // maybe bitshift the section mask left depending on the index

  unsigned long mask = (unsigned long)SECTION_MASK << index * SECTION_SIZE; // moves 0xFFFF to correct 16 bits

  unsigned short section = (unsigned short)((*aisle & mask) >> (index * SECTION_SIZE)); // bitwise ands with the mask
  //*aisle dereferences the pointer and gets the dat at the aisle
  // then we use a bitwise & to mask out the sections we dont want
  // since after mask it is 64bit unsigneed mask, we then bitshift the masked bits we wanted to the far right
  // do this with the >> (index*section_size) moves bits back to LSB
  // then we cast it to a short, since the 16bits we want aare now in the last 16bits of the unsigned long
  // the bits are not lost when casting

  // does bitwise and on two 8byte unsigned longs
  // casts them to an unsigned short
  //*aisle returns the long data 64bits

  return section;
  // returns unsigned short
}

/* Given a pointer to an aisle and a section index, return the spaces of the
 * section at the given index of the given aisle. The returned short should
 * have the least 10 significant bits set to the spaces and the 6 most
 * significant bits set to 0.
 *
 * Can assume the index is a valid index (0-3 inclusive).
 */
unsigned short get_spaces(unsigned long *aisle, int index)
{
  /*
  given a pointer to an aisle and a section index, return the spaces of the section at the given index of the given aisle
  the returned short should have the lest 10 significant bits set to the spaces and the
  6 most significant bits set to zero
  */

  unsigned short section = get_section(aisle, index); // calls previous get section function instead of copying and
  // pasting the code

  // spaces mask
  // 0x3FF = 001111111111 (10 1's)

  unsigned short spaces = section & SPACES_MASK;
  // works, masks out the least significant 10 bits of the section

  return spaces;
}

/* Given a pointer to an aisle and a section index, return the id of the
 * section at the given index of the given aisle. The returned short should
 * have the least 6 significant bits set to the id and the 10 most significant
 * bits set to 0.
 *
 * Example: if the section is 0b0110010100011010, return 0b0000000000011001.
 *
 * Can assume the index is a valid index (0-3 inclusive).
 */
unsigned short get_id(unsigned long *aisle, int index)
{

  unsigned short section = get_section(aisle, index); // gets the section from given aisle and index of section

  // the returned short should have the LEAST 6 SIGNIFICANT BITS SET TO ID AND 10 MOST SIGNIFICANT BITS SET TO 0

  // ID_MASK
  // 0xFC00 = 1111 1100 0000 0000
  // need to mask out id's from section and bitshift >>10 bits to the right so they are in the least sig fig bits

  unsigned short id = (section & ID_MASK) >> NUM_SPACES; // works
  // ex: section = 1111 1111 1111 1111
  // after mask = 1111 1100 0000 0000
  // bitshift right >> 10 bits(size of) = 0000 0000 0011 1111
  // since it is UNSIGNED the right bitshift does a logical shift (fills left with zeros)

  return id;
}

/* Given a pointer to an aisle, a section index, and a short representing a new
 * bit pattern to be used for section, set the section at the given index of
 * the given aisle to the new bit pattern.
 *
 * Can assume the index is a valid index (0-3 inclusive).
 */
void set_section(unsigned long *aisle, int index, unsigned short new_section)
{

  unsigned long mask = (unsigned long)SECTION_MASK << index * SECTION_SIZE; // moves 0xFFFF to correct 16 bits

  mask = ~mask;
  // flips the mask so that when we use a bitwise & it will keep old data and remove the bits from the section
  // we want to set

  *aisle = *aisle & mask; // clears the section bits, but not other bits

  unsigned long new_section2 = (unsigned long)new_section << index * SECTION_SIZE;
  // have to cast the short(16 bit) new_section to a unsigned long (64bit), then we will have the correct
  // amount of bits to be able to bitshift the new section to where we want it
  // creates a 64bit long with the new section bits we want and moves it to the bits of the section index

  *aisle = (*aisle | new_section2);
}

/* Given a pointer to an aisle, a section index, and a short representing a new
 * bit pattern to be used for the spaces of the section, set the spaces for the
 * section at the given index of the given aisle to the new bit pattern. The
 * new spaces pattern should be in the 10 least significant bits of the given
 * bit pattern. If the new pattern uses bits outside the 10 least significant
 * bits, then the method should leave the spaces unchanged.
 *
 * Can assume the index is a valid index (0-3 inclusive).
 */
void set_spaces(unsigned long *aisle, int index, unsigned short new_spaces)
{

  unsigned short section = get_section(aisle, index); // gets the section

  unsigned short flipped_Spaces_Mask = ~SPACES_MASK;
  // flips so 111111 000000 (masks only item id, so we can get rid of old section)

  // if new section (does not) use bits outside of 10 LSB then set spaces
  // non zero = true, 0 = false
  if (!(new_spaces & flipped_Spaces_Mask))
  {

    section = section & flipped_Spaces_Mask; // gets rid of the old section

    section = section | new_spaces;

    // now need to set the section into the aisle
    set_section(aisle, index, section);
  }

  // if new spaces has more than 10 lsb bits, does nothing
}

/* Given a pointer to an aisle, a section index, and a short representing a new
 * bit pattern to be used for the id of the section, set the id for the section
 * at the given index of the given aisle to the new bit pattern. The new id
 * pattern should be in the 6 least significant bits of the given bit pattern.
 * If the new pattern uses bits outside the 6 least significant bits, then the
 * method should leave the id unchanged.
 *
 * Can assume the index is a valid index (0-3 inclusive).
 */
void set_id(unsigned long *aisle, int index, unsigned short new_id)
{

  unsigned short section = get_section(aisle, index);

  unsigned short bitshift_id = new_id << NUM_SPACES;
  // since the id given will have the new 6 bits in the least significant bits (far right)
  // need to shift to the correct Most significan 6 bits

  unsigned short shifted_id_mask = ID_MASK >> NUM_SPACES;
  // moves the first 6 1111 1100 0000 0000 > 0000 0000 0011 11111
  // new mask can be used to test if any bits are outside of range

  // test to see if any bits are outside of the least sign 6 bits
  // if so then will return non zero value (T) then we !(T) = F
  // so it only runs if we have all zeros(0 =F) then !(F) = T
  if (!(new_id & ~shifted_id_mask))
  {

    // valid so now insert id
    section &= SPACES_MASK; // gets rid of all id bits

    section |= bitshift_id;

    // NEED TO UPDATE THE AISLE DONT FORGET
    // THIS SETS THE SECTION
    set_section(aisle, index, section);
  }
}

/* Given a pointer to an aisle, a section index, and a space index, toggle the
 * item in the given space index of the section at the given section index in
 * the given aisle. Toggling means that if the space was previously empty, it
 * should now be filled with an item, vice-versa.
 *
 * Can assume the section index is a valid index (0-3 inclusive).
 * Can assume the spaces index is a valid index (0-9 inclusive).
 */
void toggle_space(unsigned long *aisle, int index, int space_index)
{

  unsigned short spaces = get_spaces(aisle, index);
  // gets the spaces at the specific section index

  unsigned short flip_bit = 1 << space_index;
  // 1 starts at far right bit, ex: 0000 0000 0000 0001
  // shifts to the left how many times is the index
  // ex: space_index = 2, 1<<2 = 0000 0000 0000 0100
  // index in the section starts at 0 (LSB(most right bit)) to 16 (MSB(leftmostbit))

  spaces ^= flip_bit; // xors the old spaces with the bit to flip
  // if it is a 0^1 = 1, or 1^1 = 0, flips the bit

  set_spaces(aisle, index, spaces);
  // sets the new spaces with the flipped bit to the aisle and section
}

/* Given a pointer to an aisle and a section index, return the number of items
 * in the section at the given index of the given aisle.
 *
 * Can assume the index is a valid index (0-3 inclusive).
 */
unsigned short num_items(unsigned long *aisle, int index)
{

  // return the number of 1s in the spaces in the section

  unsigned short spaces = get_spaces(aisle, index);
  unsigned short count = 0; // a count to keep track of how many 1's found

  // runs while spaces has a nonzero value (nonzero = true)
  // we clear a 1 when we find it and add 1 to the count
  // while loop ends when spaces is all zeros (no more to count)

  while (spaces)
  {

    spaces &= (spaces - 1); // clears the least significant 1 (right most 1)
    // since subtracting from one borrows the least significant 1 in the spaces binary
    //(spaces -1) will have all numbers but the most significant 1
    // so when we and spaces & (spaces -1) the result is without the 1 LSB

    count++; // adds one to count
  }

  return count;
}

/* Given a pointer to an aisle, a section index, and the desired number of
 * items to add, add at most the given number of items to the section at the
 * given index in the given aisle. Items should be added to the least
 * significant spaces possible. If n is larger than or equal to the number of
 * empty spaces in the section, then the section should appear full after the
 * method finishes.
 *
 * Can assume the index is a valid index (0-3 inclusive).
 */
void add_items(unsigned long *aisle, int index, int n)
{

  unsigned short spaces = get_spaces(aisle, index);
  // gets the spaces

  int count = 0;

  // adds 1 to LSB that is 0 while:
  // the count is less than the desired number of items to add(n)
  // and while the value of spaces is less than the spaces mask (0x3FF = 001111111111 (10 1's))
  // where spaces mask represents the max value (or the max amount of items spaces can hold)
  while ((spaces < SPACES_MASK) && (count < n))
  {
    // start with 00000000

    spaces |= (spaces + 1);
    // adding 1 adds 1 to the LSB zero value (most right zero value)
    // ex: 1011 + 1 == 1100
    // then when you or that with the previous value you get
    //  1011 | 1100 = 1111, so it basically adds a one to the LSB that is 0

    count++;
  }

  set_spaces(aisle, index, spaces);
  // set the new spaces into the aisle
}

/* Given a pointer to an aisle, a section index, and the desired number of
 * items to remove, remove at most the given number of items from the section
 * at the given index in the given aisle. Items should be removed from the
 * least significant spaces possible. If n is larger than or equal to the
 * number of items in the section, then the section should appear empty after
 * the method finishes.
 *
 * Can assume the index is a valid index (0-3 inclusive).
 */
void remove_items(unsigned long *aisle, int index, int n)
{
  // remove the n number of items from the spaces
  // remove from LSB to MSB (index 0 - 9)
  // if n is larger than or equal to the number of items

  unsigned short spaces = get_spaces(aisle, index);

  int count = 0;

  // while loop
  // runs as long as the current count of removed items is less than the desired n
  // and while the value of spaces is greater than 0 (while it is not all 0s)
  while ((count < n) && (spaces > 0))
  {

    spaces &= (spaces - 1);
    // since if 0b100, 100 - 1 == 011 (removes LSB)
    // so if we 100 & (100-1) we get == 000 (removed LSB)
    // this subtraction works regardless of where the nonzero LSB is

    count++;
  }

  set_spaces(aisle, index, spaces);
  // set the new spaces into the aisle
}

/* Given a pointer to an aisle, a section index, and a number of slots to
 * rotate by, rotate the items in the section at the given index of the given
 * aisle to the left by the given number of slots.
 *
 * Example: if the spaces are 0b0111100001, then rotating left by 2 results
 *          in the spaces     0b1110000101
 *
 * Can assume the index is a valid index (0-3 inclusive).
 * Can NOT assume n < NUM_SPACES (hint: find an equivalent rotation).
 */
void rotate_items_left(unsigned long *aisle, int index, int n)
{

  // bit shift n values to the left (fills LSBs with 0s)
  // separate spaces bitshifted to the right to fill newly created zeros with msb

  n %= NUM_SPACES; // n is the remainder of n/10

  unsigned short spaces = get_spaces(aisle, index); // gets the spaces at the given section from the aisle

  unsigned short left_shifted_spaces = spaces << n;
  // shifts the spaces << n times,

  unsigned short shifted_mask = SPACES_MASK;       // creates a duplicate mask of the spaces mask
  shifted_mask = shifted_mask << (NUM_SPACES - n); // shifts the SPACES_MASK << (Num_spaces - n)
  //(Num_spaces - n) gives how many indexes within the spaces were not removed by the shift
  // shifts the mask left (Num_spaces - n) times so that the remaining mask should only be over bits that were removed
  shifted_mask &= SPACES_MASK; // we then clear the excess bits that are not inside of SPACES (10LSB)

  unsigned short removed_spaces = (spaces & shifted_mask); // gets the removed bits using the original given spaces & shifted mask
  // shifted mask at this point as 1's only at indexes where a bit was removed from shifting

  spaces = left_shifted_spaces | (removed_spaces >> (NUM_SPACES - n));
  // sets the spaces to the new shift (left_shifted_spaces) | the removed bits
  // we shift the removed spaces (removed_spaces >> (NUM_SPACES - n) ), so that the bits are at index 0 (LSB(most far right bit))
  // since (NUM_SPACES - n) is the number of bits starting from index 0(LSB) that were set to 0 from the shift
  // the bitwise | combines the left_shifted_spaces and removed_spaces bits

  spaces &= SPACES_MASK; // masks out the id part of the spaces
  // gets rid of any extra bits outside of the spaces section

  set_spaces(aisle, index, spaces); // sets the spaces
}
/* Given a pointer to an aisle, a section index, and a number of slots to
 * rotate by, rotate the items in the section at the given index of the given
 * aisle to the right by the given number of slots.
 *
 * Example: if the spaces are 0b1000011110, then rotating right by 2 results
 *          in the spaces     0b1010000111
 *
 * Can assume the index is a valid index (0-3 inclusive).
 * Can NOT assume n < NUM_SPACES (hint: find an equivalent rotation).
 */
void rotate_items_right(unsigned long *aisle, int index, int n)
{

  // bit shift n values to the left (fills LSBs with 0s)
  // separate spaces bitshifted to the right to fill newly created zeros with msb

  n %= NUM_SPACES; // n is the remainder of n/10

  unsigned short spaces = get_spaces(aisle, index); // gets the spaces at the given section from the aisle

  unsigned short right_shifted_spaces = spaces >> n;
  // shifts the spaces >> n times,

  unsigned short shifted_mask = SPACES_MASK;       // creates a duplicate mask of the spaces mask
  shifted_mask = shifted_mask >> (NUM_SPACES - n); // shifts the SPACES_MASK >> (Num_spaces - n)
  //(Num_spaces - n) gives how many indexes within the spaces were not removed by the shift
  // shifts the mask left (Num_spaces - n) times so that the remaining mask should only be over bits that were removed
  shifted_mask &= SPACES_MASK; // we then clear the excess bits that are not inside of SPACES (10LSB)

  unsigned short removed_spaces = (spaces & shifted_mask); // gets the removed bits using the original given spaces & shifted mask
  // shifted mask at this point as 1's only at indexes where a bit was removed from shifting

  spaces = right_shifted_spaces | (removed_spaces << (NUM_SPACES - n));
  // sets the spaces to the new shift (right_shifted_spaces) | the removed bits
  // we shift the removed spaces (removed_spaces << (NUM_SPACES - n) ), so that the bits are at index 10 (MSB(most far left bit of spaces))
  // since (NUM_SPACES - n) is the number of bits starting from index 0(LSB) that were set to 0 from the shift
  // the bitwise | combines the right_shifted_spaces and removed_spaces bits

  spaces &= SPACES_MASK; // masks out the id part of the spaces
  // gets rid of any extra bits outside of the spaces section

  set_spaces(aisle, index, spaces); // sets the spaces
}
