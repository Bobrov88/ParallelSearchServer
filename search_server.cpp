#include "search_server.h"

void SearchServer::AddDocument(int document_id, std::string_view document, DocumentStatus status, const std::vector<int>& ratings) {
        if ((document_id < 0) || (documents_.count(document_id) > 0)) {
            throw std::invalid_argument("Invalid document_id"s);
        }
        const auto [inserted_data, _] = documents_.emplace(document_id, DocumentData{ ComputeAverageRating(ratings), status , document.data() });
        const auto words = SplitIntoWordsNoStop(inserted_data->second.data);

        const double inv_word_count = 1.0 / words.size();
        for (string_view word : words) {
            word_to_document_freqs_[word][document_id] += inv_word_count;
            word_frequencies_[document_id][word]++;
            document_ids_and_words_set_[document_id].insert(word);
            documents_id_.insert(document_id);
        }
    }

    std::vector<Document> SearchServer::FindTopDocuments(const std::string& raw_query, DocumentStatus status) const {
        return FindTopDocuments(raw_query, [status](int document_id, DocumentStatus document_status, int rating) {
            return document_status == status;
            });
    }

    std::vector<Document> SearchServer::FindTopDocuments(const std::string& raw_query) const {
        return FindTopDocuments(raw_query, DocumentStatus::ACTUAL);
    }

    int SearchServer::GetDocumentCount() const {
        return static_cast<int>(documents_.size());
    }

    std::tuple<std::vector<std::string_view>, DocumentStatus> SearchServer::MatchDocument(std::execution::parallel_policy p, const std::string& raw_query, int document_id) const {
      if (documents_.count(document_id) == 0) throw std::out_of_range("No such document");
        const auto query = ParseQuery(raw_query);

        std::vector<std::string_view> matched_words;

        for (std::string_view word : query.minus_words) {
            if (word_to_document_freqs_.count(word) == 0) {
                continue;
            }
            if (word_to_document_freqs_.at(word).count(document_id)) {
                matched_words.clear();
                return { matched_words, documents_.at(document_id).status };
            }
        }

        for (std::string_view word : query.plus_words) {
            if (word_to_document_freqs_.count(word) == 0) {
                continue;
            }
            if (word_to_document_freqs_.at(word).count(document_id)) {
                matched_words.push_back(word);
            }
        }

        return { matched_words, documents_.at(document_id).status };    
 }
    
    std::tuple<std::vector<std::string_view>, DocumentStatus> SearchServer::MatchDocument(std::execution::sequenced_policy p, const std::string& raw_query, int document_id) const {
        if (documents_.count(document_id) == 0) throw std::out_of_range("No such document");
        const auto query = ParseQuery(raw_query);

        std::vector<std::string_view> matched_words;

        for (std::string_view word : query.minus_words) {
            if (word_to_document_freqs_.count(word) == 0) {
                continue;
            }
            if (word_to_document_freqs_.at(word).count(document_id)) {
                matched_words.clear();
                return { matched_words, documents_.at(document_id).status };
            }
        }

        for (std::string_view word : query.plus_words) {
            if (word_to_document_freqs_.count(word) == 0) {
                continue;
            }
            if (word_to_document_freqs_.at(word).count(document_id)) {
                matched_words.push_back(word);
            }
        }

        return { matched_words, documents_.at(document_id).status };
    }

    std::tuple<std::vector<std::string_view>, DocumentStatus> SearchServer::MatchDocument(const std::string& raw_query, int document_id) const {
        return MatchDocument(std::execution::seq, raw_query, document_id);
    }

    bool SearchServer::IsStopWord(std::string_view word) const {
        return stop_words_.count(std::string{ word }) > 0;
    }

    bool SearchServer::IsValidWord(std::string_view word) {
        // A valid word must not contain special characters
        return none_of(word.begin(), word.end(), [](char c) {
            return c >= '\0' && c < ' ';
            });
    }

    std::vector<std::string_view> SearchServer::SplitIntoWordsNoStop(std::string_view text) const {
        std::vector<std::string_view> words;
        for (const std::string_view word : SplitIntoWordsView(text)) {
            if (!IsValidWord(word)) {
                throw std::invalid_argument("Word "s + word.data() + " is invalid"s);
            }
            if (!IsStopWord(word)) {
                words.push_back(word);
            }
        }
        return words;
    }

    int SearchServer::ComputeAverageRating(const std::vector<int>& ratings) {
        if (ratings.empty()) {
            return 0;
        }
        int rating_sum = 0;
        for (const int rating : ratings) {
            rating_sum += rating;
        }
        return rating_sum / static_cast<int>(ratings.size());
    }

    SearchServer::QueryWord SearchServer::ParseQueryWord(std::string_view text) const {
        if (text.empty()) {
            throw std::invalid_argument("Query word is empty"s);
        }
        std::string_view word = text;
        bool is_minus = false;
        if (word[0] == '-') {
            is_minus = true;
            word = word.substr(1);
        }
        if (word.empty() || word[0] == '-' || !IsValidWord(word)) {
            throw std::invalid_argument("Query word "s + text.data() + " is invalid");
        }

        return { word, is_minus, IsStopWord(word) };
    }

    SearchServer::Query SearchServer::ParseQuery(const std::string& text) const {
        //LOG_DURATION(text);
        SearchServer::Query result;
        for (std::string_view word : SplitIntoWordsView(text)) {
            const auto query_word = ParseQueryWord(word);
            if (!query_word.is_stop) {
                if (query_word.is_minus) {
                    result.minus_words.insert(query_word.data);
                }
                else {
                    result.plus_words.insert(query_word.data);
                }
            }
        }
        return result;
    }

