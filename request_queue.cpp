#include "request_queue.h"

    RequestQueue::RequestQueue(const SearchServer& search_server) : search_server_(search_server) {
    }

    vector<Document> RequestQueue::AddFindRequest(const string& raw_query, DocumentStatus status) {
        vector<Document> documents = search_server_.FindTopDocuments(raw_query, status);
        PushIntoQueryResult(documents.empty() ? true : false);
        return documents;
    }

    vector<Document> RequestQueue::AddFindRequest(const string& raw_query) {
        vector<Document> documents = search_server_.FindTopDocuments(raw_query);
        PushIntoQueryResult(documents.empty() ? true : false);
        return documents;
    }

    int RequestQueue::GetNoResultRequests() const {
        return no_result_requests_;
    }

    void RequestQueue::PushIntoQueryResult(bool empty) {
        requests_.push_back({ empty });
        if (empty) no_result_requests_++;
        if (requests_.size() > min_in_day_) {
            if (requests_.front().empty) {
                no_result_requests_--;
            }
            requests_.pop_front();
        }
    }