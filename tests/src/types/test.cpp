#define CATCH_CONFIG_RUNNER
#include "../../vendor/catch/catch.hpp"

#include "thread_utility.hpp"
#include "types.hpp"
#include <boost/optional/optional_io.hpp>

TEST_CASE("input_source_selector") {
  // language
  {
    krbn::input_source_selector selector(std::string("^en$"),
                                         boost::none,
                                         boost::none);

    {
      nlohmann::json expected;
      expected["language"] = "^en$";
      REQUIRE(selector.to_json() == expected);
    }

    REQUIRE(selector.test(krbn::input_source_identifiers(boost::none,
                                                         boost::none,
                                                         boost::none)) == false);

    REQUIRE(selector.test(krbn::input_source_identifiers(std::string("en"),
                                                         boost::none,
                                                         boost::none)) == true);

    REQUIRE(selector.test(krbn::input_source_identifiers(std::string("en"),
                                                         std::string("com.apple.keylayout.US"),
                                                         std::string("com.apple.inputmethod.Japanese.FullWidthRoman"))) == true);

    // regex
    REQUIRE(selector.test(krbn::input_source_identifiers(std::string("en2"),
                                                         boost::none,
                                                         boost::none)) == false);
  }

  // input_source_id
  {
    krbn::input_source_selector selector(boost::none,
                                         std::string("^com\\.apple\\.keylayout\\.US$"),
                                         boost::none);

    {
      nlohmann::json expected;
      expected["input_source_id"] = "^com\\.apple\\.keylayout\\.US$";
      REQUIRE(selector.to_json() == expected);
    }

    REQUIRE(selector.test(krbn::input_source_identifiers(boost::none,
                                                         boost::none,
                                                         boost::none)) == false);

    REQUIRE(selector.test(krbn::input_source_identifiers(boost::none,
                                                         std::string("com.apple.keylayout.US"),
                                                         boost::none)) == true);

    REQUIRE(selector.test(krbn::input_source_identifiers(std::string("en"),
                                                         std::string("com.apple.keylayout.US"),
                                                         std::string("com.apple.inputmethod.Japanese.FullWidthRoman"))) == true);

    // regex
    REQUIRE(selector.test(krbn::input_source_identifiers(boost::none,
                                                         std::string("com/apple/keylayout/US"),
                                                         boost::none)) == false);
  }

  // input_mode_id
  {
    krbn::input_source_selector selector(boost::none,
                                         boost::none,
                                         std::string("^com\\.apple\\.inputmethod\\.Japanese\\.FullWidthRoman$"));

    {
      nlohmann::json expected;
      expected["input_mode_id"] = "^com\\.apple\\.inputmethod\\.Japanese\\.FullWidthRoman$";
      REQUIRE(selector.to_json() == expected);
    }

    REQUIRE(selector.test(krbn::input_source_identifiers(boost::none,
                                                         boost::none,
                                                         boost::none)) == false);

    REQUIRE(selector.test(krbn::input_source_identifiers(boost::none,
                                                         boost::none,
                                                         std::string("com.apple.inputmethod.Japanese.FullWidthRoman"))) == true);

    REQUIRE(selector.test(krbn::input_source_identifiers(std::string("en"),
                                                         std::string("com.apple.keylayout.US"),
                                                         std::string("com.apple.inputmethod.Japanese.FullWidthRoman"))) == true);

    // regex
    REQUIRE(selector.test(krbn::input_source_identifiers(boost::none,
                                                         boost::none,
                                                         std::string("com/apple/inputmethod/Japanese/FullWidthRoman"))) == false);
  }

  // combination
  {
    krbn::input_source_selector selector(std::string("^en$"),
                                         std::string("^com\\.apple\\.keylayout\\.US$"),
                                         std::string("^com\\.apple\\.inputmethod\\.Japanese\\.FullWidthRoman$"));

    REQUIRE(selector.test(krbn::input_source_identifiers(boost::none,
                                                         boost::none,
                                                         boost::none)) == false);

    REQUIRE(selector.test(krbn::input_source_identifiers(std::string("en"),
                                                         boost::none,
                                                         boost::none)) == false);

    REQUIRE(selector.test(krbn::input_source_identifiers(boost::none,
                                                         std::string("com.apple.keylayout.US"),
                                                         boost::none)) == false);

    REQUIRE(selector.test(krbn::input_source_identifiers(boost::none,
                                                         boost::none,
                                                         std::string("com.apple.inputmethod.Japanese.FullWidthRoman"))) == false);

    REQUIRE(selector.test(krbn::input_source_identifiers(std::string("en"),
                                                         std::string("com.apple.keylayout.US"),
                                                         std::string("com.apple.inputmethod.Japanese.FullWidthRoman"))) == true);

    // combination
    REQUIRE(selector.test(krbn::input_source_identifiers(std::string("en"),
                                                         std::string("com/apple/keylayout/US"),
                                                         std::string("com.apple.inputmethod.Japanese.FullWidthRoman"))) == false);
  }

  // none selector
  {
    krbn::input_source_selector selector(boost::none,
                                         boost::none,
                                         boost::none);

    REQUIRE(selector.test(krbn::input_source_identifiers(boost::none,
                                                         boost::none,
                                                         boost::none)) == true);

    REQUIRE(selector.test(krbn::input_source_identifiers(std::string("en"),
                                                         boost::none,
                                                         boost::none)) == true);

    REQUIRE(selector.test(krbn::input_source_identifiers(boost::none,
                                                         std::string("com.apple.keylayout.US"),
                                                         boost::none)) == true);

    REQUIRE(selector.test(krbn::input_source_identifiers(boost::none,
                                                         boost::none,
                                                         std::string("com.apple.inputmethod.Japanese.FullWidthRoman"))) == true);

    REQUIRE(selector.test(krbn::input_source_identifiers(std::string("en"),
                                                         std::string("com.apple.keylayout.US"),
                                                         std::string("com.apple.inputmethod.Japanese.FullWidthRoman"))) == true);
  }
}

