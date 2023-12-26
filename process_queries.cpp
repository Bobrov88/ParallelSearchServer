#include "process_queries.h"
#include "search_server.h"
#include "document.h"

std::vector<std::vector<Document>> ProcessQueries(
    const SearchServer& search_server,
    const std::vector<std::string>& queries) {
    std::vector<std::vector<Document>> matched_document(queries.size());
    std::transform(std::execution::par, queries.begin(), queries.end(), matched_document.begin(),
                      [&search_server](const std::string& query) {
                          return search_server.FindTopDocuments(query);
                      });
    return matched_document;
}

std::deque<Document> ProcessQueriesJoined(
    const SearchServer& search_server,
    const std::vector<std::string>& queries) {
    std::deque<Document> result_docs; 
    for (const auto& doc : ProcessQueries(search_server, queries)) {
        for (const auto& inner_doc : doc) {
            result_docs.push_back(inner_doc);
        }
    }
    return result_docs;
}