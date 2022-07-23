#include "counter.hpp"

namespace dummy::components {

DummyCounter::DummyCounter() : counter_{0} {}

DummyCounter::~DummyCounter() {}

const char* DummyCounter::Name() const {
    return kName;
}

void DummyCounter::Init() {}

void DummyCounter::Reset() {
    counter_.store(0);
}

std::size_t DummyCounter::GetValue() const {
    return counter_.load();
}

size_t DummyCounter::AddOne() {
    return ++counter_;
}

} // namespace dummy::components