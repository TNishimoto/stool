#include <vector>
#include <cstdint>
#include <algorithm>
#include <type_traits>
#include <limits>

// SA-IS for std::vector<C>
// - Input : std::vector<C> text
// - Output: std::vector<uint64_t> suffix array
//
// Notes:
// * This implementation assumes that the alphabet symbols in `text` are
//   non-negative integer-like values convertible to uint64_t.
// * The returned suffix array is for the original text (without an exposed
//   sentinel). Internally, SA-IS appends a unique sentinel 0 that is smaller
//   than every other symbol.
// * The result size is text.size(), i.e. the suffix array of suffixes
//   text[0..], text[1..], ..., text[n-1..].
//
// Example:
//   std::vector<char> s = {'b','a','n','a','n','a'};
//   auto sa = sais_suffix_array(s);

namespace sais_detail {

using index_type = uint64_t;

template <class Int>
static std::vector<index_type> build_sa_int(const std::vector<Int>& s, index_type upper) {
    const index_type n = static_cast<index_type>(s.size());
    std::vector<index_type> sa(n, std::numeric_limits<index_type>::max());

    if (n == 0) return sa;
    if (n == 1) {
        sa[0] = 0;
        return sa;
    }
    if (n == 2) {
        if (s[0] < s[1]) {
            sa[0] = 0;
            sa[1] = 1;
        } else {
            sa[0] = 1;
            sa[1] = 0;
        }
        return sa;
    }

    // ls[i] == true  <=> S-type
    // ls[i] == false <=> L-type
    std::vector<bool> ls(n);
    ls[n - 1] = true; // sentinel is S-type
    for (index_type i = n - 1; i > 0; --i) {
        if (s[i - 1] == s[i]) {
            ls[i - 1] = ls[i];
        } else {
            ls[i - 1] = (s[i - 1] < s[i]);
        }
    }

    std::vector<index_type> sum_l(upper + 1, 0), sum_s(upper + 1, 0);
    for (index_type i = 0; i < n; ++i) {
        if (!ls[i]) {
            ++sum_s[s[i]];
        } else {
            ++sum_l[s[i] + 1];
        }
    }
    for (index_type i = 0; i <= upper; ++i) {
        sum_s[i] += sum_l[i];
        if (i < upper) sum_l[i + 1] += sum_s[i];
    }

    auto induce = [&](const std::vector<index_type>& lms) {
        std::fill(sa.begin(), sa.end(), std::numeric_limits<index_type>::max());

        std::vector<index_type> buf(upper + 1);

        // Place LMS suffixes at bucket ends.
        std::copy(sum_s.begin(), sum_s.end(), buf.begin());
        for (index_type d : lms) {
            if (d == n) continue;
            sa[buf[s[d]]++] = d;
        }

        // Place sentinel suffix.
        std::copy(sum_l.begin(), sum_l.end(), buf.begin());
        sa[buf[s[n - 1]]++] = n - 1;

        // Induce L-type suffixes from left to right.
        for (index_type i = 0; i < n; ++i) {
            index_type v = sa[i];
            if (v == std::numeric_limits<index_type>::max() || v == 0) continue;
            --v;
            if (!ls[v]) {
                sa[buf[s[v]]++] = v;
            }
        }

        // Induce S-type suffixes from right to left.
        std::copy(sum_l.begin(), sum_l.end(), buf.begin());
        for (index_type i = n; i > 0; --i) {
            index_type v = sa[i - 1];
            if (v == std::numeric_limits<index_type>::max() || v == 0) continue;
            --v;
            if (ls[v]) {
                sa[--buf[s[v] + 1]] = v;
            }
        }
    };

    // Collect LMS positions.
    std::vector<index_type> lms_map(n + 1, std::numeric_limits<index_type>::max());
    index_type m = 0;
    for (index_type i = 1; i < n; ++i) {
        if (!ls[i - 1] && ls[i]) {
            lms_map[i] = m++;
        }
    }

    std::vector<index_type> lms;
    lms.reserve(m);
    for (index_type i = 1; i < n; ++i) {
        if (!ls[i - 1] && ls[i]) lms.push_back(i);
    }

    induce(lms);

    if (m > 0) {
        std::vector<index_type> sorted_lms;
        sorted_lms.reserve(m);
        for (index_type v : sa) {
            if (v != std::numeric_limits<index_type>::max() && lms_map[v] != std::numeric_limits<index_type>::max()) {
                sorted_lms.push_back(v);
            }
        }

        std::vector<index_type> rec_s(m);
        index_type rec_upper = 0;
        rec_s[lms_map[sorted_lms[0]]] = 0;

        for (index_type i = 1; i < m; ++i) {
            index_type l = sorted_lms[i - 1];
            index_type r = sorted_lms[i];

            index_type end_l = (lms_map[l] + 1 < m) ? lms[lms_map[l] + 1] : n;
            index_type end_r = (lms_map[r] + 1 < m) ? lms[lms_map[r] + 1] : n;

            bool same = true;
            if ((end_l - l) != (end_r - r)) {
                same = false;
            } else {
                while (l < end_l) {
                    if (s[l] != s[r] || ls[l] != ls[r]) {
                        same = false;
                        break;
                    }
                    ++l;
                    ++r;
                }
            }

            if (!same) ++rec_upper;
            rec_s[lms_map[sorted_lms[i]]] = rec_upper;
        }

        std::vector<index_type> rec_sa;
        if (rec_upper + 1 == m) {
            rec_sa.resize(m);
            for (index_type i = 0; i < m; ++i) {
                rec_sa[rec_s[i]] = i;
            }
        } else {
            rec_sa = build_sa_int(rec_s, rec_upper);
        }

        for (index_type i = 0; i < m; ++i) {
            sorted_lms[i] = lms[rec_sa[i]];
        }
        induce(sorted_lms);
    }

    return sa;
}

} // namespace sais_detail


