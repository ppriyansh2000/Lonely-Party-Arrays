// Priyansh Patel
// COP 3502c, Fall 2020
// NID - pr348838

#include "LonelyPartyArray.h"
#include <stdlib.h>
#include <stdio.h>

LonelyPartyArray *createLonelyPartyArray(int num_fragments, int fragment_length)
{
  LPA *party;
  int i;

  // check if the paramenters are valid
  if (num_fragments < 1 || fragment_length < 1)
  {
    return NULL;
  }

  party = malloc(sizeof(LPA));

  party->num_fragments = num_fragments;
  party->fragment_length = fragment_length;

  // dynamically create an array of int pointers
  party->fragments = malloc(sizeof(int *) * num_fragments);

  // check if malloc() call successfully executed
  if(party->fragments == NULL)
  {
    free(party->fragments);
    return NULL;
  }

  // dynamically create an array of int
  // (this will store the number used cells per fragment)
  party->fragment_sizes = malloc(sizeof(int) * num_fragments);

  // check if malloc() call successfully executed
  if(party->fragment_sizes == NULL)
  {
    free(party->fragment_sizes);
    free(party->fragments);
    return NULL;
  }

  // initilize
  for (i = 0; i < num_fragments; i++)
  {
    party->fragments[i] = NULL;
    party->fragment_sizes[i] = 0;
  }

  party->num_active_fragments = 0;
  party->size = 0;

  printf("-> A new LonelyPartyArray has emerged from the void. (capacity: %d, "
  "fragments: %d)\n", num_fragments * fragment_length, num_fragments);

  return party;
}

LonelyPartyArray *destroyLonelyPartyArray(LonelyPartyArray *party)
{
  int i;

  if (party == NULL)
  {
    return NULL;
  }

  // free all the dynamically created memory for struct LonelyPartyArray
  for (i = 0; i < party->num_fragments; i++)
  {
    free(party->fragments[i]);
  }

  free(party->fragments);
  free(party->fragment_sizes);
  free(party);

  // check if the free() was successful
  if (party != NULL)
  {
    printf("-> The LonelyPartyArray has returned to the void.\n");
  }

  return NULL;
}

// returns which fragment is the index referring to
int getFragment(LonelyPartyArray *party, int index)
{
  return index / party->fragment_length;
}

// returns the offset of the fragment
int getOffset(LonelyPartyArray *party, int index, int fragment)
{
  return index - (fragment * party->fragment_length);
}

// returns the starting element of the fragment
int getFirstIndex(LonelyPartyArray *party, int fragment)
{
  return fragment * party->fragment_length;
}

// returns the ending element of the fragment
int getSecondIndex(LonelyPartyArray *party, int firstIndex)
{
  return (firstIndex + party->fragment_length) - 1;
}

int set(LonelyPartyArray *party, int index, int key)
{
  int capacity;
  int fragment, offset;
  int index1, index2;
  int i;

  // check if party is NULL pointer
  if (party == NULL)
  {
    printf("-> Bloop! NULL pointer detected in set().\n");
    return LPA_FAILURE;
  }

  // calcultions: this figures out where the index goes
  capacity = getCapacity(party) - 1;

  fragment = getFragment(party, index);
  offset = getOffset(party, index, fragment);

  index1 = getFirstIndex(party, fragment);
  index2 = getSecondIndex(party, index1);

  // check to make sure index is valid
  if (index > capacity || index < 0)
  {
    printf("-> Bloop! Invalid access in set(). (index: %d, fragment: %d, "
    "offset: %d)\n", index, fragment, offset);

    return LPA_FAILURE;
  }
  else
  {
    if (party->fragments[fragment] == NULL)
    {
      // activate fragment depending on the index provided
      party->fragments[fragment] = malloc(sizeof(int) * party->fragment_length);

      // check if malloc() failed
      if (party->fragments[fragment] == NULL)
      {
        free(party->fragments[fragment]);
        return LPA_FAILURE;
      }

      // initilize created array to UNUSED
      for (i = 0; i < party->fragment_length; i++)
      {
        party->fragments[fragment][i] = UNUSED;
      }

      party->num_active_fragments++;

      printf("-> Spawned fragment %d. (capacity: %d, indices: %d..%d)\n",
      fragment, party->fragment_length, index1, index2);
    }

    // insert key in the index and change struct members values to keep track
    if (party->fragments[fragment][offset] == UNUSED)
    {
      party->fragments[fragment][offset] = key;
      party->size++;
      party->fragment_sizes[fragment]++;
    }
    else
    {
      party->fragments[fragment][offset] = key;
    }

    return LPA_SUCCESS;
  }

}

