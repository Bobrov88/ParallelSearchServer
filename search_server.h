#pragma once
#include "document.h"
#include "string_processing.h"
#include <set>
#include <map>
#include <deque>
#include <vector>
#include <string>
#include <string_view>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <execution>
#include "log_duration.h"

const int MAX_RESULT_DOCUMENT_COUNT = 5;

struct Document;
enum class DocumentStatus;
using namespace std;
void PrintDocument(const Document& document);
void PrintMatchDocumentResult(int document_id, const std::vector<std::string>& words, DocumentStatus status);

class SearchServer {
public:
    template <typename StringContainer>
    explicit SearchServer(const StringContainer& stop_words)
        : stop_words_(MakeUniqueNonEmptyStrings(stop_words))  // Extract non-empty stop words
    {
        if (!std::all_of(stop_words_.begin(), stop_words_.end(), IsValidWord)) {
            //std::string str = "Some of stop words are invalid";
            throw std::invalid_argument("Some of stop words are invalid"s);
        }
    }

    
    SearchServer(const std::string& stop_words_text)
        : SearchServer(SplitIntoWordsView(stop_words_text))  // Invoke delegating constructor
        // from string container
    {
    }

    void AddDocument(int document_id, const std::string& document, DocumentStatus status, const std::vector<int>& ratings);
    
void RemoveDocument(std::execution::parallel_policy p, int document_id);
void RemoveDocument(std::execution::sequenced_policy p, int document_id); 
void RemoveDocument(int document_id);
    
    template <typename DocumentPredicate>
    std::vector<Document> FindTopDocuments(const std::string& raw_query, DocumentPredicate document_predicate) const {
        const auto query = ParseQuery(raw_query);

        auto matched_documents = FindAllDocuments(query, document_predicate);

        std::sort(matched_documents.begin(), matched_documents.end(), [](const Document& lhs, const Document& rhs) {
            if (abs(lhs.relevance - rhs.relevance) < 1e-6) {
                return lhs.rating > rhs.rating;
            }
            else {
                return lhs.relevance > rhs.relevance;
            }
            });
        if (matched_documents.size() > MAX_RESULT_DOCUMENT_COUNT) {
            matched_documents.resize(MAX_RESULT_DOCUMENT_COUNT);
        }

        return matched_documents;
    }

    std::vector<Document> FindTopDocuments(const std::string& raw_query, DocumentStatus status) const;
    std::vector<Document> FindTopDocuments(const std::string& raw_query) const;
    int GetDocumentCount() const;
    const auto begin() const { return documents_id_.cbegin();}
    const auto end() const { return documents_id_.cend();} 
    std::map<int, std::set<string>>& getAccessToSetOfWords();

    std::tuple<std::vector<std::string>, DocumentStatus> MatchDocument(std::execution::parallel_policy p, const std::string& raw_query, int document_id) const;
    std::tuple<std::vector<std::string>, DocumentStatus> MatchDocument(std::execution::sequenced_policy p, const std::string& raw_query, int document_id) const;
    std::tuple<std::vector<std::string>, DocumentStatus> MatchDocument(const std::string& raw_query, int document_id) const;
    std::map<string, double> GetWordFrequencies(int document_id) const;
    
private:
    struct DocumentData {
        int rating;
        DocumentStatus status;
    };
    
    const std::set<std::string_view> stop_words_;
    std::map<std::string_view, std::map<int, double>> word_to_document_freqs_;
    std::map<int, map<std::string_view, double>> word_frequencies_;
    std::map<int, DocumentData> documents_;
    std::map<int, std::set<std::string_view>> document_ids_and_words_set_;
    std::set<int> documents_id_;

    bool IsStopWord(std::string_view word) const;

    static bool IsValidWord(std::string_view word);
    std::vector<std::string_view> SplitIntoWordsNoStop(const std::string& text) const;
    static int ComputeAverageRating(const std::vector<int>& ratings);
        
    struct QueryWord {
        std::string data;
        bool is_minus;
        bool is_stop;
    };

    SearchServer::QueryWord ParseQueryWord(const std::string& text) const;

    struct Query {
        std::set<std::string> plus_words;
        std::set<std::string> minus_words;
    };

    SearchServer::Query ParseQuery(const std::string& text) const;
    SearchServer::Query ParseQuery(std::execution::parallel_policy p, const std::string& text) const;
    double ComputeWordInverseDocumentFreq(const std::string& word) const;

    template <typename DocumentPredicate>
    std::vector<Document> FindAllDocuments(const Query& query, DocumentPredicate document_predicate) const {
        std::map<int, double> document_to_relevance;
        for (const std::string& word : query.plus_words) {
            if (word_to_document_freqs_.count(word) == 0) {
                continue;
            }
            const double inverse_document_freq = ComputeWordInverseDocumentFreq(word);
            for (const auto [document_id, term_freq] : word_to_document_freqs_.at(word)) {
                const auto& document_data = documents_.at(document_id);
                if (document_predicate(document_id, document_data.status, document_data.rating)) {
                    document_to_relevance[document_id] += term_freq * inverse_document_freq;
                }
            }
        }

        for (const std::string& word : query.minus_words) {
            if (word_to_document_freqs_.count(word) == 0) {
                continue;
            }
            for (const auto [document_id, _] : word_to_document_freqs_.at(word)) {
                document_to_relevance.erase(document_id);
            }
        }

        std::vector<Document> matched_documents;
        for (const auto [document_id, relevance] : document_to_relevance) {
            matched_documents.push_back({ document_id, relevance, documents_.at(document_id).rating });
        }
        return matched_documents;
    }
};