namespace stool {
    template <class C>
    std::vector<uint64_t> sais_suffix_array(const std::vector<C>& text) {
        using sais_detail::index_type;
    
        static_assert(std::is_integral<C>::value || std::is_enum<C>::value,
                      "C must be an integral or enum-like character type.");
    
        const index_type n = static_cast<index_type>(text.size());
        std::vector<uint64_t> result;
    
        if (n == 0) return result;
    
        // Coordinate compression:
        // internal string = [rank(text[i]) + 1] + sentinel(0)
        // so that 0 is a unique smallest symbol.
        std::vector<uint64_t> vals;
        vals.reserve(n);
        for (const auto& ch : text) {
            vals.push_back(static_cast<uint64_t>(ch));
        }
    
        std::vector<uint64_t> ord = vals;
        std::sort(ord.begin(), ord.end());
        ord.erase(std::unique(ord.begin(), ord.end()), ord.end());
    
        std::vector<uint64_t> s(n + 1);
        for (index_type i = 0; i < n; ++i) {
            s[i] = static_cast<uint64_t>(
                std::lower_bound(ord.begin(), ord.end(), vals[i]) - ord.begin()
            ) + 1;
        }
        s[n] = 0; // sentinel
    
        auto sa_all = sais_detail::build_sa_int(s, static_cast<uint64_t>(ord.size()));
    
        // Remove the sentinel position.
        result.reserve(n);
        for (uint64_t p : sa_all) {
            if (p != n) result.push_back(p);
        }
        return result;
    }

    template <class C>
    std::vector<uint64_t> sais_suffix_array_for_signed_characters(const std::vector<C>& text) {
        using sais_detail::index_type;

        static_assert(std::is_integral<C>::value || std::is_enum<C>::value,
                      "C must be an integral or enum-like character type.");

        const index_type n = static_cast<index_type>(text.size());
        std::vector<uint64_t> result;

        if (n == 0) return result;

        // Coordinate compression using C's own ordering.
        // This preserves negative-value order for signed integer-like alphabets.
        std::vector<C> ord = text;
        std::sort(ord.begin(), ord.end());
        ord.erase(std::unique(ord.begin(), ord.end()), ord.end());

        std::vector<uint64_t> s(n + 1);
        for (index_type i = 0; i < n; ++i) {
            s[i] = static_cast<uint64_t>(
                std::lower_bound(ord.begin(), ord.end(), text[i]) - ord.begin()
            ) + 1;
        }
        s[n] = 0; // sentinel

        auto sa_all = sais_detail::build_sa_int(s, static_cast<uint64_t>(ord.size()));

        // Remove the sentinel position.
        result.reserve(n);
        for (uint64_t p : sa_all) {
            if (p != n) result.push_back(p);
        }
        return result;
    }
    
}