int get(LonelyPartyArray *party, int index)
{
  int capacity;
  int fragment, offset;

  // check if party is a NULL pointer
  if (party == NULL)
  {
    printf("-> Bloop! NULL pointer detected in get().\n");

    return LPA_FAILURE;
  }

  capacity = getCapacity(party) - 1;

  fragment = getFragment(party, index);
  offset = getOffset(party, index, fragment);

  if (index > capacity || index < 0)
  {
    printf("-> Bloop! Invalid access in get(). (index: %d, fragment: %d, "
    "offset: %d)\n", index, fragment, offset);

    return LPA_FAILURE;
  }

  if (party->fragments[fragment] == NULL)
  {
    return UNUSED;
  }
  else
  {
    return party->fragments[fragment][offset];
  }
}

int delete(LonelyPartyArray *party, int index)
{
  int capacity;
  int fragment, offset;
  int index1, index2;

  if (party == NULL)
  {
    printf("-> Bloop! NULL pointer detected in delete().\n");

    return LPA_FAILURE;
  }

  capacity = getCapacity(party) - 1;

  fragment = getFragment(party, index);
  offset = getOffset(party, index, fragment);

  index1 = getFirstIndex(party, fragment);
  index2 = getSecondIndex(party, index1);

  // multiple checks to determine if index is valid
  if (index > capacity || index < 0)
  {
    printf("-> Bloop! Invalid access in delete(). (index: %d, fragment: %d, "
    "offset: %d)\n", index, fragment, offset);

    return LPA_FAILURE;
  }

  if (party->fragments[fragment] == NULL)
  {
    return LPA_FAILURE;
  }

  if (party->fragments[fragment][offset] == UNUSED)
  {
    return LPA_FAILURE;
  }

  // decrementing and deallocating
  party->fragments[fragment][offset] = UNUSED;
  party->size--;
  party->fragment_sizes[fragment]--;

  if (party->fragment_sizes[fragment] == 0)
  {
    free(party->fragments[fragment]);
    party->fragments[fragment] = NULL;
    party->num_active_fragments--;

    printf("-> Deallocated fragment %d. (capacity: %d, indices: %d.."
    "%d)\n", fragment, party->fragment_length, index1, index2);
  }

  return LPA_SUCCESS;
}

int containsKey(LonelyPartyArray *party, int key)
{
  int i, j;

  if (party == NULL)
  {
    return 0;
  }

  // checks if key is in one of the index
  for (i = 0; i < party->num_fragments; i++)
  {
    if (party->fragments[i] != NULL)
    {
      for (j = 0; j < party->fragment_length; j++)
      {
        if (party->fragments[i][j] == key)
        {
          return 1;
        }
      }
    }
  }
  return 0;
}

int isSet(LonelyPartyArray *party, int index)
{
  int capacity;
  int fragment, offset;

  if (party == NULL)
  {
    return 0;
  }

  capacity = getCapacity(party) - 1;

  fragment = getFragment(party, index);
  offset = getOffset(party, index, fragment);

  // multiple checks to determine that there is a key in the index given
  if (index > capacity || index < 0)
  {
    return 0;
  }

  if (party->fragments[fragment] == NULL)
  {
    return 0;
  }

  if (party->fragments[fragment][offset] == UNUSED)
  {
    return 0;
  }

  return 1;
}

int printIfValid(LonelyPartyArray *party, int index)
{
  int capacity;
  int fragment, offset;

  if (party == NULL)
  {
    return LPA_FAILURE;
  }

  capacity = getCapacity(party) - 1;

  fragment = getFragment(party, index);
  offset = getOffset(party, index, fragment);

  // multiple checks to print the key for given index
  if (index > capacity || index < 0)
  {
    return LPA_FAILURE;
  }

  if (party->fragments[fragment] == NULL)
  {
    return LPA_FAILURE;
  }

  if (party->fragments[fragment][offset] == UNUSED)
  {
    return LPA_FAILURE;
  }

  // prints the key
  printf("%d\n", party->fragments[fragment][offset]);
  return LPA_SUCCESS;
}

LonelyPartyArray *resetLonelyPartyArray(LonelyPartyArray *party)
{
  int capacity;
  int i;

  if (party == NULL)
  {
    printf("-> Bloop! NULL pointer detected in resetLonelyPartyArray().\n");
    return party;
  }

  capacity = getCapacity(party);

  // loop through the active fragments reset to NULL
  for (i = 0; i < party->num_fragments; i++)
  {
    free(party->fragments[i]);

    if (party->fragment_sizes[i] != 0)
    {
      party->fragments[i] = NULL;
      party->fragment_sizes[i] = 0;
    }

    // reset to 0 to keep accurate track of stats
    party->num_active_fragments = 0;
    party->size = 0;
  }

  printf("-> The LonelyPartyArray has returned to its nascent state. "
  "(capacity: %d, fragments: %d)\n", capacity, party->num_fragments);

  return party;
}

int getSize(LonelyPartyArray *party)
{
  if (party == NULL)
  {
    return -1;
  }

  return party->size;
}

