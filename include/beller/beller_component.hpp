#pragma once
#include "./beller_small_component.hpp"

namespace stool
{
    namespace beller
    {
        class BellerComponent
        {
        public:
            using INDEX = uint64_t;
            using INTERVAL = stool::LCPInterval<INDEX>;

            std::vector<std::queue<INTERVAL>> intervalQueues;
            std::vector<bool> checker;

            std::vector<bool> lcp_checker;


            std::vector<uint64_t> counter;
            std::vector<uint8_t> occurrenceChars;
            std::vector<CharInterval<INDEX, uint8_t>> charIntervalTmpVec;

            std::queue<INTERVAL> outputQueue;
            IntervalSearchDataStructure<uint8_t> *range = nullptr;

            uint64_t lcp = 0;
            bool _process_end = false;
            bool output_single_lcp_interval = false;

            void refresh()
            {
                uint64_t bwtSize = this->range->wt->size();
                uint64_t CHARMAX = UINT8_MAX + 1;

                intervalQueues.clear();
                while (outputQueue.size() > 0)
                {
                    outputQueue.pop();
                }
                checker.clear();
                counter.clear();
                occurrenceChars.clear();
                charIntervalTmpVec.clear();
                lcp_checker.clear();

                intervalQueues.resize(CHARMAX);
                counter.resize(CHARMAX, 0);
                checker.resize(bwtSize + 1, false);
                checker[0] = false;

                if(this->output_single_lcp_interval){
                    lcp_checker.resize(bwtSize, false);
                }


                //checker2.resize(bwtSize, false);

                occurrenceChars.resize(0);
                charIntervalTmpVec.resize(CHARMAX);

                this->lcp = 0;
                this->_process_end = false;
            }

            void initialize(IntervalSearchDataStructure<uint8_t> *_range)
            {
                this->range = _range;
                this->refresh();
                this->first_process();
            }

            void process(BellerSmallComponent &bsc)
            {
                // uint64_t k = 0;
                for (auto &c : this->occurrenceChars)
                {
                    auto &que = this->intervalQueues[c];
                    uint64_t queSize = this->counter[c];


                    
                    while (queSize > 0)
                    {
                        bsc.occB = true;

                        auto top = que.front();
                        que.pop();
                        queSize--;

                        //std::cout << "TOP/" << top.to_string() << std::endl;

                        if(this->output_single_lcp_interval){
                            if(!lcp_checker[top.i]){
                                uint64_t lcp_value = top.lcp == 0 ? 0 : top.lcp - 1;
                                this->outputQueue.push(INTERVAL(top.i, top.i, lcp_value));
                            }
                            lcp_checker[top.i] = true;
                        }

                        /*
                        if(this->output_single_lcp_interval && (!checker2[top.j]) && (top.i == top.j)){
                            checker2[top.j] = true;
                                this->outputQueue.push(INTERVAL(top.i-1, top.j-1, 999 + top.lcp));
                        }
                        */


                        if (!this->checker[top.j + 1])
                        {
                            if (bsc.last_lb == UINT64_MAX)
                            {
                                bsc.last_lb = top.i;
                            }

                            this->checker[top.j + 1] = true;
                            bsc.last_idx = top.j + 1;

                            uint64_t charIntvCount = this->range->getIntervals(top.i, top.j, this->charIntervalTmpVec);

                            // this->debugCounter++;

                            for (uint64_t i = 0; i < charIntvCount; i++)
                            {
                                auto &intv = this->charIntervalTmpVec[i];
                                this->intervalQueues[intv.c].push(INTERVAL(intv.i, intv.j, top.lcp + 1));
                                // k++;
                                bsc.next_occurrence_set.insert(intv.c);
                            }
                            
                            
                        }
                        else
                        {
                            this->checker[top.j + 1] = true;
                            if (top.i == bsc.last_idx)
                            {
                                INTERVAL iv(bsc.last_lb, top.j, top.lcp - 1);
                                if(!this->output_single_lcp_interval){
                                    this->outputQueue.push(iv);
                                }

                                bsc.last_lb = UINT64_MAX;
                                bsc.last_idx = UINT64_MAX;

                                uint64_t charIntvCount = this->range->getIntervals(top.i, top.j, this->charIntervalTmpVec);
                                // this->debugCounter++;

                                for (uint64_t i = 0; i < charIntvCount; i++)
                                {
                                    auto &intv = this->charIntervalTmpVec[i];
                                    this->intervalQueues[intv.c].push(INTERVAL(intv.i, intv.j, top.lcp + 1));
                                    // k++;

                                    bsc.next_occurrence_set.insert(intv.c);
                                }
                            }
                        }
                    }
                }
            }
            void compute_LCP_intervals()
            {
                using INTERVAL = stool::LCPInterval<INDEX>;
                uint64_t max_interval_count = 0;

                std::vector<INTERVAL> output_lcp_intervals;

                BellerSmallComponent bsc;
                bsc.initialize();

                uint64_t interval_count = 0;

                this->lcp++;
                for (uint64_t x = 0; x < this->intervalQueues.size(); x++)
                {
                    auto &que = this->intervalQueues[x];
                    uint64_t queSize = que.size();
                    this->counter[x] = queSize;
                    interval_count += queSize;
                }
                if (max_interval_count < interval_count)
                    max_interval_count = interval_count;

                this->process(bsc);

                this->occurrenceChars.resize(0);
                for (auto c : bsc.next_occurrence_set)
                {
                    this->occurrenceChars.push_back(c);
                }
                if (!bsc.occB)
                {
                    this->checker.pop_back();
                    this->_process_end = true;
                }

                // return output_lcp_intervals;
            }

            bool empty_output_queue() const
            {
                return this->outputQueue.size() == 0;
            }

            LCPInterval<INDEX> get_current_lcp_interval() const
            {
                if (this->empty_output_queue())
                {
                    throw std::runtime_error("component is end");
                }
                else
                {
                    return this->outputQueue.front();
                }
            }
            bool proceed()
            {
                if (this->empty_output_queue())
                {
                    return false;
                }
                else
                {
                    this->outputQueue.pop();
                    while (this->outputQueue.size() == 0)
                    {
                        if (this->_process_end)
                        {
                            return true;
                        }
                        else
                        {
                            this->compute_LCP_intervals();
                        }
                    }
                    return true;
                }
            }

        private:
            void first_process()
            {
                uint64_t bwtSize = this->range->wt->size();
                INTERVAL fst(0, bwtSize - 1, 0);
                uint64_t charIntvCount = this->range->getIntervals(fst.i, fst.j, this->charIntervalTmpVec);

                // this->debugCounter++;

                std::set<uint8_t> next_occurrence_set;

                for (uint64_t i = 0; i < charIntvCount; i++)
                {
                    auto &intv = this->charIntervalTmpVec[i];
                    this->intervalQueues[intv.c].push(INTERVAL(intv.i, intv.j, 1));
                    next_occurrence_set.insert(intv.c);
                }

                for (auto c : next_occurrence_set)
                {
                    this->occurrenceChars.push_back(c);
                }

                outputQueue.push(INTERVAL(0, this->range->get_text_size() - 1, 0));
            }
        };


    } // namespace beller
} // namespace stool