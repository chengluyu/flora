#ifndef FLOAR_H
#define FLOAR_H

#define NOCOPY_CLASS(class_name)\
  class_name(const class_name&) = delete;\
  class_name(class_name&&) = delete;\
  class_name& operator= (const class_name&) = delete;\
  class_name& operator= (class_name&&) = delete;

#define STATIC_CLASS(class_name)\
  class_name() = delete;\
  NOCOPY_CLASS(class_name)

#endif