TEST_CASE("make_key_code") {
  REQUIRE(krbn::types::make_key_code("spacebar") == krbn::key_code::spacebar);
  REQUIRE(krbn::types::make_key_code("unknown") == boost::none);
  REQUIRE(krbn::types::make_key_code_name(krbn::key_code::spacebar) == std::string("spacebar"));
  REQUIRE(krbn::types::make_key_code_name(krbn::key_code::left_option) == std::string("left_alt"));

  {
    auto actual = krbn::types::make_key_code(krbn::hid_usage_page(kHIDPage_KeyboardOrKeypad),
                                             krbn::hid_usage(kHIDUsage_KeyboardTab));
    REQUIRE(*actual == krbn::key_code(kHIDUsage_KeyboardTab));
  }
  {
    auto actual = krbn::types::make_key_code(krbn::hid_usage_page(krbn::kHIDPage_AppleVendorTopCase),
                                             krbn::hid_usage(krbn::kHIDUsage_AV_TopCase_KeyboardFn));
    REQUIRE(*actual == krbn::key_code::fn);
  }
  {
    auto actual = krbn::types::make_key_code(krbn::hid_usage_page(krbn::kHIDPage_AppleVendorKeyboard),
                                             krbn::hid_usage(krbn::kHIDUsage_AppleVendorKeyboard_Function));
    REQUIRE(*actual == krbn::key_code::fn);
  }
  {
    auto actual = krbn::types::make_key_code(krbn::hid_usage_page(kHIDPage_Button),
                                             krbn::hid_usage(1));
    REQUIRE(actual == boost::none);
  }
}

TEST_CASE("make_key_code (modifier_flag)") {
  REQUIRE(krbn::types::make_key_code(krbn::modifier_flag::zero) == boost::none);
  REQUIRE(krbn::types::make_key_code(krbn::modifier_flag::caps_lock) == krbn::key_code::caps_lock);
  REQUIRE(krbn::types::make_key_code(krbn::modifier_flag::left_control) == krbn::key_code::left_control);
  REQUIRE(krbn::types::make_key_code(krbn::modifier_flag::left_shift) == krbn::key_code::left_shift);
  REQUIRE(krbn::types::make_key_code(krbn::modifier_flag::left_option) == krbn::key_code::left_option);
  REQUIRE(krbn::types::make_key_code(krbn::modifier_flag::left_command) == krbn::key_code::left_command);
  REQUIRE(krbn::types::make_key_code(krbn::modifier_flag::right_control) == krbn::key_code::right_control);
  REQUIRE(krbn::types::make_key_code(krbn::modifier_flag::right_shift) == krbn::key_code::right_shift);
  REQUIRE(krbn::types::make_key_code(krbn::modifier_flag::right_option) == krbn::key_code::right_option);
  REQUIRE(krbn::types::make_key_code(krbn::modifier_flag::right_command) == krbn::key_code::right_command);
  REQUIRE(krbn::types::make_key_code(krbn::modifier_flag::fn) == krbn::key_code::fn);
  REQUIRE(krbn::types::make_key_code(krbn::modifier_flag::end_) == boost::none);
}

