//#include "search_server.h"
//#include "process_queries.h"
//#include <iostream>
//#include <string>
//#include <vector>
//
//using namespace std;
//
//int main() {
//    SearchServer search_server("and with"s);
//
//    int id = 0;
//    for (
//        const string& text : {
//            "funny pet and nasty rat"s,
//            "funny pet with curly hair"s,
//            "funny pet and not very nasty rat"s,
//            "pet with rat and rat and rat"s,
//            "nasty rat with curly hair"s,
//        }
//        ) {
//        search_server.AddDocument(++id, text, DocumentStatus::ACTUAL, { 1, 2 });
//    }
//
//    const string query = "curly and funny -not"s;
//
//    {
//        const auto [words, status] = search_server.MatchDocument(query, 1);
//        cout << words.size() << " words for document 1"s << endl;
//        // 1 words for document 1
//    }
//
//    {
//        const auto [words, status] = search_server.MatchDocument(execution::seq, query, 2);
//        cout << words.size() << " words for document 2"s << endl;
//        // 2 words for document 2
//    }
//
//    {
//        const auto [words, status] = search_server.MatchDocument(execution::par, query, 3);
//        cout << words.size() << " words for document 3"s << endl;
//        // 0 words for document 3
//    }
//    int a;
//    cin >> a;
//    return 0;
//}

/*
#include "search_server.h"
#include <iostream>
#include <random>
#include <string>
#include <vector>
#include "log_duration.h"
using namespace std;
string GenerateWord(mt19937& generator, int max_length) {
    const int length = uniform_int_distribution(1, max_length)(generator);
    string word;
    word.reserve(length);
    for (int i = 0; i < length; ++i) {
        word.push_back(uniform_int_distribution(static_cast<unsigned int>('a'), static_cast < unsigned int>('z'))(generator));
    }
    return word;
}
vector<string> GenerateDictionary(mt19937& generator, int word_count, int max_length) {
    vector<string> words;
    words.reserve(word_count);
    for (int i = 0; i < word_count; ++i) {
        words.push_back(GenerateWord(generator, max_length));
    }
    sort(words.begin(), words.end());
    words.erase(unique(words.begin(), words.end()), words.end());
    return words;
}
string GenerateQuery(mt19937& generator, const vector<string>& dictionary, int max_word_count) {
    const int word_count = uniform_int_distribution(1, max_word_count)(generator);
    string query;
    for (int i = 0; i < word_count; ++i) {
        if (!query.empty()) {
            query.push_back(' ');
        }
        query += dictionary[uniform_int_distribution<int>(0, dictionary.size() - 1)(generator)];
    }
    return query;
}
vector<string> GenerateQueries(mt19937& generator, const vector<string>& dictionary, int query_count, int max_word_count) {
    vector<string> queries;
    queries.reserve(query_count);
    for (int i = 0; i < query_count; ++i) {
        queries.push_back(GenerateQuery(generator, dictionary, max_word_count));
    }
    return queries;
}
template <typename QueriesProcessor>
void Test(string_view mark, QueriesProcessor processor, const SearchServer& search_server, const vector<string>& queries) {
    LOG_DURATION(mark);
    const auto documents_lists = processor(search_server, queries);
}
#define TEST(processor) Test(#processor, processor, search_server, queries)

int main() {
    mt19937 generator;
    const auto dictionary = GenerateDictionary(generator, 50'000'000, 10);
    const auto documents = GenerateQueries(generator, dictionary, 20'000, 10);
    SearchServer search_server(dictionary[0]);
    for (size_t i = 0; i < documents.size(); ++i) {
        search_server.AddDocument(i, documents[i], DocumentStatus::ACTUAL, { 1, 2, 3 });
    }
    const auto queries = GenerateQueries(generator, dictionary, 2'000, 7);
    //TEST(ProcessQueries);
    {
        LOG_DURATION("match1");
        const auto [words, status] = search_server.MatchDocument(queries[0], 1);
        cout << words.size() << " words for document 1"s << endl;
        // 1 words for document 1
    }

    {
        LOG_DURATION("match2");
        const auto [words, status] = search_server.MatchDocument(execution::seq, queries[1], 2);
        cout << words.size() << " words for document 2"s << endl;
        // 2 words for document 2
    }

    {
        LOG_DURATION("match3");
        const auto [words, status] = search_server.MatchDocument(execution::par, queries[2], 3);
        cout << words.size() << " words for document 3"s << endl;
        // 0 words for document 3
    }
}*/

