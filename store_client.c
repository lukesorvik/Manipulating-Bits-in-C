/*
 * CSE 351 Lab 1b (Manipulating Bits in C)
 *
 * Name(s):  Luke Sorvik
 * NetID(s): lukesorv
 *
 * This is a file for managing a store of various aisles, represented by an
 * array of 64-bit integers. See aisle_manager.c for details on the aisle
 * layout and descriptions of the aisle functions that you may call here.
 *
 * Written by Porter Jones (pbjones@cs.washington.edu)
 */

#include <stddef.h> // To be able to use NULL
#include "aisle_manager.h"
#include "store_client.h"
#include "store_util.h"
#include <stdio.h>

// Number of aisles in the store
#define NUM_AISLES 10
// the number of 64 bit aisles we can keep in the store
// the store is an array of 64 bit aisles (8byte)

// Number of sections per aisle
// each section 16 bits
#define SECTIONS_PER_AISLE 4

//..........................................
// The number of bits in a section used for the item spaces
#define NUM_SPACES 10
// I ADDED THIS IN
//..........................................

// Number of items in the stockroom (2^6 different id combinations)
// 6 bits for the item part of a section, 2^6 = 64
#define NUM_ITEMS 64

// Global array of aisles in this store. Each unsigned long in the array
// represents one aisle.
unsigned long aisles[NUM_AISLES];
// creates an array of size 10, or 8 byte(64 bit aisles)

// Array used to stock items that can be used for later. The index of the array
// corresponds to the item id and the value at an index indicates how many of
// that particular item are in the stockroom.
int stockroom[NUM_ITEMS];
// array to store 64 items
// the index of an item corresponds to that items id
// value at that index indicates how many of that item are int the stock room
// ex index[2], item 2, stockroom[2] = 5, 5 in stockroom

/* Starting from the first aisle, refill as many sections as possible using
 * items from the stockroom. A section can only be filled with items that match
 * the section's item id. Prioritizes and fills sections with lower addresses
 * first. Sections with lower addresses should be fully filled (if possible)
 * before moving onto the next section.
 */
void refill_from_stockroom()
{
  // starting from index 0 of aisles[], refill as many sections as possible using items from the stockroom
  // fill empty spaces in a section with items from the stock room
  // prioritizes and fills sections with lower addresses first
  // sections with lower addresses should be fully filled

  // for loop that iterates through every aisle
  for (int aisle = 0; aisle < NUM_AISLES; aisle++)
  {

    // iterates through the sections in an aisle
    for (int section = 0; section < SECTIONS_PER_AISLE; section++)
    {

      // section is 16 bits (size of long)
      //  " Prioritizes and fills sections with lower addresses* first."
      // get aisle, use pointer arithmetic to get the section

      // gets item id of section
      unsigned short item_id = get_id(&aisles[aisle], section);

      // Check if there are items in the stockroom for this item id
      if (stockroom[item_id] > 0)
      {

        // Calculate the number of empty spaces in the section
        int empty_spaces = NUM_SPACES - num_items(&aisles[aisle], section);
        // num items returns the number of items in the section

        // if the empty spaces is less than the number of items availiable in stockroom
        while ((stockroom[item_id] > 0) && (empty_spaces > 0))
        {
          // probably weird case where empty spaces is greater than the stockroom
          // need to fill anyway
          // so do while(stockroom[itemid] > 0) (not empty)
          // add one item and subtract one item from stockroom

          // Update the aisle with the new items
          add_items(&aisles[aisle], section, 1);
          // adds empty_spaces # of items

          empty_spaces--; // removes an empty space

          // Reduce the stockroom count for this item
          stockroom[item_id]--;
        }
      }
    }
  }
}

/* Remove at most num items from sections with the given item id, starting with
 * sections with lower addresses, and return the total number of items removed.
 * Multiple sections can store items of the same item id. If there are not
 * enough items with the given item id in the aisles, first remove all the
 * items from the aisles possible and then use items in the stockroom of the
 * given item id to finish fulfilling an order. If the stockroom runs out of
 * items, you should remove as many items as possible.
 */
