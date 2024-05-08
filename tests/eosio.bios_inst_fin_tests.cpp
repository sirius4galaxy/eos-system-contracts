#include <boost/test/unit_test.hpp>
#include <eosio/testing/tester.hpp>
#include <eosio/chain/abi_serializer.hpp>

#include <fc/variant_object.hpp>
#include <fc/io/json.hpp>

#include "contracts.hpp"

using namespace eosio::testing;
using namespace eosio;
using namespace eosio::chain;
using namespace eosio::testing;
using namespace fc;

using mvo = fc::mutable_variant_object;

// tests for instant finality actions
class eosio_bios_if_tester : public tester {
public:

   eosio_bios_if_tester() {
      create_accounts( { "iftester"_n } );
      produce_block();

      // instant finality actions are privileged
      base_tester::push_action(config::system_account_name, "setpriv"_n,
                               config::system_account_name,  mutable_variant_object()
                               ("account", "iftester")
                               ("is_priv", 1)
      );

      set_code( "iftester"_n, contracts::bios_wasm() );
      set_abi( "iftester"_n, contracts::bios_abi().data() );
      produce_block();
   }
};

BOOST_AUTO_TEST_SUITE(eosio_bios_if_tests)

// one finalizer in finalizer policy
BOOST_FIXTURE_TEST_CASE( set_1_finalizer, eosio_bios_if_tester ) try {
   // public_key and pop are generated by spring-util
   push_action("iftester"_n, "setfinalizer"_n, "iftester"_n, mvo()
      ("finalizer_policy", mvo()
         ("threshold", 2)
         ("finalizers", std::vector<mvo>{
            mvo()
               ("description", "set_1_finalizer")
               ("weight", 2)
               ("public_key", "PUB_BLS_6j4Y3LfsRiBxY-DgvqrZNMCttHftBQPIWwDiN2CMhHWULjN1nGwM1O_nEEJefqwAG4X09n4Kdt4a1mfZ1ES1cLGjQo6uLLSloiVW4i9BUhMHU2nVujP1_U_9ihdI3egZ17N-iA")
               ("pop", "SIG_BLS_N5r73_i50OVkydasCVVBOqqAqM4XQo_-DHgNawK77bcf06Bx0_rh5TNn9iZewNMZ6ecyEjs_sEkwjAXplhqyqf7S9FqSt8mfRxO7pE3bUZS0Z-Fxitsh9X0l_-kj3Z8VD8IwsaUwBLacudzShIXA-5E47cEqYoV3bGhANerKuDhZ4Pesm2xotAScK0pcNp0LbTNj0MZpVr0u6kJh169IoeG4ngCvD6uE2EicNrzyvDhu0u925Q1cm5z_bVha-DsANq3zcA")})));

    signed_block_ptr cur_block = produce_block();
    fc::variant pretty_output;
    abi_serializer::to_variant( *cur_block, pretty_output, get_resolver(), fc::microseconds::maximum() );

    BOOST_REQUIRE(pretty_output.get_object().contains("instant_finality_extension"));
    std::string output_json = fc::json::to_pretty_string(pretty_output);
    BOOST_TEST(output_json.find("\"generation\": 1") != std::string::npos);
    BOOST_TEST(output_json.find("\"threshold\": 2") != std::string::npos);
    BOOST_TEST(output_json.find("set_1_finalizer") != std::string::npos);
    BOOST_TEST(output_json.find("PUB_BLS_6j4Y3LfsRiBxY-DgvqrZNMCttHftBQPIWwDiN2CMhHWULjN1nGwM1O_nEEJefqwAG4X09n4Kdt4a1mfZ1ES1cLGjQo6uLLSloiVW4i9BUhMHU2nVujP1_U_9ihdI3egZ17N-iA") != std::string::npos);
} FC_LOG_AND_RETHROW()