int getCapacity(LonelyPartyArray *party)
{
  int capacity;

  if (party == NULL)
  {
    return -1;
  }

  return party->num_fragments * party->fragment_length;
}

int getAllocatedCellCount(LonelyPartyArray *party)
{
  if (party == NULL)
  {
    return -1;
  }

  return party->num_active_fragments * party->fragment_length;
}

long long unsigned int getArraySizeInBytes(LonelyPartyArray *party)
{
  int capacity;
  long long unsigned int bytes;

  if (party == NULL)
  {
    return 0;
  }

  capacity = getCapacity(party);
  bytes = (long long unsigned int)sizeof(int) * (long long unsigned int)capacity;

  return bytes;
}

long long unsigned int getCurrentSizeInBytes(LonelyPartyArray *party)
{
  int capacity, fragments, fragments_sizes;
  int active, size_struct;
  long long unsigned int bytes;

  if (party == NULL)
  {
    return 0;
  }

  // calculations: to find bytes of the LPA data structure
  size_struct = sizeof(*party) + sizeof(party);
  fragments = sizeof(int *) * party->num_fragments;
  fragments_sizes = sizeof(int) * party->num_fragments;
  active = sizeof(int) * (party->num_active_fragments * party->fragment_length);
  capacity = size_struct + fragments + fragments_sizes + active;
  bytes = (long long unsigned int)capacity;

  return bytes;
}

// bonus function
LonelyPartyArray *cloneLonelyPartyArray(LonelyPartyArray *party)
{
  LPA *clone_party;
  int i, j, capacity;

  if (party == NULL)
  {
    return NULL;
  }

  clone_party = malloc(sizeof(LPA));

  // making copies of struct int members
  clone_party->num_fragments = party->num_fragments;
  clone_party->fragment_length = party->fragment_length;
  clone_party->size = party->size;
  clone_party->num_active_fragments = party->num_active_fragments;

  // used in printing
  capacity = getCapacity(party);

  clone_party->fragments = malloc(sizeof(int *) * clone_party->num_fragments);

  // check if malloc() failed
  if (clone_party->fragments == NULL)
  {
    free(clone_party->fragments);
    return NULL;
  }

  clone_party->fragment_sizes = malloc(sizeof(int) * clone_party->num_fragments);

  // check if malloc() failed
  if (clone_party->fragments == NULL)
  {
    free(clone_party->fragments);
    return NULL;
  }


  for (i = 0; i < clone_party->num_fragments; i++)
  {
    clone_party->fragments[i] = NULL;

    // clone fragment_sizes array
    clone_party->fragment_sizes[i] = party->fragment_sizes[i];

    // checks if fragments need to be dynamically allocated
    if (clone_party->fragment_sizes[i] != 0)
    {
      clone_party->fragments[i] = malloc(sizeof(int) * clone_party->fragment_length);

      if (clone_party->fragments[i] == NULL)
      {
        free(clone_party->fragments[i]);
        return NULL;
      }
      // each element is initilized to UNUSED
      for (j = 0; j < clone_party->fragment_length; j++)
      {
        clone_party->fragments[i][j] = party->fragments[i][j];
      }
    }
  }

  printf("-> Successfully cloned the LonelyPartyArray. (capacity: %d, fragments: "
  "%d)\n", capacity, clone_party->num_fragments);

  return clone_party;
}

// this function is for me to test in main
// checks all the struct field values
void check_struct(LonelyPartyArray *party)
{
  int i, j;

  if (party == NULL)
  {
    printf("LPA is NULL Pointer\n");
  }
  else
  {
    printf("\n----------------------------------------------------------\n");
    printf("Checking Current Struct Values............\n\n");
    printf("num_fragments: %d\n", party->num_fragments);
    printf("fragment_length: %d\n", party->fragment_length);
    printf("num_active_fragments: %d\n", party->num_active_fragments);
    printf("size: %d\n", party->size);

    printf("\n");
    printf("fragment_sizes:\n");
    for (i = 0; i < party->num_fragments; i++)
    {
      if (party->fragment_sizes[i] != 0)
      {
        printf("[%d]: %d\n", i, party->fragment_sizes[i]);
      }
    }

    printf("\n");
    printf("fragments:\n");

    for (i = 0; i < party->num_fragments; i++)
    {
      if (party->fragments[i] != NULL)
      {
        for (j = 0; j < party->fragment_length; j++)
        {
          if (party->fragments[i][j] != UNUSED)
          {
            printf("Fragment[%d][%d]: %d\n", i, j, party->fragments[i][j] );
          }
        }
        printf("\n");
      }
    }
    printf("----------------------------------------------------------\n");
  }
}

double difficultyRating(void)
{
  return 3.2;
}

double hoursSpent(void)
{
  return 35.0;
}