int fulfill_order(unsigned short id, int num)
{
  // remove num items from sections that have the given item id
  // start at lowest section (0)
  // return the number of items removed
  // multiple sections can have the same item id
  // if there are not enough items with the given item id in the aisles, first remove all the items from the aisles possible,
  // then use items in the stockroom of the given item id to finish fullfilling an order
  // if the stockroom runs out of items you should remove as many items as possible

  int count = 0; // keeps count of number of items removed

  // iterates through each isle
  for (int aisle = 0; aisle < NUM_AISLES; aisle++)
  {

    // iterates through the sections in an aisle
    for (int section = 0; section < SECTIONS_PER_AISLE; section++)
    {

      int items_in_section = num_items(&aisles[aisle], section);

      // if the current section has the item id we need to remove
      if (id == get_id(aisles + aisle, section))
      {

        // check if how many items to remove is less than num
        // if it is not (items in section > num to remove), else, remove as many items as it can ( num)

        if (num > 0)
        {
          // if there are numbers to still be removed
          // while there are items in the section && the num(number of items to remove) is greater than 0

          int items_to_remove;
          if (items_in_section < num)
          {
            // if the items in the section is less than the items to remove
            items_to_remove = items_in_section;
          }

          else
          {
            // if items in section > num
            items_to_remove = num;
          }

          // YES IT WORKS ^^^^
          // PROBLEM WAS WHEN SECTION > NUM, WAS SETTING COUNT TO NUMBER OF ITEMS IN THE SECTION NOT THE NUMBER OF ITEMS REMOVED

          remove_items(aisles + aisle, section, items_to_remove); // removes the number nums from section
          // if nums greater than items in section, clears all in section
          // if nums less than in section, removes as many possible

          // adding too much if nums is less than
          count += items_to_remove; // adds to removed counts

          // when num is < items in the section it incorrectly sets num to negatative
          // num = 4, items =5'
          num -= items_to_remove; // removes 1 from count to remove
        }
      }
    }
  }

  // if reaches this point have iterated through all sections and aisles
  //remove from stockpile if needed

  if (((stockroom[id]) > 0) && (num > 0))
  {
    // if the stockroom of the item still has items, and the number of items that need to be removed still has items

    if (stockroom[id] < num)
    {
      // if the items to remove is greater than the numbers in the stockroom
      num -= stockroom[id]; // subtracts the number of stock from the nums to be removed
      count += stockroom[id];
      stockroom[id] = 0; // sets the stock of that item to 0
    }

    else
    {
      // stockroom[id] > num
      stockroom[id] -= num;
      count += num;
      num = 0;
    }
  }

  return count; //returns the count of items removed
}


/* Return a pointer to the first section in the aisles with the given item id
 * that has no items in it or NULL if no such section exists. Only consider
 * items stored in sections in the aisles (i.e., ignore anything in the
 * stockroom). Break ties by returning the section with the lowest address.
 */
unsigned short *empty_section_with_id(unsigned short id)
{
  //return a pointer to the first section in the aisles with the given item id
  //the section MUST have no items OR returns null if no such section exists
  //only consider items stored in the sections (dont consider stockroom)
  //break ties by returning the section with the lowest address

  //iterate through each section,
  //if current section matches the id AND is fully empty
  //return the address of the section


  //iterates through the aisles
  for (int aisle = 0; aisle < NUM_AISLES; aisle++)
  {

    // iterates through the sections in an aisle
    for (int section = 0; section < SECTIONS_PER_AISLE; section++)
    {
      
      //if the id and section id match && the items in the section are empty
      if (id == get_id(aisles + aisle, section) && (num_items(&aisles[aisle], section) == 0 )) {
        //return *get_section(&aisles[aisle], section); //returns the section found, DOESNT WORK RETURNS THE VALUE OF THE SECTION

        unsigned short *empty_section = (unsigned short *)(aisles + aisle) + section;
        //unsigned short *empty_section - creates a 8byte pointer to the empty section's address
        //(unsigned short *)(aisles + aisle) gets the address of the aisle in the aisles array, then casts it to an unsigned short pointer (unsigned short *)
        //we cast it since the aisles are stored as unsigned longs(8bytes) so when doing pointer arithmetic it moves in increments of 8 bytes
        //but we want it to move in section increments(2 bytes) when finding the address of the section
        //(unsigned short *)(aisles + aisle) + section -> has the address of the aisle the empty section is in (casted to a unsigned short* 2 bytes)
        //and increments it +section *2bytes foward
        //so if the section that is empty is section 2, and the aisle is 1 it would be
        // aisles[1] + 2 *2 = aisles[1] + 4 bytes foward = adress of the section

        return empty_section;
        
      }
     
    }
  }




  return NULL;
}

/* Return a pointer to the section with the most items in the store. Only
 * consider items stored in sections in the aisles (i.e., ignore anything in
 * the stockroom). Break ties by returning the section with the lowest address.
 */
unsigned short *section_with_most_items()
{
  
  //return a pointer to the section with the MOST ITEMS in the store. 
  //only consider items stored in the aisles
  //break ties by returning the section with the lowest adress


  unsigned short* most_section = (unsigned short *)(aisles) + 0; //pointer to the section with the most items
  //have to point the most_section to the first section in the aisles array because when all the aisles are 0, it returns null 
  //when it should return the first address
  //also if we default to the first section and it ends up not being the max, it will be replaced as we iterate through
  int most_items = 0; //running max

  
  //iterates through the aisles
  for (int aisle = 0; aisle < NUM_AISLES; aisle++)
  {

    // iterates through the sections in an aisle
    for (int section = 0; section < SECTIONS_PER_AISLE; section++)
    {
      
      int current_items = num_items(aisles + aisle, section); //makes a count of how many items are in this section
       unsigned short *section_address = (unsigned short *)(aisles + aisle) + section; //gets the address of the current section

       if (current_items > most_items) {
        //if current items is greater than the running max value
        most_section = section_address; //set the address of the section with the most items to this one
        most_items = current_items; //set the most items found to the items in this section
       }
     
    }
  }



  return most_section; //returns the section with the most items in it
}
