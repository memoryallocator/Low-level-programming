#ifndef LOW_LEVEL_LIST_HIGH_ORDER_FUNCTIONS_H
#define LOW_LEVEL_LIST_HIGH_ORDER_FUNCTIONS_H

#include "linked_list.h"

void foreach(List list, void (* f)(ListDataType));

List map(List list, ListDataType (* f)(ListDataType));

List map_mut(List list, ListDataType (* f)(ListDataType));

ListDataType foldl(ListDataType accum, ListDataType (* f)(ListDataType, ListDataType), List list);

List iterate(ListDataType s, size_t n, ListDataType (* f)(ListDataType));

#endif  // LOW_LEVEL_LIST_HIGH_ORDER_FUNCTIONS_H