TEST_CASE("make_modifier_flag") {
  REQUIRE(krbn::types::make_modifier_flag(krbn::key_code::caps_lock) == boost::none);
  REQUIRE(krbn::types::make_modifier_flag(krbn::key_code::left_control) == krbn::modifier_flag::left_control);
  REQUIRE(krbn::types::make_modifier_flag(krbn::key_code::left_shift) == krbn::modifier_flag::left_shift);
  REQUIRE(krbn::types::make_modifier_flag(krbn::key_code::left_option) == krbn::modifier_flag::left_option);
  REQUIRE(krbn::types::make_modifier_flag(krbn::key_code::left_command) == krbn::modifier_flag::left_command);
  REQUIRE(krbn::types::make_modifier_flag(krbn::key_code::right_control) == krbn::modifier_flag::right_control);
  REQUIRE(krbn::types::make_modifier_flag(krbn::key_code::right_shift) == krbn::modifier_flag::right_shift);
  REQUIRE(krbn::types::make_modifier_flag(krbn::key_code::right_option) == krbn::modifier_flag::right_option);
  REQUIRE(krbn::types::make_modifier_flag(krbn::key_code::right_command) == krbn::modifier_flag::right_command);
  REQUIRE(krbn::types::make_modifier_flag(krbn::key_code::fn) == krbn::modifier_flag::fn);

  REQUIRE(krbn::types::make_modifier_flag(krbn::hid_usage_page::keyboard_or_keypad, krbn::hid_usage(kHIDUsage_KeyboardA)) == boost::none);
  REQUIRE(krbn::types::make_modifier_flag(krbn::hid_usage_page::keyboard_or_keypad, krbn::hid_usage(kHIDUsage_KeyboardErrorRollOver)) == boost::none);
  REQUIRE(krbn::types::make_modifier_flag(krbn::hid_usage_page::keyboard_or_keypad, krbn::hid_usage(kHIDUsage_KeyboardLeftShift)) == krbn::modifier_flag::left_shift);
  REQUIRE(krbn::types::make_modifier_flag(krbn::hid_usage_page::button, krbn::hid_usage(1)) == boost::none);
}

TEST_CASE("make_consumer_key_code") {
  REQUIRE(krbn::types::make_consumer_key_code("mute") == krbn::consumer_key_code::mute);
  REQUIRE(!krbn::types::make_consumer_key_code("unknown"));

  REQUIRE(krbn::types::make_consumer_key_code_name(krbn::consumer_key_code::mute) == std::string("mute"));

  REQUIRE(krbn::types::make_consumer_key_code(krbn::hid_usage_page::consumer, krbn::hid_usage::csmr_mute) == krbn::consumer_key_code::mute);
  REQUIRE(!krbn::types::make_consumer_key_code(krbn::hid_usage_page::keyboard_or_keypad, krbn::hid_usage(kHIDUsage_KeyboardA)));

  REQUIRE(krbn::types::make_hid_usage_page(krbn::consumer_key_code::mute) == krbn::hid_usage_page::consumer);
  REQUIRE(krbn::types::make_hid_usage(krbn::consumer_key_code::mute) == krbn::hid_usage::csmr_mute);
}

TEST_CASE("make_pointing_button") {
  REQUIRE(krbn::types::make_pointing_button("button1") == krbn::pointing_button::button1);
  REQUIRE(!krbn::types::make_pointing_button("unknown"));

  REQUIRE(krbn::types::make_pointing_button_name(krbn::pointing_button::button1) == std::string("button1"));

  {
    auto actual = krbn::types::make_pointing_button(krbn::hid_usage_page(kHIDPage_Button),
                                                    krbn::hid_usage(1));
    REQUIRE(*actual == krbn::pointing_button::button1);
  }
  {
    auto actual = krbn::types::make_pointing_button(krbn::hid_usage_page(kHIDPage_KeyboardOrKeypad),
                                                    krbn::hid_usage(kHIDUsage_KeyboardTab));
    REQUIRE(actual == boost::none);
  }
}

TEST_CASE("make_new_device_id") {
  auto device_id1 = krbn::types::make_new_device_id(krbn::vendor_id(1234),
                                                    krbn::product_id(5678),
                                                    true,
                                                    false);
  auto device_id2 = krbn::types::make_new_device_id(krbn::vendor_id(2345),
                                                    krbn::product_id(6789),
                                                    false,
                                                    true);

  REQUIRE(krbn::types::find_device_identifiers(device_id1)->get_vendor_id() == krbn::vendor_id(1234));
  REQUIRE(krbn::types::find_device_identifiers(device_id1)->get_product_id() == krbn::product_id(5678));
  REQUIRE(krbn::types::find_device_identifiers(device_id1)->get_is_keyboard() == true);
  REQUIRE(krbn::types::find_device_identifiers(device_id1)->get_is_pointing_device() == false);

  REQUIRE(krbn::types::find_device_identifiers(device_id2)->get_vendor_id() == krbn::vendor_id(2345));
  REQUIRE(krbn::types::find_device_identifiers(device_id2)->get_product_id() == krbn::product_id(6789));
  REQUIRE(krbn::types::find_device_identifiers(device_id2)->get_is_keyboard() == false);
  REQUIRE(krbn::types::find_device_identifiers(device_id2)->get_is_pointing_device() == true);

  REQUIRE(krbn::types::find_device_identifiers(krbn::device_id(-1)) == nullptr);

  krbn::types::detach_device_id(device_id1);
  krbn::types::detach_device_id(krbn::device_id(-1));

  REQUIRE(krbn::types::find_device_identifiers(device_id1) == nullptr);
}

int main(int argc, char* const argv[]) {
  krbn::thread_utility::register_main_thread();
  return Catch::Session().run(argc, argv);
}