// two finalizers in finalizer policy
BOOST_FIXTURE_TEST_CASE( set_2_finalizers, eosio_bios_if_tester ) try {
   // public_key and pop are generated by spring-util
   push_action("iftester"_n, "setfinalizer"_n, "iftester"_n, mvo()
      ("finalizer_policy", mvo()
         ("threshold", 5)
         ("finalizers", std::vector<mvo>{
            mvo()
               ("description", "set_2_finalizer_1")
               ("weight", 3)
               ("public_key", "PUB_BLS_gtaOjOTa0NzDt8etBDqLoZKlfKTpTalcdfmbTJknLUJB2Fu4Cv-uoa8unF3bJ5kFewaCzf3tjYUyNE6CDSrwvYP5Nw47Y9oE9x4nqJWfJykMOoaI0kJz-GDrGN2nZdUAp5tWEg")
               ("pop", "SIG_BLS_9e1SzM60bWLdxwz4lYNQMNzMGeFuzFgJDYy7WykmynRVRQeIx2O2xnyzwv1WXvgYHLyMYZ4wK0Y_kU6jl330WazkBsw-_GzvIOGy8fnBnt5AyMaj9X5bhDbvB5MZc0QQz4-P2Z4SltTY17ZItGeekkjX_fgQ9kegM4qnuGU-2iqFj5i3Qf322L77b2SHjFoLmxdFOsfGpz7LyImSP8GcZH39W30cj5bmxfsp_90tGdAkz-7DG9nhSHYxFq6qTqMGijVPGg"),
            mvo()
               ("description", "set_2_finalizer_2")
               ("weight", 5)
               ("public_key", "PUB_BLS_kV0d54mbPRbd65t4ttv_-CxNt8ktKmf8q4uKZzNTzFSHDSj5rLlP_hdovTsHAPQOAyyzJ4bRTheKjSUj-IoTW96v3VdlifgtDbSVmg4JZR8H_tlStQSWsTHGo8pTX8cR_HEVoA")
               ("pop", "SIG_BLS_DsTwQvYa4uP51putCpLiZlJDyCL24l7bVu1kznc4X8GIYQdElYaCO88RFfCvncYFKwJfKZb-LNMW4GHZzhnyWu0Gp-ougZhSIq4mi1FrtM39uT6KP00-fYYPLOw5nzoLOl__f72qEkBluaaXKIiboWfQ-VnRdzvuE-8Y_m36u6d7bThWy1Lz27mUeelhLawEaKIW688HpFAQDEuvvKfAZYvsyPYXod6a_2KYS4rQkyAtjJXBWk4W0cuPU5n9lNYGyuzF4Q")
            })));

    signed_block_ptr cur_block = produce_block();
    fc::variant pretty_output;
    abi_serializer::to_variant( *cur_block, pretty_output, get_resolver(), fc::microseconds::maximum() );

    BOOST_REQUIRE(pretty_output.get_object().contains("instant_finality_extension"));
    std::string output_json = fc::json::to_pretty_string(pretty_output);
    BOOST_TEST(output_json.find("\"generation\": 1") != std::string::npos);
    BOOST_TEST(output_json.find("\"threshold\": 5") != std::string::npos);
    BOOST_TEST(output_json.find("set_2_finalizer_2") != std::string::npos);
    BOOST_TEST(output_json.find("PUB_BLS_kV0d54mbPRbd65t4ttv_-CxNt8ktKmf8q4uKZzNTzFSHDSj5rLlP_hdovTsHAPQOAyyzJ4bRTheKjSUj-IoTW96v3VdlifgtDbSVmg4JZR8H_tlStQSWsTHGo8pTX8cR_HEVoA") != std::string::npos);
} FC_LOG_AND_RETHROW()

// finalizer cannot be empty
BOOST_FIXTURE_TEST_CASE( empty_finalizers, eosio_bios_if_tester ) try {
   BOOST_REQUIRE_THROW(push_action("iftester"_n, "setfinalizer"_n, "iftester"_n, mvo()
      ("finalizer_policy", mvo()("threshold", 2)
         ("finalizers", std::vector<mvo>{}))), eosio_assert_message_exception);
} FC_LOG_AND_RETHROW()

