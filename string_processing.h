#pragma once
#include <set>
#include <vector>
#include <string>
#include <string_view>
#include <execution>

template <typename StringContainer>
std::set<std::string_view> MakeUniqueNonEmptyStrings(const StringContainer& strings) {
    std::set<std::string_view> non_empty_strings;
    for (const std::string& str : strings) {
        if (!str.empty()) {
            non_empty_strings.insert(str);
        }
    }
    return non_empty_strings;
}

std::vector<std::string_view> SplitIntoWordsView(const std::string& text);