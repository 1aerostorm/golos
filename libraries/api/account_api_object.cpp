#include <golos/api/account_api_object.hpp>

namespace golos { namespace api {

using golos::chain::by_account;
using golos::chain::by_account_bandwidth_type;
using golos::chain::account_bandwidth_object;
using golos::chain::account_authority_object;
using golos::chain::account_metadata_object;
using golos::chain::bandwidth_type;

account_api_object::account_api_object(const account_object& a, const golos::chain::database& db)
    :   id(a.id), name(a.name), memo_key(a.memo_key), proxy(a.proxy),
        last_account_update(a.last_account_update), created(a.created), mined(a.mined),
        owner_challenged(a.owner_challenged), active_challenged(a.active_challenged),
        last_owner_proved(a.last_owner_proved), last_active_proved(a.last_active_proved),
        recovery_account(a.recovery_account), reset_account(a.reset_account),
        last_account_recovery(a.last_account_recovery), comment_count(a.comment_count),
        lifetime_vote_count(a.lifetime_vote_count), post_count(a.post_count),
        posts_capacity(a.posts_capacity), comments_capacity(a.comments_capacity), voting_capacity(a.voting_capacity),
        can_vote(a.can_vote),
        voting_power(a.voting_power), last_vote_time(a.last_vote_time),
        balance(a.balance), savings_balance(a.savings_balance), accumulative_balance(a.accumulative_balance), tip_balance(a.tip_balance),
        sbd_balance(a.sbd_balance), sbd_seconds(a.sbd_seconds),
        sbd_seconds_last_update(a.sbd_seconds_last_update),
        sbd_last_interest_payment(a.sbd_last_interest_payment),
        savings_sbd_balance(a.savings_sbd_balance), savings_sbd_seconds(a.savings_sbd_seconds),
        savings_sbd_seconds_last_update(a.savings_sbd_seconds_last_update),
        savings_sbd_last_interest_payment(a.savings_sbd_last_interest_payment),
        savings_withdraw_requests(a.savings_withdraw_requests),
        benefaction_rewards(a.benefaction_rewards), curation_rewards(a.curation_rewards),
        delegation_rewards(a.delegation_rewards), posting_rewards(a.posting_rewards),
        vesting_shares(a.vesting_shares),
        delegated_vesting_shares(a.delegated_vesting_shares), received_vesting_shares(a.received_vesting_shares),
        vesting_withdraw_rate(a.vesting_withdraw_rate), next_vesting_withdrawal(a.next_vesting_withdrawal),
        withdrawn(a.withdrawn), to_withdraw(a.to_withdraw), withdraw_routes(a.withdraw_routes),
        witnesses_voted_for(a.witnesses_voted_for),
        last_comment(a.last_comment), last_post(a.last_post) {
    size_t n = a.proxied_vsf_votes.size();
    proxied_vsf_votes.reserve(n);
    for (size_t i = 0; i < n; i++) {
        proxied_vsf_votes.push_back(a.proxied_vsf_votes[i]);
    }

    const auto& auth = db.get_authority(name);
    owner = authority(auth.owner);
    active = authority(auth.active);
    posting = authority(auth.posting);
    last_owner_update = auth.last_owner_update;

    auto meta = db.find<account_metadata_object, by_account>(name);
    if (meta != nullptr) {
        json_metadata = golos::chain::to_string(meta->json_metadata);
    }

    auto post = db.find<account_bandwidth_object, by_account_bandwidth_type>(std::make_tuple(name, bandwidth_type::post));
    if (post != nullptr) {
        post_bandwidth = post->average_bandwidth;
    }

    auto forum = db.find<account_bandwidth_object, by_account_bandwidth_type>(std::make_tuple(name, bandwidth_type::forum));
    if (forum != nullptr) {
        average_bandwidth = forum->average_bandwidth;
        lifetime_bandwidth = forum->lifetime_bandwidth;
        last_bandwidth_update = forum->last_bandwidth_update;
    }

    auto market = db.find<account_bandwidth_object, by_account_bandwidth_type>(std::make_tuple(name, bandwidth_type::market));
    if (market != nullptr) {
        average_market_bandwidth = market->average_bandwidth;
        lifetime_market_bandwidth = market->lifetime_bandwidth;
        last_market_bandwidth_update = market->last_bandwidth_update;
    }

    auto custom_json = db.find<account_bandwidth_object, by_account_bandwidth_type>(std::make_tuple(name, bandwidth_type::custom_json));
    if (custom_json != nullptr) {
        average_custom_json_bandwidth = custom_json->average_bandwidth;
        lifetime_custom_json_bandwidth = custom_json->lifetime_bandwidth;
        last_custom_json_bandwidth_update = custom_json->last_bandwidth_update;
    }

    if (db.head_block_time() < a.referral_end_date) {
        referrer_account = a.referrer_account;
        referrer_interest_rate = a.referrer_interest_rate;
        referral_end_date = a.referral_end_date;
        referral_break_fee = a.referral_break_fee;
    }

    last_active_operation = a.last_active_operation;
    last_claim = a.last_claim;

    if (db.has_hardfork(STEEMIT_HARDFORK_0_23__83)) {
        auto now = db.head_block_time().sec_since_epoch();
        auto now_block = db.head_block_num();

        auto expire_block = now_block;
        auto expire = last_claim.sec_since_epoch() + db.get_witness_schedule_object().median_props.claim_idleness_time;
        if (expire > now) {
            expire_block += ((expire - now) / STEEMIT_BLOCK_INTERVAL);
        }

        auto interval = GOLOS_CLAIM_IDLENESS_CHECK_INTERVAL;
        expire_block += (interval - expire_block % interval);

        expire = now + (expire_block - now_block) * STEEMIT_BLOCK_INTERVAL;
        claim_expiration = time_point_sec(expire);
    }
}

account_api_object::account_api_object() = default;

} } // golos::api
