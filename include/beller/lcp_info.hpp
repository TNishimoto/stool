#pragma once

namespace stool
{
    namespace beller
    {
        struct LCPInfo
        {
            uint64_t lcp;
            uint64_t position;
            LCPInfo()
            {
                this->lcp = UINT64_MAX;
                this->position = UINT64_MAX;
            }
            LCPInfo(uint64_t _lcp, uint64_t _position) : lcp(_lcp), position(_position) {}
        };
    } // namespace beller
} // namespace stool