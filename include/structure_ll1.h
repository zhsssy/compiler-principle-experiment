//
// Created by zhsssy.
//

#ifndef COMPILER_PRINCIPLE_EXPERIMENT_STRUCTURE_LL1_H
#define COMPILER_PRINCIPLE_EXPERIMENT_STRUCTURE_LL1_H

#include <set>
#include <string>
#include <map>
#include <iostream>
#include <exception.h>

using namespace std;

typedef char VT_TYPE;
typedef char VN_TYPE;
typedef map<VN_TYPE, set<string>> Production;
typedef pair<VN_TYPE, set<string>> ProPair;

// lexical -> structure ll1 -> ll1 analysis table construct -> ll1 analysis

class grammar {
private:
    set<VT_TYPE> vt;
    set<VN_TYPE> vn;

    VN_TYPE start;

    Production production;
public:
    grammar();

    ~grammar();

    inline void set_start(const VN_TYPE s);

    inline void add_vt(const VT_TYPE elem);

    inline void add_vn(const VN_TYPE elem);

    inline void remove_vn(const VN_TYPE elem);

    void add_production(const VT_TYPE begin, const string &end);

    inline const VN_TYPE get_start() const;

    inline set<VT_TYPE> &get_vt();

    inline const set<VT_TYPE> &get_vt() const;

    inline set<VN_TYPE> &get_vn();

    inline const set<VN_TYPE> &get_vn() const;

    inline Production &get_production();

    inline const Production &get_production() const;

    [[nodiscard]] inline bool is_vt(char ch) const;

    inline bool is_vn(char ch) const;

    void construct_LL1();

    const VT_TYPE fuzzy_reduced(const string &str) const;

    void print_production();

private:
    void eliminate_left_recursion();

    void eliminate_one_left_recursion(const VN_TYPE elem);

    void eliminate_indirect_recursion(const VN_TYPE i, const VN_TYPE j);

    void eliminate_unneed_vn();

    void eliminate_unneed_vn(const VN_TYPE elem, set<VN_TYPE> &s);

    void extract_left_divisor();

    void extract_one_left_divisor(const VN_TYPE elem);

    const VN_TYPE get_unuse_vn() const;
};

grammar::grammar() : vn(), vt(), production() {}

grammar::~grammar() {}

void grammar::set_start(const VN_TYPE s) {
    start = s;
}

void grammar::print_production() {
    for (ProPair pair: production) {
        string tmp;
        tmp += pair.first;
        tmp += "->";

        for (const auto &s: pair.second) {
            tmp += s + "|";
        }
        tmp.back() = ';';
        cout << tmp << endl;
    }
};

void grammar::add_vt(const VT_TYPE elem) { vt.insert(elem); }

void grammar::add_vn(const VN_TYPE elem) { vn.insert(elem); }

void grammar::remove_vn(const VN_TYPE elem) {
    vn.erase(elem);
    production.erase(elem);
}

const VN_TYPE grammar::get_start() const { return start; }

set<VT_TYPE> &grammar::get_vt() { return vt; }

const set<VT_TYPE> &grammar::get_vt() const { return vt; }

set<VN_TYPE> &grammar::get_vn() { return vn; }

const set<VN_TYPE> &grammar::get_vn() const { return vn; }

Production &grammar::get_production() { return production; }

const Production &grammar::get_production() const { return production; }

bool grammar::is_vt(char ch) const { return vt.contains(ch); }

bool grammar::is_vn(char ch) const { return vn.contains(ch); }


void grammar::add_production(const VN_TYPE begin, const string &end) {
    auto iter = production.find(begin);

    // 找到现有的 不然就直接加入一个新的 production
    if (iter != production.end()) {
        iter->second.insert(end);
    } else {
        auto *ends = new set<string>();
        ends->insert(end);
        production.insert(*(new ProPair(begin, *ends)));
    }
}

void grammar::construct_LL1() {
    this->eliminate_left_recursion();
    this->extract_left_divisor();
}

const VN_TYPE grammar::fuzzy_reduced(const string &str) const {
    size_t index;
#ifdef DEBUG
    cout << "start reduce: " << str << endl;
#endif
    cout << "reduce: " << str << endl;
    for (const VN_TYPE one: vn)
        for (auto pro: production.find(one)->second)
            if (str.size() == pro.size()) {
                for (index = 0; index < str.size(); ++index)
                    if (is_vt(pro.at(index)) && str.at(index) != pro.at(index))
                        break;

                if (index == str.size()) return one;
            }

    throw ReduceException();
}

