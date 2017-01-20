#ifndef UVECTOR_H_
#define UVECTOR_H_

#include <vector>
#include <pattern.h>

struct uvector_hash {
  size_t operator()(const std::vector<unsigned>& v) const {
    size_t h = v.size();
    for (auto e : v)
      h ^= e + 0x9e3779b9 + (h<<6) + (h>>2);
    return h;
  }
};

struct pattern_hash {
  size_t operator()(Pattern& p) const {
    return p.hash();
  }
};

#endif
