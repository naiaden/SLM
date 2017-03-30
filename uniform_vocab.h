#ifndef _UNIFORM_VOCAB_H_
#define _UNIFORM_VOCAB_H_

#include <cassert>
#include <vector>

#include <pattern.h>
#include <classdecoder.h>



namespace cpyp {

// uniform distribution over a fixed vocabulary
struct UniformVocabulary {
  UniformVocabulary(unsigned vs, double, double, double, double) : p0(1.0 / vs), draws() {/*std::cerr << "[vs:" << vs << ",p0:" << p0 << "]";*/}
  void setVocabSize(unsigned vs) { p0 = 1.0/vs; }
  template<typename Engine>
  void increment(const Pattern&, const Pattern&, Engine&) { ++draws; }
  template<typename Engine>
  void decrement(const Pattern&, const Pattern&, Engine&) { --draws; assert(draws >= 0); }
  double prob(const Pattern& , const Pattern&) const { return p0; }
  double probFullNaiveHelper(const Pattern& w, const Pattern& context, const Pattern& pattern, double p1) const { return p0; }
  double glm_prob(const Pattern& , const Pattern&, const ClassDecoder *const) const { return p0; }
  template<typename Engine>
  void resample_hyperparameters(Engine&) {}
  double log_likelihood() const { /*std::cout << "(draws:" << draws << ",log(p0):" << log(p0) << ")\n";*/ return draws * log(p0); }
  template<class Archive> void serialize(Archive& ar, const unsigned int) {
    ar & p0;
    ar & draws;
  }
  double p0;
  int draws;


};

}

#endif
