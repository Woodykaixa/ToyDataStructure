#pragma once
#include "map.h"
#include "vlarray.h"
#include <cstring>
#include <string>

class huff_node {
  public:
    enum class append_dir { left = 0, right = 1 };

    enum class node_type { leave = 0, not_leave = 1 };

    explicit huff_node(const node_type& type, const int val = 0,
                       const double rate = 0.0, const unsigned int weight = 0) {
        type_ = type;
        symbol_value_ = val;
        symbol_rate_ = rate;
        symbol_weight_ = weight;
        left_ = nullptr;
        right_ = nullptr;
    }

    void set_rate(const double rate) { symbol_rate_ = rate; }

    double get_rate() const { return symbol_rate_; }

    unsigned int get_weight() const { return symbol_weight_; }

    huff_node* left() const { return left_; }

    huff_node* right() const { return right_; }

    node_type type() const { return type_; }

    int symbol_value() const { return symbol_value_; }

  private:
    huff_node* left_;
    huff_node* right_;
    int symbol_value_;
    unsigned int symbol_weight_;
    double symbol_rate_;
    node_type type_;
    friend class symbol;

    void append_node(huff_node* n, append_dir dir) {
        if (type_ == node_type::leave) {
            return;
        }
        if (dir == append_dir::left) {
            left_ = n;
        } else {
            right_ = n;
        }
    }
};

class symbol {
    using symbol_type = huff_node::node_type;
    char ch_;
    unsigned int weight_;
    double rate_;
    huff_node* node_;
    symbol_type type_;

  public:
    symbol(const symbol_type&& type, const char c = '\0',
           const unsigned int weight = 0) {
        type_ = type;
        ch_ = c;
        weight_ = weight;
        rate_ = 0;
        node_ = new huff_node(type, c, 0, weight);
    }

    ~symbol() = default;
    symbol& operator=(const symbol& rhs) = default;

    void set_rate(const double rate) {
        rate_ = rate;
        node_->set_rate(rate);
    }

    double get_rate() const { return rate_; }

    unsigned int get_weight() const { return weight_; }

    char get_char() const { return ch_; }

    symbol_type get_type() const { return type_; }

    huff_node* get_huff_node() const { return node_; }

    void append_symbol(huff_node::append_dir&& dir, symbol* s) {
        if (type_ == huff_node::node_type::leave) {
            return;
        }
        if (dir == huff_node::append_dir::left) {
            node_->left_ = s->get_huff_node();
        } else {
            node_->right_ = s->get_huff_node();
        }
        if (node_->left_ != nullptr && node_->right_ != nullptr) {
            node_->symbol_rate_ =
                node_->left_->symbol_rate_ + node_->right_->symbol_rate_;
            node_->symbol_weight_ =
                node_->left_->symbol_weight_ + node_->right_->symbol_weight_;
            node_->symbol_value_ =
                node_->left_->symbol_value_ + node_->right_->symbol_value_;
        } else if (node_->left_ != nullptr) {
            node_->symbol_rate_ = node_->left_->symbol_rate_;
            node_->symbol_weight_ = node_->left_->symbol_weight_;
            node_->symbol_value_ = node_->left_->symbol_value_;
        } else {
            node_->symbol_rate_ = node_->right_->symbol_rate_;
            node_->symbol_weight_ = node_->right_->symbol_weight_;
            node_->symbol_value_ = node_->right_->symbol_value_;
        }
        rate_ = node_->symbol_rate_;
        weight_ = node_->symbol_weight_;
        ch_ = char(node_->symbol_value_);
    }
};

class huffman {
    unsigned int total_weight_;
    int symbol_start_index_;
    int symbol_end_index_;
    huff_node* tree_root_;
    cym::vlarray<symbol*>* symbols_;
    cym::map<char, int> char_count_map_;
    cym::map<char, std::string> encode_map_;

  public:
    huffman() {
        total_weight_ = 0;
        symbol_start_index_ = 0;
        symbol_end_index_ = 0;
        tree_root_ = nullptr;
        symbols_ = new cym::vlarray<symbol*>;
    }

    ~huffman() {
        for (int i = 0; i < symbol_end_index_; i++) {
            delete symbols_->at(i);
        }
        delete symbols_;
    }

    void read_string(const char* str) {
        const size_t str_len = strlen(str);
        for (size_t i = 0; i < str_len; ++i) {
            const int count = char_count_map_.get(str[i], 0);
            if (count == 0) {
                char_count_map_.put(str[i], 1);
            } else {
                int count = char_count_map_.get(str[i], 0);
                char_count_map_.remove(str[i]);
                char_count_map_.put(str[i], count + 1);
            }
        }
        cym::pair<char, unsigned int>** pairs = char_count_map_.get_pairs();

        const size_t used = char_count_map_.get_used_count();

        for (size_t i = 0; i < used; i++) {
            cym::pair<char, unsigned int>* p = pairs[i];
            while (p != nullptr) {
                const symbol s(huff_node::node_type::leave, p->key(), p->val());
                add_symbol(s);
                p = p->next();
            }
        }
    }

    void add_symbol(const symbol s) {
        if (total_weight_ + s.get_weight() <= total_weight_) {
            return;
        }

        symbol* sym_copy = new symbol(s);
        (*symbols_)[symbol_end_index_] = sym_copy;
        total_weight_ += s.get_weight();
        symbol_end_index_++;
    }

