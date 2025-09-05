#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <cassert>
#include <chrono>
#include <random>
#include <bitset>
#include "../../../include/light_stool.hpp"
#include "./load_and_save_test.hpp"
#include "./build_test.hpp"
#include "./update_test.hpp"
#include "./query_test.hpp"

namespace stool
{

    template <typename INTEGER_CONTAINER, bool SUPPORT_PSUM, bool SUPPORT_SEARCH>
    class DynamicIntegerTest
    {
    public:
        std::vector<uint64_t> _inputs;
        std::vector<uint64_t> _naive_values;
        uint64_t _max_value;
        uint64_t _seed;

        DynamicIntegerTest()
        {
        }

        std::function<INTEGER_CONTAINER(uint64_t, int64_t)> builder_function = [&](uint64_t i, [[maybe_unused]] int64_t message_paragraph)
        {
            if (i == 0 || _inputs[i] != _inputs[i - 1])
            {
                std::cout << stool::Message::get_paragraph_string(message_paragraph) << "len = " << _inputs[i] << ": " << std::flush;
            }

            uint64_t len = _inputs[i];
            auto tmp = stool::StringGenerator::create_random_integer_sequence(len, _max_value, _seed++);
            _naive_values.swap(tmp);
            INTEGER_CONTAINER test_container = INTEGER_CONTAINER::build(_naive_values);
            std::cout << "+" << std::flush;

            if (i == _inputs.size() - 1 || _inputs[i] != _inputs[i + 1])
            {
                std::cout << std::endl;
            }
            return test_container;
        };
        std::function<bool(INTEGER_CONTAINER &, uint64_t, int64_t)> equal_check_function = [&](INTEGER_CONTAINER &obj, [[maybe_unused]] uint64_t i, [[maybe_unused]] int64_t message_paragraph)
        {
            std::vector<uint64_t> test_values = obj.to_vector();
            stool::EqualChecker::equal_check(_naive_values, test_values, "EQUAL_CHECK_FUNCTION");
            return true;
        };

        std::function<bool(INTEGER_CONTAINER &, INTEGER_CONTAINER &)> equal_check_function3 = [&](INTEGER_CONTAINER &obj1, INTEGER_CONTAINER &obj2)
        {
            std::vector<uint64_t> test_values = obj1.to_vector();
            std::vector<uint64_t> correct_values = obj2.to_vector();
            stool::EqualChecker::equal_check(correct_values, test_values, "EQUAL_CHECK_FUNCTION");
            return true;
        };

        std::function<INTEGER_CONTAINER(uint64_t, int64_t)> test_builder_function = [&](uint64_t i, [[maybe_unused]] int64_t message_paragraph)
        {
            if (i == 0 || _inputs[i] != _inputs[i - 1])
            {
                std::cout << stool::Message::get_paragraph_string(message_paragraph) << "len = " << _inputs[i] << ": " << std::flush;
            }

            uint64_t len = _inputs[i];
            auto tmp = stool::StringGenerator::create_random_integer_sequence(len, _max_value, _seed++);
            _naive_values.swap(tmp);
            INTEGER_CONTAINER test_container = INTEGER_CONTAINER::build(_naive_values);
            std::cout << "+" << std::flush;

            if (i == _inputs.size() - 1 || _inputs[i] != _inputs[i + 1])
            {
                std::cout << std::endl;
            }
            return test_container;
        };

        std::function<std::vector<uint64_t>(uint64_t, int64_t)> correct_builder_function = [&](uint64_t i, [[maybe_unused]] int64_t message_paragraph)
        {
            std::vector<uint64_t> obj;
            for (uint64_t x = 0; x < _inputs[i]; x++)
            {
                obj.push_back(_naive_values[x]);
            }
            return obj;
        };

        std::function<void(INTEGER_CONTAINER &, std::vector<uint64_t> &, uint64_t, int64_t)> insert_function = [&](INTEGER_CONTAINER &test_obj, std::vector<uint64_t> &correct_obj, [[maybe_unused]] uint64_t i, [[maybe_unused]] int64_t message_paragraph)
        {
            std::mt19937 mt(_seed++);
            std::uniform_int_distribution<uint64_t> get_rand_value(0, UINT64_MAX);
            uint64_t new_value = get_rand_value(mt) % _max_value;
            uint64_t pos = get_rand_value(mt) % (correct_obj.size() + 1);

            correct_obj.insert(correct_obj.begin() + pos, new_value);
            test_obj.insert(pos, new_value);
        };

