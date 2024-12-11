#include <algorithm>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_set>
#include <string_view>

using namespace std;

class Domain {
    // разработайте класс домена
public:
    explicit Domain(string_view strin) {
        domain_ = string(strin.rbegin(), strin.rend()) + "."s;
    }

    //explicit Domain(const string& strin) : domain_(strin) {}

    // конструктор должен позволять конструирование из string, с сигнатурой определитесь сами
    bool operator==(const Domain& other) const {
        return domain_ == other.domain_;
    }

    bool operator<(const Domain& other) const {
        return std::lexicographical_compare(domain_.begin(), domain_.end(), other.domain_.begin(), other.domain_.end(),
            [](char l, char r) {
                return (r != '.') && (l < r || l == '.');
            });
    }



    // разработайте operator==
    bool IsSubdomain(const Domain& other) const {
        return domain_.size() >= other.domain_.size() && domain_.substr(0, other.domain_.size()) == other.domain_;
    }


    const std::string& GetName() const { return domain_; }


private:
    std::string domain_;
};


namespace std {
    template <>
    struct hash<Domain> {
        size_t operator()(const Domain& domain) const {
            return std::hash<std::string_view>()(std::string_view(domain.GetName()));
        }
    };
}

class DomainChecker {
public:
    // конструктор должен принимать список запрещённых доменов через пару итераторов
    template <typename InputIterator>
    DomainChecker(InputIterator first, InputIterator last) {
        std::vector<Domain> all_domains(first, last);
        sort(all_domains.begin(), all_domains.end());
        bad_domain_ = AbsorbSubdomains(std::move(all_domains));
    }

    // разработайте метод IsForbidden, возвращающий true, если домен запрещён
    bool IsForbidden(const Domain& domain) const {
        auto iter = upper_bound(bad_domain_.begin(), bad_domain_.end(), domain);
        if (iter == bad_domain_.begin()) {
            return false;
        }

        return domain.IsSubdomain(*prev(iter));
    }


private:
    std::vector<Domain> bad_domain_;
    //std::unordered_set<Domain> bad_domain_;

    static vector<Domain> AbsorbSubdomains(vector<Domain> domains) {
        domains.erase(unique(begin(domains), end(domains),
            [](const Domain& lhs, const Domain& rhs) {
                return lhs.IsSubdomain(rhs) || rhs.IsSubdomain(lhs);
            }),
            end(domains));
        return domains;
    }



};

// разработайте функцию ReadDomains, читающую заданное количество доменов из стандартного входа
std::vector<Domain> ReadDomains(std::istream& stream, size_t num) {
    std::vector<Domain> domains;
    std::string input;
    for (size_t i = 0; i < num; ++i) {
        std::getline(stream, input);
        domains.push_back(Domain(input));
    }
    return domains;
}

template <typename Number>
Number ReadNumberOnLine(istream& input) {
    string line;
    getline(input, line);

    Number num;
    std::istringstream(line) >> num;

    return num;
}

int main() {
    const std::vector<Domain> forbidden_domains = ReadDomains(cin, ReadNumberOnLine<size_t>(cin));
    DomainChecker checker(forbidden_domains.begin(), forbidden_domains.end());

    const std::vector<Domain> test_domains = ReadDomains(cin, ReadNumberOnLine<size_t>(cin));
    for (const Domain& domain : test_domains) {
        cout << (checker.IsForbidden(domain) ? "Bad"sv : "Good"sv) << endl;
    }
}