double SearchServer::ComputeWordInverseDocumentFreq(std::string_view word) const {
        return std::log(GetDocumentCount() * 1.0 / word_to_document_freqs_.at(word).size());
    }

std::map<string_view, double> SearchServer::GetWordFrequencies(int document_id) const {
        if (word_frequencies_.count(document_id))
            return word_frequencies_.at(document_id);
        return {};
}

std::map<int, std::set<string_view>>& SearchServer::getAccessToSetOfWords() {
    return document_ids_and_words_set_;
}

void SearchServer::RemoveDocument(std::execution::parallel_policy p, int document_id) {
        if (documents_.count(document_id) == 0) return;
        documents_.erase(document_id);
        document_ids_and_words_set_.erase(document_id);
        documents_id_.erase(document_id);
    
/*        auto doc_erase = std::move(word_frequencies_[document_id]);
        std::vector<std::pair<std::string, double>> v(make_move_iterator(doc_erase.begin()), make_move_iterator(doc_erase.end()));
        std::for_each(p,
                      v.begin(),
                      v.end(),
                      [document_id, this] (const auto& element) {
                           word_to_document_freqs_[element.first].erase(document_id);
                      });*/
    
   std::vector<std::pair<const std::string_view, double>*> v(word_frequencies_[document_id].size());
    std::transform(p,
                   word_frequencies_[document_id].begin(),
                   word_frequencies_[document_id].end(),
                   v.begin(), [](auto& el) {
                       return &el;
                   } 
                  );
    for_each(p, v.begin(), v.end(), [this, document_id](auto &element) {
        word_to_document_freqs_[element->first].erase(document_id);
    });
   /* std::vector<std::pair<const int, double>*> v(word_to_document_freqs_.size());
    for_each(word_to_document_freqs_.begin(), word_to_document_freqs_.end(),
             [document_id, &v](auto& element) {
                 auto it = element.second.find(document_id);
                 if (it != element.second.end())
                     v.push_back(&(*it));
                  });*/
            
    word_frequencies_.erase(document_id);
    }

void SearchServer::RemoveDocument(std::execution::sequenced_policy p, int document_id) {
        if (documents_.count(document_id) == 0) return;
        documents_.erase(document_id);
        document_ids_and_words_set_.erase(document_id);
        documents_id_.erase(document_id);
    
        for (auto& doc : word_frequencies_.at(document_id)) {
            word_to_document_freqs_.at(doc.first).erase(document_id);
        }
            
    word_frequencies_.erase(document_id);
    }
    
void SearchServer::RemoveDocument(int document_id) {
    RemoveDocument(std::execution::seq, document_id);
}

void PrintDocument(const Document& document) {
    std::cout << "{ "s
        << "document_id = "s << document.id << ", "s
        << "relevance = "s << document.relevance << ", "s
        << "rating = "s << document.rating << " }"s << std::endl;
}

void PrintMatchDocumentResult(int document_id, const std::vector<std::string>& words, DocumentStatus status) {
    std::cout << "{ "s
        << "document_id = "s << document_id << ", "s
        << "status = "s << static_cast<int>(status) << ", "s
        << "words ="s;
    for (auto& word : words) {
        std::cout << ' ' << word;
    }
   std::cout << "}"s << std::endl;
}