        std::function<void(INTEGER_CONTAINER &, std::vector<uint64_t> &, uint64_t, int64_t)> remove_function = [&](INTEGER_CONTAINER &test_obj, std::vector<uint64_t> &correct_obj, [[maybe_unused]] uint64_t i, [[maybe_unused]] int64_t message_paragraph)
        {
            std::mt19937 mt(_seed++);
            std::uniform_int_distribution<uint64_t> get_rand_value(0, UINT64_MAX);
            if (correct_obj.size() > 0)
            {
                uint64_t pos = get_rand_value(mt) % (correct_obj.size());

                correct_obj.erase(correct_obj.begin() + pos);
                test_obj.remove(pos);
            }
        };

        std::function<void(INTEGER_CONTAINER &, std::vector<uint64_t> &, uint64_t, int64_t)> replace_function = [&](INTEGER_CONTAINER &test_obj, std::vector<uint64_t> &correct_obj, [[maybe_unused]] uint64_t i, [[maybe_unused]] int64_t message_paragraph)
        {
            std::mt19937 mt(_seed++);
            std::uniform_int_distribution<uint64_t> get_rand_value(0, UINT64_MAX);
            uint64_t pos = get_rand_value(mt) % (correct_obj.size());
            uint64_t new_value = get_rand_value(mt) % _max_value;

            correct_obj[pos] = new_value;
            test_obj.set_value(pos, new_value);
        };

        std::function<void(INTEGER_CONTAINER &, std::vector<uint64_t> &, uint64_t, int64_t)> push_back_function = [&](INTEGER_CONTAINER &test_obj, std::vector<uint64_t> &correct_obj, [[maybe_unused]] uint64_t i, [[maybe_unused]] int64_t message_paragraph)
        {
            std::mt19937 mt(_seed++);
            std::uniform_int_distribution<uint64_t> get_rand_value(0, UINT64_MAX);
            uint64_t new_value = get_rand_value(mt) % _max_value;

            correct_obj.push_back(new_value);
            test_obj.push_back(new_value);
        };

        std::function<void(INTEGER_CONTAINER &, std::vector<uint64_t> &, uint64_t, int64_t)> pop_back_function = [&](INTEGER_CONTAINER &test_obj, std::vector<uint64_t> &correct_obj, [[maybe_unused]] uint64_t i, [[maybe_unused]] int64_t message_paragraph)
        {
            std::mt19937 mt(_seed++);
            std::uniform_int_distribution<uint64_t> get_rand_value(0, UINT64_MAX);
            if (correct_obj.size() > 0)
            {
                correct_obj.pop_back();
                test_obj.pop_back();
            }
        };

        std::function<bool(INTEGER_CONTAINER &, std::vector<uint64_t> &)> equal_check_function2 = [&](INTEGER_CONTAINER &test_obj, std::vector<uint64_t> &correct_obj)
        {
            std::vector<uint64_t> test_values = test_obj.to_vector();
            stool::EqualChecker::equal_check(correct_obj, test_values, "INSERT_TEST");
            return true;
        };

        std::function<void(INTEGER_CONTAINER &, std::vector<uint64_t> &, uint64_t, int64_t)> psum_function = [&](INTEGER_CONTAINER &test_obj, std::vector<uint64_t> &correct_obj, [[maybe_unused]] uint64_t i, [[maybe_unused]] int64_t message_paragraph)
        {
            if constexpr (SUPPORT_PSUM)
            {
                std::mt19937 mt(_seed++);
                std::uniform_int_distribution<uint64_t> get_rand_value(0, UINT64_MAX);

                uint64_t pos = get_rand_value(mt) % (correct_obj.size());
                uint64_t psum1 = DynamicIntegerTest::compute_psum(correct_obj, pos);
                uint64_t psum2 = test_obj.psum(pos);
                if (psum1 != psum2)
                {
                    std::cout << "psum_test error" << std::endl;
                    std::cout << "psum1 = " << psum1 << std::endl;

                    throw std::runtime_error("psum_test error");
                }
            }
            else
            {
                throw std::runtime_error("psum_test is not supported");
            }
        };

