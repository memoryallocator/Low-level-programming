#ifndef LOW_LEVEL_LIST_HIGH_ORDER_FUNCTIONS_H
#define LOW_LEVEL_LIST_HIGH_ORDER_FUNCTIONS_H

#include "linked_list.h"

void foreach(struct List list, void (* f)(ListDataType));

struct List map(struct List list, ListDataType (* f)(ListDataType));

struct List map_mut(struct List list, ListDataType (* f)(ListDataType));

ListDataType foldl(ListDataType accum, ListDataType (* f)(ListDataType, ListDataType), struct List list);

struct List iterate(ListDataType s, size_t n, ListDataType (* f)(ListDataType));

#endif  // LOW_LEVEL_LIST_HIGH_ORDER_FUNCTIONS_H