// public_key must start with PUB_BLS
BOOST_FIXTURE_TEST_CASE( public_key_not_started_with_PUB_BLS, eosio_bios_if_tester ) try {
   BOOST_REQUIRE_THROW(push_action("iftester"_n, "setfinalizer"_n, "iftester"_n, mvo()
      ("finalizer_policy", mvo()("threshold", 2)
         ("finalizers", std::vector<mvo>{mvo()
            ("description", "set_1_finalizer")
            ("weight", 2)
            // PUB_BLS to UB_BLS
            ("public_key", "UB_BLS_lkixuIBmY1Xuc0GFnek0iDXitKAAQt87CL7Q1jZsjLid-9cruKfj_KjJk2P4GxsL7HnArD6F0lNAoxsLHISpsBBHqi-ET5JAYfKkS5lgG4To1vP48x6TtOPOslsr5D0YqKbtng")
            ("pop", "SIG_BLS_ubO3gsBLRuXg1UWLvNg54B4Xr2c1wKmwIVUzAxKOCAVKmOHabYdm222v3Go8bPkU7R9sRq2qYkhJQGh3wLbtcIhc2D6CPMpRIUB6mIcX18X2l-3ceXWdWfQBFWs1sUoXfp2UpmeMIPVcDKqgKWnJGyYyei-ekE86XnWaA8bBMzxAcJpEml4E--PRe48m27gVKqaTb6oY27oZHiHbjocZi1TU0RsDNGJAjaIapZkBKRUStQymTE6mmgdgLRHlR4EFjsiRUA")}))), eosio_assert_message_exception);
} FC_LOG_AND_RETHROW()

// pop signature must start with SIG_BLS
BOOST_FIXTURE_TEST_CASE( signature_not_started_with_SIG_BLS, eosio_bios_if_tester ) try {
   BOOST_REQUIRE_THROW(push_action("iftester"_n, "setfinalizer"_n, "iftester"_n, mvo()
      ("finalizer_policy", mvo()("threshold", 2)
         ("finalizers", std::vector<mvo>{mvo()
            ("description", "set_1_finalizer")
            ("weight", 2)
            ("public_key", "PUB_BLS_lkixuIBmY1Xuc0GFnek0iDXitKAAQt87CL7Q1jZsjLid-9cruKfj_KjJk2P4GxsL7HnArD6F0lNAoxsLHISpsBBHqi-ET5JAYfKkS5lgG4To1vP48x6TtOPOslsr5D0YqKbtng")
            // SIG_BLS changed to SIG_BL
            ("pop", "SIG_BL_ubO3gsBLRuXg1UWLvNg54B4Xr2c1wKmwIVUzAxKOCAVKmOHabYdm222v3Go8bPkU7R9sRq2qYkhJQGh3wLbtcIhc2D6CPMpRIUB6mIcX18X2l-3ceXWdWfQBFWs1sUoXfp2UpmeMIPVcDKqgKWnJGyYyei-ekE86XnWaA8bBMzxAcJpEml4E--PRe48m27gVKqaTb6oY27oZHiHbjocZi1TU0RsDNGJAjaIapZkBKRUStQymTE6mmgdgLRHlR4EFjsiRUA")}))), eosio_assert_message_exception);
} FC_LOG_AND_RETHROW()

// test to verify POP verification failure
BOOST_FIXTURE_TEST_CASE( pop_failed, eosio_bios_if_tester ) try {
   BOOST_REQUIRE_THROW(push_action("iftester"_n, "setfinalizer"_n, "iftester"_n, mvo()
      ("finalizer_policy", mvo()("threshold", 2)
         ("finalizers", std::vector<mvo>{mvo()
            ("description", "set_1_finalizer")
            ("weight", 2)
            ("public_key", "PUB_BLS_lkixuIBmY1Xuc0GFnek0iDXitKAAQt87CL7Q1jZsjLid-9cruKfj_KjJk2P4GxsL7HnArD6F0lNAoxsLHISpsBBHqi-ET5JAYfKkS5lgG4To1vP48x6TtOPOslsr5D0YqKbtng")
            // SIG_BLS_u changed to SIG_BLS_v
            ("pop", "SIG_BLS_vbO3gsBLRuXg1UWLvNg54B4Xr2c1wKmwIVUzAxKOCAVKmOHabYdm222v3Go8bPkU7R9sRq2qYkhJQGh3wLbtcIhc2D6CPMpRIUB6mIcX18X2l-3ceXWdWfQBFWs1sUoXfp2UpmeMIPVcDKqgKWnJGyYyei-ekE86XnWaA8bBMzxAcJpEml4E--PRe48m27gVKqaTb6oY27oZHiHbjocZi1TU0RsDNGJAjaIapZkBKRUStQymTE6mmgdgLRHlR4EFjsiRUA")}))), eosio_assert_message_exception);
} FC_LOG_AND_RETHROW()