        std::function<void(INTEGER_CONTAINER &, std::vector<uint64_t> &, uint64_t, int64_t)> reverse_psum_function = [&](INTEGER_CONTAINER &test_obj, std::vector<uint64_t> &correct_obj, [[maybe_unused]] uint64_t i, [[maybe_unused]] int64_t message_paragraph)
        {
            if constexpr (SUPPORT_PSUM)
            {
                std::mt19937 mt(_seed++);
                std::uniform_int_distribution<uint64_t> get_rand_value(0, UINT64_MAX);

                uint64_t pos = get_rand_value(mt) % (correct_obj.size());
                uint64_t psum1 = DynamicIntegerTest::compute_reverse_psum(correct_obj, pos);
                uint64_t psum2 = test_obj.reverse_psum(pos);
                if (psum1 != psum2)
                {
                    std::cout << "reverse_psum_test error" << std::endl;
                    std::cout << "psum1 = " << psum1 << std::endl;

                    throw std::runtime_error("reverse_psum_test error");
                }
            }
            else
            {
                throw std::runtime_error("reverse_psum_test is not supported");
            }
        };

        std::function<void(INTEGER_CONTAINER &, std::vector<uint64_t> &, uint64_t, int64_t)> search_function = [&](INTEGER_CONTAINER &test_obj, std::vector<uint64_t> &correct_obj, [[maybe_unused]] uint64_t i, [[maybe_unused]] int64_t message_paragraph)
        {
            if constexpr (SUPPORT_SEARCH)
            {
                std::mt19937 mt(_seed++);
                std::uniform_int_distribution<uint64_t> get_rand_value(0, UINT64_MAX);

                uint64_t value = get_rand_value(mt) % this->_max_value;
                uint64_t pos1 = DynamicIntegerTest::compute_search(correct_obj, value);
                uint64_t pos2 = test_obj.search(value);
                if (pos1 != pos2)
                {
                    std::cout << "search_test error" << std::endl;
                    std::cout << "pos1 = " << pos1 << std::endl;

                    throw std::runtime_error("search_test error");
                }
            }
            else
            {
                throw std::runtime_error("search_test is not supported");
            }
        };

        static uint64_t compute_psum(const std::vector<uint64_t> &bv, uint64_t i)
        {
            uint64_t sum = 0;

            if (i >= bv.size())
            {
                std::cout << "compute_psum error" << std::endl;
                std::cout << "i = " << i << std::endl;
                std::cout << "bv.size() = " << bv.size() << std::endl;
                throw std::runtime_error("compute_psum error");
            }

            for (uint64_t j = 0; j <= i; j++)
            {
                sum += bv[j];
            }
            return sum;
        }

        static uint64_t compute_reverse_psum(const std::vector<uint64_t> &bv, uint64_t i)
        {
            uint64_t sum = 0;

            if (i >= bv.size())
            {
                std::cout << "compute_psum error" << std::endl;
                std::cout << "i = " << i << std::endl;
                std::cout << "bv.size() = " << bv.size() << std::endl;
                throw std::runtime_error("compute_psum error");
            }

            for (uint64_t j = 0; j <= i; j++)
            {
                sum += bv[bv.size() - j - 1];
            }
            return sum;
        }

        static int64_t compute_search(const std::vector<uint64_t> &bv, uint64_t value)
        {
            uint64_t sum = 0;
            for (uint64_t j = 0; j < bv.size(); j++)
            {
                sum += bv[j];
                if (sum >= value)
                {
                    return j;
                }
            }
            return -1;
        }

        void build_test(uint64_t max_len, uint64_t max_value, uint64_t number_of_trials, uint64_t seed)
        {
            this->_max_value = max_value;
            this->_seed = seed;
            this->_inputs.clear();
            for (uint64_t len = 16; len < max_len; len *= 2)
            {
                for (uint64_t x = 0; x < number_of_trials; x++)
                {
                    this->_inputs.push_back(len);
                }
            }
            stool::BuildTest::build_test(this->_inputs.size(), builder_function, equal_check_function, stool::Message::SHOW_MESSAGE);
        }

        void reverse_psum_test(uint64_t max_len, uint64_t max_value, uint64_t number_of_trials, uint64_t seed)
        {
            this->_max_value = max_value;
            this->_seed = seed;
            this->_inputs.clear();
            for (uint64_t len = 16; len < max_len; len *= 2)
            {
                for (uint64_t x = 0; x < number_of_trials; x++)
                {
                    this->_inputs.push_back(len);
                }
            }
            stool::QueryTest::query_test("REVERSE_PSUM", this->_inputs.size(), number_of_trials, this->test_builder_function, this->correct_builder_function, this->reverse_psum_function, stool::Message::SHOW_MESSAGE);
        }
        void psum_test(uint64_t max_len, uint64_t max_value, uint64_t number_of_trials, uint64_t seed)
        {
            this->_max_value = max_value;
            this->_seed = seed;
            this->_inputs.clear();
            for (uint64_t len = 16; len < max_len; len *= 2)
            {
                for (uint64_t x = 0; x < number_of_trials; x++)
                {
                    this->_inputs.push_back(len);
                }
            }
            stool::QueryTest::query_test("PSUM", this->_inputs.size(), number_of_trials, this->test_builder_function, this->correct_builder_function, this->psum_function, stool::Message::SHOW_MESSAGE);
        }

