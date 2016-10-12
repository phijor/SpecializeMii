#ifndef COMMON_H
#define COMMON_H

#define _assert_offset(type, member, offset)                                   \
    _Static_assert(offsetof(type, member) == (offset),                         \
                   "Offset assertion: " #type "." #member                      \
                   " is not located at offset " #offset "!\n")

#endif /* ----- #ifndef COMMON_H ----- */
