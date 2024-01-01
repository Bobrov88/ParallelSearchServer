#pragma once
#include <set>
#include <vector>
#include <string>
#include <string_view>
#include <execution>

template <typename StringContainer>
std::set<std::string> MakeUniqueNonEmptyStrings(const StringContainer& strings) {
    std::set<std::string> non_empty_strings;
    for (string_view str : strings) {
        if (!str.empty()) {
            std::string str_const{ str };
            non_empty_strings.insert(str_const);
        }
    }
    return non_empty_strings;
}

std::vector<std::string_view> SplitIntoWordsView(std::string_view text);