        void search_test(uint64_t max_len, uint64_t max_value, uint64_t number_of_trials, uint64_t seed)
        {
            this->_max_value = max_value;
            this->_seed = seed;
            this->_inputs.clear();
            for (uint64_t len = 16; len < max_len; len *= 2)
            {
                for (uint64_t x = 0; x < number_of_trials; x++)
                {
                    this->_inputs.push_back(len);
                }
            }
            stool::QueryTest::query_test("SEARCH", this->_inputs.size(), number_of_trials, this->test_builder_function, this->correct_builder_function, this->search_function, stool::Message::SHOW_MESSAGE);
        }

        void insert_test(uint64_t max_len, uint64_t max_value, uint64_t number_of_trials, bool detail_check, uint64_t seed)
        {
            this->_max_value = max_value;
            this->_seed = seed;
            this->_inputs.clear();

            for (uint64_t len = 16; len < max_len; len *= 2)
            {
                for (uint64_t x = 0; x < number_of_trials; x++)
                {
                    _inputs.push_back(len);
                }
            }

            stool::UpdateTest::update_test<INTEGER_CONTAINER, std::vector<uint64_t>>("INSERT", _inputs.size(), number_of_trials,
                                                                                     this->test_builder_function, this->correct_builder_function, this->insert_function, this->equal_check_function2, detail_check);
        }

        void remove_test(uint64_t max_len, uint64_t max_value, uint64_t number_of_trials, bool detail_check, uint64_t seed)
        {
            this->_max_value = max_value;
            this->_seed = seed;
            this->_inputs.clear();

            for (uint64_t len = 16; len < max_len; len *= 2)
            {
                for (uint64_t x = 0; x < number_of_trials; x++)
                {
                    this->_inputs.push_back(len);
                }
            }

            stool::UpdateTest::update_test<INTEGER_CONTAINER, std::vector<uint64_t>>("REMOVE", _inputs.size(), number_of_trials,
                                                                                     this->test_builder_function, this->correct_builder_function, this->remove_function, this->equal_check_function2, detail_check);
        }

        void replace_test(uint64_t max_len, uint64_t max_value, uint64_t number_of_trials, bool detail_check, uint64_t seed)
        {
            this->_max_value = max_value;
            this->_seed = seed;
            this->_inputs.clear();

            for (uint64_t len = 16; len < max_len; len *= 2)
            {
                for (uint64_t x = 0; x < number_of_trials; x++)
                {
                    this->_inputs.push_back(len);
                }
            }

            stool::UpdateTest::update_test<INTEGER_CONTAINER, std::vector<uint64_t>>("REPLACE", _inputs.size(), number_of_trials,
                                                                                     this->test_builder_function, this->correct_builder_function, this->replace_function, this->equal_check_function2, detail_check);
        }

        void push_back_test(uint64_t max_len, uint64_t max_value, uint64_t number_of_trials, bool detail_check, uint64_t seed)
        {
            this->_max_value = max_value;
            this->_seed = seed;
            this->_inputs.clear();

            for (uint64_t len = 16; len < max_len; len *= 2)
            {
                for (uint64_t x = 0; x < number_of_trials; x++)
                {
                    this->_inputs.push_back(len);
                }
            }

            stool::UpdateTest::update_test<INTEGER_CONTAINER, std::vector<uint64_t>>("PUSH_BACK", _inputs.size(), number_of_trials,
                                                                                     this->test_builder_function, this->correct_builder_function, this->push_back_function, this->equal_check_function2, detail_check);
        }

        void pop_back_test(uint64_t max_len, uint64_t max_value, uint64_t number_of_trials, bool detail_check, uint64_t seed)
        {

            this->_max_value = max_value;
            this->_seed = seed;
            this->_inputs.clear();

            for (uint64_t len = 16; len < max_len; len *= 2)
            {
                for (uint64_t x = 0; x < number_of_trials; x++)
                {
                    this->_inputs.push_back(len);
                }
            }

            stool::UpdateTest::update_test<INTEGER_CONTAINER, std::vector<uint64_t>>("POP_BACK", _inputs.size(), number_of_trials,
                                                                                     this->test_builder_function, this->correct_builder_function, this->pop_back_function, this->equal_check_function2, detail_check);
        }

