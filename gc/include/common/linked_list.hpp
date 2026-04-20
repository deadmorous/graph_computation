#pragma once
// Compatibility shim — use mpk/mix/util/linked_list.hpp directly in new code.
#include "mpk/mix/util/linked_list.hpp"

namespace common
{
using mpk::mix::LinkedListConnectivity;
using mpk::mix::LinkedListConnectivityType;
using mpk::mix::ValueLinkedListConnectivity;
using mpk::mix::ValueLinkedListItem;
using mpk::mix::IntrusiveLinkedListConnectivity;
using mpk::mix::IntrusiveLinkedList;
using mpk::mix::ValueLinkedList;
} // namespace common