// Verifies threshold must be greater than half the sum of the weights
BOOST_FIXTURE_TEST_CASE( threshold_equal_to_half_weights, eosio_bios_if_tester ) try {
   BOOST_REQUIRE_THROW(push_action("iftester"_n, "setfinalizer"_n, "iftester"_n, mvo()
      ("finalizer_policy", mvo()
         ("threshold", 5)
         ("finalizers", std::vector<mvo>{
            mvo()
               ("description", "set_2_finalizer_1")
               ("weight", 5)
               ("public_key", "PUB_BLS_kV0d54mbPRbd65t4ttv_-CxNt8ktKmf8q4uKZzNTzFSHDSj5rLlP_hdovTsHAPQOAyyzJ4bRTheKjSUj-IoTW96v3VdlifgtDbSVmg4JZR8H_tlStQSWsTHGo8pTX8cR_HEVoA")
               ("pop", "SIG_BLS_DsTwQvYa4uP51putCpLiZlJDyCL24l7bVu1kznc4X8GIYQdElYaCO88RFfCvncYFKwJfKZb-LNMW4GHZzhnyWu0Gp-ougZhSIq4mi1FrtM39uT6KP00-fYYPLOw5nzoLOl__f72qEkBluaaXKIiboWfQ-VnRdzvuE-8Y_m36u6d7bThWy1Lz27mUeelhLawEaKIW688HpFAQDEuvvKfAZYvsyPYXod6a_2KYS4rQkyAtjJXBWk4W0cuPU5n9lNYGyuzF4Q"),
            mvo()
               ("description", "set_2_finalizer_2")
               ("weight", 5)
               ("public_key", "PUB_BLS_lkixuIBmY1Xuc0GFnek0iDXitKAAQt87CL7Q1jZsjLid-9cruKfj_KjJk2P4GxsL7HnArD6F0lNAoxsLHISpsBBHqi-ET5JAYfKkS5lgG4To1vP48x6TtOPOslsr5D0YqKbtng")
               ("pop", "SIG_BLS_ubO3gsBLRuXg1UWLvNg54B4Xr2c1wKmwIVUzAxKOCAVKmOHabYdm222v3Go8bPkU7R9sRq2qYkhJQGh3wLbtcIhc2D6CPMpRIUB6mIcX18X2l-3ceXWdWfQBFWs1sUoXfp2UpmeMIPVcDKqgKWnJGyYyei-ekE86XnWaA8bBMzxAcJpEml4E--PRe48m27gVKqaTb6oY27oZHiHbjocZi1TU0RsDNGJAjaIapZkBKRUStQymTE6mmgdgLRHlR4EFjsiRUA")
            }))), eosio_assert_message_exception);
} FC_LOG_AND_RETHROW()

