#pragma once
#include <cassert>
#include <chrono>
#include <random>
#include <algorithm>
#include <set>
#include "./beller_component.hpp"
#include "./lcp_info.hpp"
#include "../bwt/bwt_functions.hpp"

namespace stool
{
    namespace beller
    {
        class LCPEnumerator
        {
            BellerComponent component;

        public:
            using INDEX = uint64_t;
            using INTERVAL = stool::LCPInterval<INDEX>;
            LCPEnumerator(IntervalSearchDataStructure<uint8_t> *_range)
            {
                this->component.initialize(_range);
                this->component.output_single_lcp_interval = true;
            }

            class LCPIterator
            {

            public:
                BellerComponent *component = nullptr;
                uint64_t index = 0;
                LCPInfo info;

                using iterator_category = std::forward_iterator_tag;
                using value_type = LCPInfo;
                using difference_type = std::ptrdiff_t;
                LCPIterator(BellerComponent *_component, uint64_t _index, LCPInfo _info) : component(_component), index(_index), info(_info)
                {
                    if (this->component == nullptr)
                    {
                        this->index = UINT64_MAX;
                    }
                }

                LCPInfo operator*() const
                {
                    if (component != nullptr)
                    {
                        return this->info;
                    }
                    else
                    {
                        throw std::runtime_error("component is nullptr");
                    }
                }

                LCPIterator &operator++()
                {
                    if (this->component != nullptr)
                    {
                        if (this->index == 0)
                        {
                            this->index++;
                            auto intv = this->component->get_current_lcp_interval();
                            this->info.lcp = intv.lcp;
                            this->info.position = intv.i;
                        }
                        else
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
                                    this->info.lcp = UINT64_MAX;
                                    this->info.position = UINT64_MAX;
                                }
                                else
                                {
                                    this->index++;
                                    auto intv = this->component->get_current_lcp_interval();
                                    this->info.lcp = intv.lcp;
                                    this->info.position = intv.i;
                                }
                                return *this;
                            }
                        }
                    }
                    else
                    {
                        throw std::runtime_error("component is nullptr");
                    }
                    return *this;
                }

                LCPIterator operator++(int)
                {
                    LCPIterator temp = *this;
                    ++(*this);
                    return temp;
                }

                bool operator==(const LCPIterator &other) const { return this->index == other.index; }
                bool operator!=(const LCPIterator &other) const { return this->index != other.index; }
                bool operator<(const LCPIterator &other) const { return this->index < other.index; }
                bool operator>(const LCPIterator &other) const { return this->index > other.index; }
                bool operator<=(const LCPIterator &other) const { return this->index <= other.index; }
                bool operator>=(const LCPIterator &other) const { return this->index >= other.index; }
            };

            LCPIterator begin()
            {
                this->component.initialize(this->component.range);
                return LCPIterator(&this->component, 0, LCPInfo(0, 0));
            }
            LCPIterator end()
            {
                return LCPIterator(nullptr, UINT64_MAX, LCPInfo(UINT64_MAX, UINT64_MAX));
            }

            static std::vector<uint64_t> construct_LCP_array(const sdsl::int_vector<> &bwt, [[maybe_unused]]int message_paragraph = stool::Message::SHOW_MESSAGE)
            {
                std::vector<uint64_t> C;
                stool::bwt::BWTFunctions::construct_C_array(bwt, C);

                sdsl::wt_huff<> wt;
                construct_im(wt, bwt);

                uint64_t lastChar = bwt[bwt.size() - 1];

                stool::IntervalSearchDataStructure<uint8_t> range;
                range.initialize(&wt, &C, lastChar);

                LCPEnumerator comp(&range);

                std::vector<uint64_t> output;
                output.resize(bwt.size(), UINT64_MAX);

                for (auto it = comp.begin(); it != comp.end(); it++)
                {
                    output[(*it).position] = (*it).lcp;
                }
                return output;
            }
            static std::vector<uint64_t> compute_lcp_statistics(const sdsl::int_vector<> &bwt, int message_paragraph = stool::Message::SHOW_MESSAGE)
            {

                if(message_paragraph != stool::Message::NO_MESSAGE){
                    std::cout << stool::Message::get_paragraph_string(message_paragraph) << "Computing LCP statistics..." << std::endl;
                }

                std::vector<uint64_t> C;
                stool::bwt::BWTFunctions::construct_C_array(bwt, C, stool::Message::add_message_paragraph(message_paragraph));

                sdsl::wt_huff<> wt;

                if(message_paragraph != stool::Message::NO_MESSAGE){
                    std::cout << stool::Message::get_paragraph_string(message_paragraph+1) << "Constructing wavelet tree..." << std::flush;
                }
                construct_im(wt, bwt);
                if(message_paragraph != stool::Message::NO_MESSAGE){
                    std::cout << "[DONE]" << std::endl;
                }

                uint64_t lastChar = bwt[bwt.size() - 1];

                stool::IntervalSearchDataStructure<uint8_t> range;
                range.initialize(&wt, &C, lastChar);

                LCPEnumerator comp(&range);


                if(message_paragraph != stool::Message::NO_MESSAGE){
                    std::cout << stool::Message::get_paragraph_string(message_paragraph+1) << "Computing LCP values..." << std::flush;
                }

                std::vector<uint64_t> r;
                uint64_t counter = 0;
                for (auto it = comp.begin(); it != comp.end(); it++)
                {
                    counter++;
                    if(message_paragraph != stool::Message::NO_MESSAGE && counter % 10000000 == 0){
                        uint64_t text_mb = bwt.size() / 1000000;
                        uint64_t counter_mb = counter / 1000000;
                        std::cout << "\r" << stool::Message::get_paragraph_string(message_paragraph+1) << "Computing LCP values... [" << counter_mb << "/" << text_mb << "MB]" << std::flush;
                    }
                    while (r.size() <= (*it).lcp)
                    {
                        r.push_back(0);
                    }
                    r[(*it).lcp]++;
                }
                if(message_paragraph != stool::Message::NO_MESSAGE){
                    std::cout << "[DONE]" << std::endl;
                }

                if(message_paragraph != stool::Message::NO_MESSAGE){
                    std::cout << "\r" << stool::Message::get_paragraph_string(message_paragraph) << "Computing LCP statistics[DONE]" << std::endl;
                }


                return r;
            }
        };

    } // namespace beller
} // namespace stool