        void load_and_save_file_test(uint64_t max_element_count, uint64_t max_value, uint64_t trial_count, [[maybe_unused]] bool detailed_check, uint64_t seed, int message_paragraph = stool::Message::SHOW_MESSAGE)
        {
            this->_max_value = max_value;
            this->_seed = seed;
            this->_inputs.clear();

            for (uint64_t len = 16; len < max_element_count; len *= 2)
            {
                for (uint64_t x = 0; x < trial_count; x++)
                {
                    this->_inputs.push_back(len);
                }
            }

            stool::LoadAndSaveTest::load_and_save_file_test(this->_inputs.size(), builder_function, equal_check_function3, "flc_vector.bits", message_paragraph);
        }

        void load_and_save_bytes_test(uint64_t max_element_count, uint64_t max_value, uint64_t trial_count, [[maybe_unused]] bool detailed_check, uint64_t seed, int message_paragraph = stool::Message::SHOW_MESSAGE)
        {
            this->_max_value = max_value;
            this->_seed = seed;
            this->_inputs.clear();

            for (uint64_t len = 16; len < max_element_count; len *= 2)
            {
                for (uint64_t x = 0; x < trial_count; x++)
                {
                    this->_inputs.push_back(len);
                }
            }

            stool::LoadAndSaveTest::load_and_save_bytes_test(this->_inputs.size(), builder_function, equal_check_function3, message_paragraph);
        }

        template <bool USE_QUERY = true>
        void random_test(uint64_t max_len, uint64_t max_value, uint64_t number_of_trials, uint64_t max_counter, bool detail_check, uint64_t seed)
        {
            this->_max_value = max_value;
            this->_seed = seed;
            this->_inputs.clear();

            std::cout << "RANDOM_TEST: \t" << std::flush;
            std::mt19937 mt(seed);
            std::uniform_int_distribution<uint64_t> get_rand_value(0, UINT64_MAX);

            for (uint64_t i = 0; i < number_of_trials; i++)
            {
                uint64_t counter = 0;
                std::vector<uint64_t> naive_values = stool::StringGenerator::create_random_integer_sequence(max_len / 2, max_value, seed++);
                INTEGER_CONTAINER test_container = INTEGER_CONTAINER::build(naive_values);

                std::cout << "+" << std::flush;

                while (counter < max_counter)
                {

                    uint64_t type = get_rand_value(mt) % 10;

                    if (type == 0 || type == 1)
                    {
                        this->push_back_function(test_container, naive_values, i, stool::Message::SHOW_MESSAGE);
                    }
                    else if (type == 2 || type == 3)
                    {
                        this->pop_back_function(test_container, naive_values, i, stool::Message::SHOW_MESSAGE);
                    }
                    else if (type == 4)
                    {
                        this->insert_function(test_container, naive_values, i, stool::Message::SHOW_MESSAGE);
                    }
                    else if (type == 5)
                    {
                        this->remove_function(test_container, naive_values, i, stool::Message::SHOW_MESSAGE);
                    }
                    else if (type == 8)
                    {
                        this->replace_function(test_container, naive_values, i, stool::Message::SHOW_MESSAGE);
                    }
                    else
                    {
                        if constexpr (USE_QUERY)
                        {
                            this->psum_function(test_container, naive_values, i, stool::Message::SHOW_MESSAGE);
                            this->search_function(test_container, naive_values, i, stool::Message::SHOW_MESSAGE);
                            this->reverse_psum_function(test_container, naive_values, i, stool::Message::SHOW_MESSAGE);
                        }
                    }
                    counter++;

                    if (detail_check)
                    {
                        try
                        {
                            std::vector<uint64_t> test_values = test_container.to_vector();
                            stool::EqualChecker::equal_check(naive_values, test_values, "RANDOM_TEST");
                        }
                        catch (const std::runtime_error &e)
                        {
                            std::cout << "random_test error" << std::endl;
                            std::cout << "type = " << type << std::endl;
                            throw e;
                        }
                    }
                }
                std::vector<uint64_t> test_values = test_container.to_vector();
                stool::EqualChecker::equal_check(naive_values, test_values, "RANDOM_TEST");
            }
            std::cout << "[DONE]" << std::endl;
        }
    };
}