// Verifies threshold greater by one than half of the sum of the weights works
BOOST_FIXTURE_TEST_CASE( threshold_greater_than_by_one_half_weights, eosio_bios_if_tester ) try {
   BOOST_REQUIRE_NO_THROW(push_action("iftester"_n, "setfinalizer"_n, "iftester"_n, mvo()
      ("finalizer_policy", mvo()
         ("threshold", 6)
         ("finalizers", std::vector<mvo>{
            mvo()
               ("description", "set_2_finalizer_1")
               ("weight", 5)
               ("public_key", "PUB_BLS_kV0d54mbPRbd65t4ttv_-CxNt8ktKmf8q4uKZzNTzFSHDSj5rLlP_hdovTsHAPQOAyyzJ4bRTheKjSUj-IoTW96v3VdlifgtDbSVmg4JZR8H_tlStQSWsTHGo8pTX8cR_HEVoA")
               ("pop", "SIG_BLS_DsTwQvYa4uP51putCpLiZlJDyCL24l7bVu1kznc4X8GIYQdElYaCO88RFfCvncYFKwJfKZb-LNMW4GHZzhnyWu0Gp-ougZhSIq4mi1FrtM39uT6KP00-fYYPLOw5nzoLOl__f72qEkBluaaXKIiboWfQ-VnRdzvuE-8Y_m36u6d7bThWy1Lz27mUeelhLawEaKIW688HpFAQDEuvvKfAZYvsyPYXod6a_2KYS4rQkyAtjJXBWk4W0cuPU5n9lNYGyuzF4Q"),
            mvo()
               ("description", "set_2_finalizer_2")
               ("weight", 5)
               ("public_key", "PUB_BLS_lkixuIBmY1Xuc0GFnek0iDXitKAAQt87CL7Q1jZsjLid-9cruKfj_KjJk2P4GxsL7HnArD6F0lNAoxsLHISpsBBHqi-ET5JAYfKkS5lgG4To1vP48x6TtOPOslsr5D0YqKbtng")
               ("pop", "SIG_BLS_ubO3gsBLRuXg1UWLvNg54B4Xr2c1wKmwIVUzAxKOCAVKmOHabYdm222v3Go8bPkU7R9sRq2qYkhJQGh3wLbtcIhc2D6CPMpRIUB6mIcX18X2l-3ceXWdWfQBFWs1sUoXfp2UpmeMIPVcDKqgKWnJGyYyei-ekE86XnWaA8bBMzxAcJpEml4E--PRe48m27gVKqaTb6oY27oZHiHbjocZi1TU0RsDNGJAjaIapZkBKRUStQymTE6mmgdgLRHlR4EFjsiRUA")
            }))));
} FC_LOG_AND_RETHROW()

// Verifies no duplicate keys are allowed
BOOST_FIXTURE_TEST_CASE( duplicate_pub_keys, eosio_bios_if_tester ) try {
   BOOST_REQUIRE_THROW(push_action("iftester"_n, "setfinalizer"_n, "iftester"_n, mvo()
      ("finalizer_policy", mvo()
         ("threshold", 10)
         ("finalizers", std::vector<mvo>{
            mvo()
               ("description", "set_2_finalizer_1")
               ("weight", 5)
               ("public_key", "PUB_BLS_lkixuIBmY1Xuc0GFnek0iDXitKAAQt87CL7Q1jZsjLid-9cruKfj_KjJk2P4GxsL7HnArD6F0lNAoxsLHISpsBBHqi-ET5JAYfKkS5lgG4To1vP48x6TtOPOslsr5D0YqKbtng")
               ("pop", "SIG_BLS_ubO3gsBLRuXg1UWLvNg54B4Xr2c1wKmwIVUzAxKOCAVKmOHabYdm222v3Go8bPkU7R9sRq2qYkhJQGh3wLbtcIhc2D6CPMpRIUB6mIcX18X2l-3ceXWdWfQBFWs1sUoXfp2UpmeMIPVcDKqgKWnJGyYyei-ekE86XnWaA8bBMzxAcJpEml4E--PRe48m27gVKqaTb6oY27oZHiHbjocZi1TU0RsDNGJAjaIapZkBKRUStQymTE6mmgdgLRHlR4EFjsiRUA"),
            mvo()
               ("description", "set_2_finalizer_2")
               ("weight", 5)
               ("public_key", "PUB_BLS_lkixuIBmY1Xuc0GFnek0iDXitKAAQt87CL7Q1jZsjLid-9cruKfj_KjJk2P4GxsL7HnArD6F0lNAoxsLHISpsBBHqi-ET5JAYfKkS5lgG4To1vP48x6TtOPOslsr5D0YqKbtng")
               ("pop", "SIG_BLS_ubO3gsBLRuXg1UWLvNg54B4Xr2c1wKmwIVUzAxKOCAVKmOHabYdm222v3Go8bPkU7R9sRq2qYkhJQGh3wLbtcIhc2D6CPMpRIUB6mIcX18X2l-3ceXWdWfQBFWs1sUoXfp2UpmeMIPVcDKqgKWnJGyYyei-ekE86XnWaA8bBMzxAcJpEml4E--PRe48m27gVKqaTb6oY27oZHiHbjocZi1TU0RsDNGJAjaIapZkBKRUStQymTE6mmgdgLRHlR4EFjsiRUA")
            }))), eosio_assert_message_exception);
} FC_LOG_AND_RETHROW()

