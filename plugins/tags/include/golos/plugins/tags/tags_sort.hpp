#pragma once

#include <golos/api/discussion.hpp>

namespace golos { namespace plugins { namespace tags { namespace sort {
    using golos::api::discussion;
    using golos::api::comment_object;
    using golos::chain::account_object;
    using protocol::asset;
    using protocol::share_type;
    using protocol::authority;
    using protocol::account_name_type;
    using protocol::public_key_type;
    
    struct tags_sort {
        bool reversed;
        tags_sort(bool r = false) : reversed(r) {}

        virtual bool compare(const discussion& first, const discussion& second) const = 0;

        bool operator()(const discussion& first, const discussion& second) const {
            bool res = compare(first, second);
            return reversed ? (!res) : res;
        }
    };

    struct by_trending : tags_sort {
        by_trending(bool r = false) : tags_sort(r) {}

        bool compare(const discussion& first, const discussion& second) const {
            if (std::greater<double>()(first.trending, second.trending)) {
                return true;
            } else if (std::greater<double>()(second.trending, first.trending)) {
                return false;
            }
            return std::less<comment_object::id_type>()(first.id, second.id);
        }
    };

    struct by_promoted : tags_sort {
        by_promoted(bool r = false) : tags_sort(r) {}

        bool compare(const discussion& first, const discussion& second) const {
            if (!first.promoted) {
                return false;
            }
            if (std::greater<share_type>()(first.promoted->amount, second.promoted->amount)) {
                return true;
            } else if (std::equal_to<share_type>()(first.promoted->amount, second.promoted->amount)) {
                return std::less<comment_object::id_type>()(first.id, second.id);
            }
            return false;
        }
    };

    struct by_created : tags_sort {
        by_created(bool r = false) : tags_sort(r) {}

        bool compare(const discussion& first, const discussion& second) const {
            if (std::greater<time_point_sec>()(first.created, second.created)) {
                return true;
            } else if (std::equal_to<time_point_sec>()(first.created, second.created)) {
                return std::less<comment_object::id_type>()(first.id, second.id);
            }
            return false;
        }
    };

    struct by_active : tags_sort {
        by_active(bool r = false) : tags_sort(r) {}

        bool compare(const discussion& first, const discussion& second) const {
            if (!first.active.valid() || !second.active.valid()) {
                return false;
            }
            if (std::greater<time_point_sec>()(*first.active, *second.active)) {
                return true;
            } else if (std::equal_to<time_point_sec>()(*first.active, *second.active)) {
                return std::less<comment_object::id_type>()(first.id, second.id);
            }
            return false;
        }
    };

    struct by_updated : tags_sort {
        by_updated(bool r = false) : tags_sort(r) {}

        bool compare(const discussion& first, const discussion& second) const {
            if (!first.last_update.valid() || !second.last_update.valid()) {
                return false;
            }
            if (std::greater<time_point_sec>()(*first.last_update, *second.last_update)) {
                return true;
            } else if (std::equal_to<time_point_sec>()(*first.last_update, *second.last_update)) {
                return std::less<comment_object::id_type>()(first.id, second.id);
            }
            return false;
        }
    };

    struct by_cashout : tags_sort {
        by_cashout(bool r = false) : tags_sort(r) {}

        bool compare(const discussion& first, const discussion& second) const {
            if (std::less<time_point_sec>()(first.cashout_time, second.cashout_time)) {
                return true;
            } else if (std::equal_to<time_point_sec>()(first.cashout_time, second.cashout_time)) {
                return std::less<comment_object::id_type>()(first.id, second.id);
            }
            return false;
        }
    };

    struct by_net_rshares : tags_sort {
        by_net_rshares(bool r = false) : tags_sort(r) {}

        bool compare(const discussion& first, const discussion& second) const {
            if (std::greater<share_type>()(first.net_rshares, second.net_rshares)) {
                return true;
            } else if (std::equal_to<share_type>()(first.net_rshares, second.net_rshares)) {
                return std::less<comment_object::id_type>()(first.id, second.id);
            }
            return false;
        }
    };

    struct by_net_votes : tags_sort {
        by_net_votes(bool r = false) : tags_sort(r) {}

        bool compare(const discussion& first, const discussion& second) const {
            if (std::greater<int32_t>()(first.net_votes, second.net_votes)) {
                return true;
            } else if (std::equal_to<int32_t>()(first.net_votes, second.net_votes)) {
                return std::less<comment_object::id_type>()(first.id, second.id);
            }
            return false;
        }
    };

    struct by_children : tags_sort {
        by_children(bool r = false) : tags_sort(r) {}

        bool compare(const discussion& first, const discussion& second) const {
            if (std::greater<int32_t>()(first.children, second.children)) {
                return true;
            } else if (std::equal_to<int32_t>()(first.children, second.children)) {
                return std::greater<comment_object::id_type>()(first.id, second.id);
            }
            return false;
        }
    };

    struct by_hot : tags_sort {
        by_hot(bool r = false) : tags_sort(r) {}

        bool compare(const discussion& first, const discussion& second) const {
            if (std::greater<double>()(first.hot, second.hot)) {
                return true;
            } else if (std::greater<double>()(second.hot, first.hot)) {
                return false;
            }
            return std::less<comment_object::id_type>()(first.id, second.id);
        }
    };

    struct by_donates : tags_sort {
        by_donates(bool r = false) : tags_sort(r) {}

        bool compare(const discussion& first, const discussion& second) const {
            if (std::greater<asset>()(first.donates, second.donates)) {
                return true;
            } else if (std::equal_to<asset>()(first.donates, second.donates)) {
                return std::greater<comment_object::id_type>()(first.id, second.id);
            }
            return false;
        }
    };

} } } }  // golos::plugins::tags::sort
