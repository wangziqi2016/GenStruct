
#ifndef _BINTREE_H
#define _BINTREE_H

// The following must be overridden if the default key comparison function is not
// built-in comparison operation
#ifndef BINTREE_KEY_LESS
#define BINTREE_KEY_LESS(x, y) ((uint64_t)x < (uint64_t)y)
#else

#ifndef BINTREE_KEY_EQ
#define BINTREE_KEY_EQ(x, y) ((uint64_t)x == (uint64_t)y)
#else 



#endif