// Verifies description cannot exceed maximum allowed size (256 chars)
BOOST_FIXTURE_TEST_CASE( long_description, eosio_bios_if_tester ) try {
   BOOST_REQUIRE_THROW(push_action("iftester"_n, "setfinalizer"_n, "iftester"_n, mvo()
      ("finalizer_policy", mvo()("threshold", 2)
         ("finalizers", std::vector<mvo>{mvo()
            ("description", std::string(257, 'a'))
            ("weight", 2)
            ("public_key", "PUB_BLS_lkixuIBmY1Xuc0GFnek0iDXitKAAQt87CL7Q1jZsjLid-9cruKfj_KjJk2P4GxsL7HnArD6F0lNAoxsLHISpsBBHqi-ET5JAYfKkS5lgG4To1vP48x6TtOPOslsr5D0YqKbtng")
            ("pop", "SIG_BLS_ubO3gsBLRuXg1UWLvNg54B4Xr2c1wKmwIVUzAxKOCAVKmOHabYdm222v3Go8bPkU7R9sRq2qYkhJQGh3wLbtcIhc2D6CPMpRIUB6mIcX18X2l-3ceXWdWfQBFWs1sUoXfp2UpmeMIPVcDKqgKWnJGyYyei-ekE86XnWaA8bBMzxAcJpEml4E--PRe48m27gVKqaTb6oY27oZHiHbjocZi1TU0RsDNGJAjaIapZkBKRUStQymTE6mmgdgLRHlR4EFjsiRUA")}))), eosio_assert_message_exception);
} FC_LOG_AND_RETHROW()

// Verifies description equal to maximum allowed size (256 chars)
BOOST_FIXTURE_TEST_CASE( description_size_equal_to_max, eosio_bios_if_tester ) try {
   BOOST_REQUIRE_NO_THROW(push_action("iftester"_n, "setfinalizer"_n, "iftester"_n, mvo()
      ("finalizer_policy", mvo()("threshold", 2)
         ("finalizers", std::vector<mvo>{mvo()
            ("description", std::string(256, 'a'))
            ("weight", 2)
            ("public_key", "PUB_BLS_lkixuIBmY1Xuc0GFnek0iDXitKAAQt87CL7Q1jZsjLid-9cruKfj_KjJk2P4GxsL7HnArD6F0lNAoxsLHISpsBBHqi-ET5JAYfKkS5lgG4To1vP48x6TtOPOslsr5D0YqKbtng")
            ("pop", "SIG_BLS_ubO3gsBLRuXg1UWLvNg54B4Xr2c1wKmwIVUzAxKOCAVKmOHabYdm222v3Go8bPkU7R9sRq2qYkhJQGh3wLbtcIhc2D6CPMpRIUB6mIcX18X2l-3ceXWdWfQBFWs1sUoXfp2UpmeMIPVcDKqgKWnJGyYyei-ekE86XnWaA8bBMzxAcJpEml4E--PRe48m27gVKqaTb6oY27oZHiHbjocZi1TU0RsDNGJAjaIapZkBKRUStQymTE6mmgdgLRHlR4EFjsiRUA")}))));
} FC_LOG_AND_RETHROW()

BOOST_AUTO_TEST_SUITE_END()