    void calculate_rate() const {
        for (int i = 0; i < symbol_end_index_; ++i) {
            symbols_->at(i)->set_rate(double(symbols_->at(i)->get_weight()) /
                                      total_weight_ * 100);
        }
    }

  private:
    void sort(const int begin, const int end) const {
        const int mid = (begin + end) / 2;
        symbol* pivot = symbols_->at(mid);
        symbols_->at(mid) = symbols_->at(0);
        symbols_->at(0) = pivot;
        int lb = begin + 1;
        int hb = end;

        while (lb <= hb) {
            while (symbols_->at(lb)->get_rate() < pivot->get_rate() &&
                   lb < hb) {
                lb++;
            }

            while (pivot->get_rate() < symbols_->at(hb)->get_rate()) {
                hb--;
            }

            if (lb < hb) {
                symbol* tmp = symbols_->at(hb);
                symbols_->at(hb) = symbols_->at(lb);
                symbols_->at(lb) = tmp;
                hb--;
                lb++;
            } else {
                lb++;
            }
        }

        symbols_->at(0) = symbols_->at(hb);
        symbols_->at(hb) = pivot;
        if (1 < hb - begin) {
            sort(begin, hb - 1);
        }
        if (1 < end - hb) {
            sort(hb + 1, end);
        }
    }

    void insert_merged_symbol(symbol* s) {
        int i;
        for (i = symbol_start_index_; i < symbol_end_index_; i++) {
            if (s->get_rate() < symbols_->at(i)->get_rate()) {

                break;
            }
        }
        if (symbols_->get_size() == symbol_end_index_) {
            symbols_->resize();
        }
        for (int j = symbol_end_index_; j >= i; j--) {

            symbols_->at(j + 1) = symbols_->at(j);
        }
        symbols_->at(i) = s;
        symbol_end_index_++;
    }

    symbol* get_symbol() {
        symbol* s = symbols_->at(symbol_start_index_++);
        return s;
    }

  public:
    void build_tree() {
        calculate_rate();
        sort(0, symbol_end_index_ - 1);

        while (symbol_end_index_ - symbol_start_index_ > 1) {
            symbol* s1 = get_symbol();
            symbol* s2 = get_symbol();
            symbol* mixed = new symbol(huff_node::node_type::not_leave);
            mixed->append_symbol(huff_node::append_dir::left, s1);
            mixed->append_symbol(huff_node::append_dir::right, s2);
            insert_merged_symbol(mixed);
        }

        tree_root_ = get_symbol()->get_huff_node();
    }

  private:
    void recursive_scan_tree(huff_node* node, std::string str,
                             huff_node::append_dir dir) {
        str += static_cast<char>(dir) + '0';
        if (node->type() == huff_node::node_type::leave) {
            const char c = static_cast<char>(node->symbol_value());
            encode_map_.put(c, str);
            return;
        }
        huff_node* l = node->left();
        if (l != nullptr) {
            recursive_scan_tree(l, str, huff_node::append_dir::left);
        }
        huff_node* r = node->right();
        if (r != nullptr) {
            recursive_scan_tree(r, str, huff_node::append_dir::right);
        }
    }

  public:
    void generate_encoding() {
        huff_node* tmp = tree_root_;
        const std::string empty;
        huff_node* l = tmp->left();
        if (l != nullptr) {
            recursive_scan_tree(l, empty, huff_node::append_dir::left);
        }
        huff_node* r = tmp->right();
        if (r != nullptr) {
            recursive_scan_tree(r, empty, huff_node::append_dir::right);
        }
    }

    std::string encode_string(const char* str) {
        const size_t length = strlen(str);
        std::string result;
        const char* format =
            "<error: cannot convert to huffman code from char: %c>";
        char buffer[53];
        for (size_t i = 0; i < length; i++) {
            sprintf(buffer, format, str[i]);
            std::string s = encode_map_.get(str[i], std::string(buffer));
            result += s;
        }
        return result;
    }

    std::string encode_string(std::string& str) {
        return encode_string(str.c_str());
    }

    huff_node* get_tree() const { return tree_root_; }

  private:
    static std::string decode_error(std::string& huff_code) {
        std::string err = "<error: cannot decode huffman code: ";
        err += huff_code + ">";
        huff_code.clear();
        return err;
    }

    huff_node* parse_code(std::string& result, huff_node* node,
                          std::string& code) const {
        const char last = code[code.length() - 1];
        if (last == '0') {
            node = node->left();
        } else if (last == '1') {
            node = node->right();
        } else {
            result += decode_error(code);
            return tree_root_;
        }
        if (node == nullptr) {
            result += decode_error(code);
            return tree_root_;
        }
        if (node->type() == huff_node::node_type::leave) {
            char c = static_cast<char>(node->symbol_value());
            result += c;
            code.clear();
            return tree_root_;
        }
        return node;
    }

  public:
    std::string decode_string(const char* str) const {
        const size_t length = strlen(str);
        std::string result;
        std::string code;
        huff_node* tmp = tree_root_;

        for (size_t i = 0; i < length; ++i) {
            code += str[i];
            tmp = parse_code(result, tmp, code);
        }
        return result;
    }

    std::string decode_string(std::string& str) const {
        return decode_string(str.c_str());
    }
};