//#include <iostream>
//#include <random>
//#include <string>
//#include <vector>
//
//#include "log_duration.h"
//
//using namespace std;
//
//string GenerateWord(mt19937& generator, int max_length) {
//    const int length = uniform_int_distribution(1, max_length)(generator);
//    string word;
//    word.reserve(length);
//    for (int i = 0; i < length; ++i) {
//        word.push_back(uniform_int_distribution(97, 122)(generator));
//    }
//    return word;
//}
//
//vector<string> GenerateDictionary(mt19937& generator, int word_count, int max_length) {
//    vector<string> words;
//    words.reserve(word_count);
//    for (int i = 0; i < word_count; ++i) {
//        words.push_back(GenerateWord(generator, max_length));
//    }
//    sort(words.begin(), words.end());
//    words.erase(unique(words.begin(), words.end()), words.end());
//    return words;
//}
//
//string GenerateQuery(mt19937& generator, const vector<string>& dictionary, int word_count, double minus_prob = 0) {
//    string query;
//    for (int i = 0; i < word_count; ++i) {
//        if (!query.empty()) {
//            query.push_back(' ');
//        }
//        if (uniform_real_distribution<>(0, 1)(generator) < minus_prob) {
//            query.push_back('-');
//        }
//        query += dictionary[uniform_int_distribution<int>(0, dictionary.size() - 1)(generator)];
//    }
//    return query;
//}
//
//vector<string> GenerateQueries(mt19937& generator, const vector<string>& dictionary, int query_count, int max_word_count) {
//    vector<string> queries;
//    queries.reserve(query_count);
//    for (int i = 0; i < query_count; ++i) {
//        queries.push_back(GenerateQuery(generator, dictionary, max_word_count));
//    }
//    return queries;
//}
//
//template <typename ExecutionPolicy>
//void Test(string_view mark, SearchServer search_server, const string& query, ExecutionPolicy&& policy) {
//    LOG_DURATION(mark);
//    const int document_count = search_server.GetDocumentCount();
//    int word_count = 0;
//    for (int id = 0; id < document_count; ++id) {
//        const auto [words, status] = search_server.MatchDocument(policy, query, id);
//        word_count += words.size();
//    }
//    cout << word_count << endl;
//}
//
//#define TEST(policy) Test(#policy, search_server, query, execution::policy)
//
//int main() {
//    mt19937 generator;
//
//    const auto dictionary = GenerateDictionary(generator, 1000, 10);
//    const auto documents = GenerateQueries(generator, dictionary, 10'000, 70);
//
//    const string query = GenerateQuery(generator, dictionary, 500, 0.1);
//
//    SearchServer search_server(dictionary[0]);
//    for (size_t i = 0; i < documents.size(); ++i) {
//        search_server.AddDocument(i, documents[i], DocumentStatus::ACTUAL, { 1, 2, 3 });
//    }
//
//    TEST(seq);
//    TEST(par);
//}


#include <algorithm>
#include <execution>
#include <iostream>

using namespace std;

template <typename RandomAccessIterator, typename Value>
RandomAccessIterator LowerBound(const execution::sequenced_policy&,
    RandomAccessIterator range_begin, RandomAccessIterator range_end,
    const Value& value) {
    auto left_bound = range_begin;
    auto right_bound = range_end;
    while (left_bound + 1 < right_bound) {
        const auto middle = left_bound + (right_bound - left_bound) / 2;
        if (*middle < value) {
            left_bound = middle;
        }
        else {
            right_bound = middle;
        }
    }
    if (left_bound == range_begin && !(*left_bound < value)) {
        return left_bound;
    }
    else {
        return right_bound;
    }
}

template <typename RandomAccessIterator, typename Value>
RandomAccessIterator LowerBound(RandomAccessIterator range_begin, RandomAccessIterator range_end,
    const Value& value) {
    return LowerBound(execution::seq, range_begin, range_end, value);
}

template <typename RandomAccessIterator, typename Value>
RandomAccessIterator LowerBound(const execution::parallel_policy&, RandomAccessIterator range_begin,
    RandomAccessIterator range_end, const Value& value) {
    return LowerBound(execution::seq, range_begin, range_end, value);
}

int main() {
    const vector<string> strings = { "cat", "dog", "dog", "horse" };

    const vector<string> requests = { "bear", "cat", "deer", "dog", "dogs", "horses" };

    // последовательные версии
    cout << "Request [" << requests[0] << "] → position "
        << LowerBound(strings.begin(), strings.end(), requests[0]) - strings.begin() << endl;
    cout << "Request [" << requests[1] << "] → position "
        << LowerBound(execution::seq, strings.begin(), strings.end(), requests[1])
        - strings.begin()
        << endl;
    cout << "Request [" << requests[2] << "] → position "
        << LowerBound(execution::seq, strings.begin(), strings.end(), requests[2])
        - strings.begin()
        << endl;

    // параллельные
    cout << "Request [" << requests[3] << "] → position "
        << LowerBound(execution::par, strings.begin(), strings.end(), requests[3])
        - strings.begin()
        << endl;
    cout << "Request [" << requests[4] << "] → position "
        << LowerBound(execution::par, strings.begin(), strings.end(), requests[4])
        - strings.begin()
        << endl;
    cout << "Request [" << requests[5] << "] → position "
        << LowerBound(execution::par, strings.begin(), strings.end(), requests[5])
        - strings.begin()
        << endl;
}