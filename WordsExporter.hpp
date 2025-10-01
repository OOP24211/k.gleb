#ifndef WORDS_EXPORTER_HPP
#define WORDS_EXPORTER_HPP

#include <string>
#include <map>
#include "WordStats.hpp"

class WordsExporter {
    std::string const file_;
    std::map<std::string, Word_stats>& words_map_;
    size_t& total_words_;

public:
    WordsExporter(std::string const& file,
                  std::map<std::string, Word_stats>& words_map,
                  size_t& total);
    void calculate_frequency();
    void export_to_csv();
};

#endif // WORDS_EXPORTER_HPP
