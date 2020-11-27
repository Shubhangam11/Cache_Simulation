#include "YOURCODEHERE.h"




void  setSizesOffsetsAndMaskFields(cache* acache, unsigned int size, unsigned int assoc, unsigned int blocksize){

  unsigned int localVAbits=8*sizeof(uint64_t*);

  if (localVAbits!=64){

    fprintf(stderr,"Running non-portable code on unsupported platform, terminating. Please use designated machines.\n");

    exit(-1);
  }


  unsigned int numSet;

  unsigned int sizeIndex;

  acache->numways = assoc;
 
  acache->blocksize = blocksize;

  acache->numsets = (size / blocksize) / assoc;

  acache->BO = 0;

  // get the log2 of blocksize

  while (blocksize >= 2) {

    blocksize /= 2;

    acache->BO++;
  }

  sizeIndex = 0;

  numSet = acache->numsets;

  // get the log2 of numSet

  while (numSet >= 2) {

    numSet /= 2;

    sizeIndex++;
  }

  acache->VAImask = 0;

  // create index andmask

  for (int curBitI = 0; curBitI < sizeIndex; ++curBitI) {

    acache->VAImask |= ((uint64_t)1 << curBitI);
  }

  acache->TO = acache->BO + sizeIndex;

  unsigned int sizeTag = localVAbits - acache->TO;

  acache->VATmask = 0;

  // create tag andmask

  for (int curBitT = 0; curBitT < sizeTag; ++curBitT) {
    acache->VATmask |= ((uint64_t)1 << curBitT);
  }

}


unsigned long long getindex(cache* acache, unsigned long long address){
 
 unsigned long long index;

  index = address >> acache->BO;

  index &= acache->VAImask;

  return index;

 // return 0; //Replace with correct value
}

unsigned long long gettag(cache* acache, unsigned long long address){
   unsigned long long tag;
  
 tag = address >> acache->TO;

  tag &= acache->VATmask;

  return tag;	 

}

void writeback(cache* acache, unsigned int index, unsigned int oldestway){

const int word_size = 8;

  unsigned long long address = 0;
 
 unsigned long long tag = 0;

  tag = acache -> sets[index].blocks[oldestway].tag;

  address = (tag << (acache -> TO)) + (index << (acache -> BO));

  int i =0;

    while (i < (acache -> blocksize/word_size)){
    performaccess(acache -> nextcache, (word_size * i + address), 8, 1, acache -> sets[index].blocks[oldestway].datawords[i], i);
	i++;
  }

}

void fill(cache * acache, unsigned int index, unsigned int oldestway, unsigned long long address){

  const int word_size = 8;

  unsigned long long value = 0;


  unsigned long long base_address;


  base_address = (address/acache -> blocksize) * (acache -> blocksize);


  for (int i = 0; i < (acache -> blocksize/word_size); i++){


    value = performaccess(acache -> nextcache, (word_size * i + base_address), 8, 0, 0, i);

    acache -> sets[index].blocks[oldestway].datawords[i] = value;

  }
}
