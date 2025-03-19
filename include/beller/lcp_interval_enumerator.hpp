#pragma once
#include <cassert>
#include <chrono>
#include <random>
#include <algorithm>
#include <set>
#include "./beller_component.hpp"

namespace stool
{
    namespace beller
    {
        class LCPIntervalEnumerator
        {
            BellerComponent component;

        public:
            using INDEX = uint64_t;
            using INTERVAL = stool::LCPInterval<INDEX>;
            LCPIntervalEnumerator(IntervalSearchDataStructure<uint8_t> *_range)
            {
                this->component.initialize(_range);
            }

            class LCPIntervalIterator
            {

            public:
                BellerComponent *component = nullptr;
                uint64_t index = 0;
                LCPInterval<INDEX> interval;

                using iterator_category = std::forward_iterator_tag;
                using value_type = stool::LCPInterval<uint64_t>;
                using difference_type = std::ptrdiff_t;
                LCPIntervalIterator(BellerComponent *_component, uint64_t _index, LCPInterval<INDEX> _interval) : component(_component), index(_index), interval(_interval)
                {
                    if (this->component == nullptr)
                    {
                        this->index = UINT64_MAX;
                    }
                }

                LCPInterval<INDEX> operator*() const
                {
                    if (component != nullptr)
                    {
                        return this->interval;
                    }
                    else
                    {
                        throw std::runtime_error("component is nullptr");
                    }
                }

                LCPIntervalIterator &operator++()
                {
                    if (this->component != nullptr)
                    {
                        if (this->component->empty_output_queue())
                        {
                            throw std::runtime_error("component error");
                        }
                        else
                        {
                            this->component->proceed();
                            if (this->component->empty_output_queue())
                            {
                                this->component = nullptr;
                                this->index = UINT64_MAX;
                                this->interval = LCPInterval<INDEX>(UINT64_MAX, UINT64_MAX, UINT64_MAX);
                            }
                            else
                            {
                                this->index++;
                                this->interval = this->component->get_current_lcp_interval();
                            }
                            return *this;
                        }
                    }
                    else
                    {
                        throw std::runtime_error("component is nullptr");
                    }
                    return *this;
                }

                LCPIntervalIterator operator++(int)
                {
                    LCPIntervalIterator temp = *this;
                    ++(*this);
                    return temp;
                }

                bool operator==(const LCPIntervalIterator &other) const { return this->index == other.index; }
                bool operator!=(const LCPIntervalIterator &other) const { return this->index != other.index; }
                bool operator<(const LCPIntervalIterator &other) const { return this->index < other.index; }
                bool operator>(const LCPIntervalIterator &other) const { return this->index > other.index; }
                bool operator<=(const LCPIntervalIterator &other) const { return this->index <= other.index; }
                bool operator>=(const LCPIntervalIterator &other) const { return this->index >= other.index; }
            };

            LCPIntervalIterator begin()
            {
                this->component.initialize(this->component.range);
                return LCPIntervalIterator(&this->component, 0, this->component.get_current_lcp_interval());
            }
            LCPIntervalIterator end()
            {
                return LCPIntervalIterator(nullptr, UINT64_MAX, LCPInterval<INDEX>(UINT64_MAX, UINT64_MAX, UINT64_MAX));
            }

            static std::vector<INTERVAL> compute_lcp_intervals(const sdsl::int_vector<> &bwt, [[maybe_unused]]int message_paragraph = stool::Message::NO_MESSAGE)
            {
                std::vector<uint64_t> C;
                stool::bwt::BWTFunctions::construct_C_array(bwt, C);


                sdsl::wt_huff<> wt;
                construct_im(wt, bwt);

                uint64_t lastChar = bwt[bwt.size() - 1];

                stool::IntervalSearchDataStructure<uint8_t> range;
                range.initialize(&wt, &C, lastChar);

                LCPIntervalEnumerator comp(&range);

                std::vector<INTERVAL> output;
                for (auto it = comp.begin(); it != comp.end(); it++)
                {
                    output.push_back(*it);
                }
                return output;
            }
        };

    } // namespace beller
} // namespace stool