void grammar::eliminate_left_recursion() {
    set<VN_TYPE>::const_iterator vn_i_citer;
    set<VN_TYPE>::const_iterator vn_j_citer;

    for (vn_i_citer = vn.cbegin(); vn_i_citer != vn.cend(); ++vn_i_citer) {
        for (vn_j_citer = vn.cbegin(); vn_j_citer != vn_i_citer; ++vn_j_citer) {
            eliminate_indirect_recursion(*vn_i_citer, *vn_j_citer);
        }
        eliminate_one_left_recursion(*vn_i_citer);
    }

    eliminate_unneed_vn();
}

void grammar::eliminate_one_left_recursion(VN_TYPE elem) {
    set<string> &p = production[elem];
    set<string> new_set;
    set<string>::const_iterator iter;
    VN_TYPE other = 0;

    for (iter = p.cbegin(); iter != p.cend();) {
        if (iter->at(0) == elem) {
            if (other == 0) {
                other = this->get_unuse_vn();
                vn.insert(other);
                vt.insert('@');
                this->add_production(other, "@");
            }
            this->add_production(other, iter->substr(1, iter->size()) + other);
            iter = p.erase(iter);
        } else ++iter;
    }

    if (other != 0) {
        for (iter = p.cbegin(); iter != p.cend(); ++iter) {
            new_set.insert(*iter + other);
        }
        p.erase(p.begin(), p.end());
        p.insert(new_set.begin(), new_set.end());
    }
}

void grammar::eliminate_indirect_recursion(const VN_TYPE i, const VN_TYPE j) {
    set<string> &i_set = production[i];
    set<string> &j_set = production[j];

    set<string>::const_iterator i_iter;
    set<string>::const_iterator j_iter;

    // 合并带入
    for (i_iter = i_set.cbegin(); i_iter != i_set.cend();) {
        if (i_iter->at(0) == j) {
            for (j_iter = j_set.cbegin(); j_iter != j_set.cend(); ++j_iter) {
                i_set.insert(*j_iter + i_iter->substr(1, i_iter->size()));
            }
            i_iter = i_set.erase(i_iter);
        } else ++i_iter;
    }
}

void grammar::eliminate_unneed_vn() {
    set<VN_TYPE> s;
    set<VN_TYPE> unneed;
    this->eliminate_unneed_vn(start, s);

    set<VN_TYPE>::const_iterator iter;

    for (iter = vn.cbegin(); iter != vn.cend();)
        if (s.find(*iter) == s.end()) {
            production.erase(*iter);
            iter = vn.erase(iter);
        } else ++iter;
}

void grammar::eliminate_unneed_vn(const VN_TYPE elem, set<VN_TYPE> &s) {
    s.insert(elem);
    const set<string> &p = production[elem];
    for (string str: p)
        for (char ch: str)
            if (isupper(ch) && s.find(ch) == s.end())
                this->eliminate_unneed_vn(ch, s);
}

void grammar::extract_left_divisor() {
    for (VN_TYPE ch: vn)
        this->extract_one_left_divisor(ch);
}

void grammar::extract_one_left_divisor(const VN_TYPE elem) {
    set<string> &this_production = production[elem];
    map<char, string> save_map;
    map<char, VN_TYPE> new_vn_map;

    for (auto iter = this_production.cbegin(); iter != this_production.cend();) {
        const char ch = iter->at(0);
        auto save_find = save_map.find(ch);

        if (save_find == save_map.end()) {
            save_map.insert(pair<char, string>(ch, *iter));
            ++iter;
        } else {
            VN_TYPE other;
            auto map_iter = new_vn_map.find(ch);

            if (map_iter == new_vn_map.end()) {
                string s;
                other = this->get_unuse_vn();
                vn.insert(other);

                new_vn_map.insert(pair<char, VN_TYPE>(ch, other));
                const string &str = save_find->second;
                if (str.size() == 1)
                    this->add_production(other, "@");
                else
                    this->add_production(other, str.substr(1, str.size()));

                s += ch;
                s += other;

                // s add new grammar this vn_type
                this_production.insert(s);
                this_production.erase(save_find->second);
            } else other = map_iter->second;

            if (iter->size() == 1)
                this->add_production(other, "@");
            else
                this->add_production(other, iter->substr(1, iter->size()));

            iter = this_production.erase(iter);
        }
    }

    for (auto pair: new_vn_map)
        this->extract_one_left_divisor(pair.second);
}

const VN_TYPE grammar::get_unuse_vn() const {
    VN_TYPE result;
    for (result = 'A'; vn.find(result) != vn.end() && result <= 'Z'; ++result);
    if (result > 'Z')
        throw "Don't have other VT!";
    return result;
}

#endif //COMPILER_PRINCIPLE_EXPERIMENT_STRUCTURE_LL1_H
