#include <iostream>
#include <memory>
#include <vector>
#include <string_view>
#include <algorithm>
#include <limits>
#include <unordered_map>
#include <sstream>

#include <cstdio>
#include <cstring>

using namespace std;

using money_t = std::int64_t;
using timestamp_t = std::int64_t;
using account_id_t = string_view;

constexpr const std::size_t ACCOUNT_ID_LENGTH { 15 };

struct Account
{
    Account(std::string &&name, std::string &&account_number, money_t balance, money_t overdraft)
      : name_(std::move(name))
      , account_number_(std::move(account_number))
      , account_balance_(balance)
      , overdraft_limit_(overdraft)
    {}
    Account(const Account &) = delete;
    Account &operator =(const Account &) = delete;
    Account(Account &&) = default;
    Account &operator =(Account &&) = default;

    bool is_valid() const
    {
        string_view id { account_number_ };
        if (id.size() < ACCOUNT_ID_LENGTH)
        {
            return false;
        }

        string_view prefix { id.data(), 3 };
        if (!(prefix == "CAT"))
        {
            return false;
        }

        /* Convert checksum to number */
        string_view checksum_str { id.data(), 5 };
        checksum_str.remove_prefix(3);
        std::size_t checksum = std::atol(checksum_str.data());

        /* Remove CAT<checksum> */
        id.remove_prefix(5);
        std::unordered_map<char, std::pair<int, int>> letters;
        for (auto &character : id)
        {
            char key = std::tolower(character);
            auto it = letters.find(key);
            if (it == letters.end())
            {
                auto res = letters.emplace(std::make_pair(key, std::make_pair(0 , 0)));
                it = res.first;
            }

            if (character >= 'a' && character <= 'z')
            {
                it->second.first++;
            }
            if (character >= 'A' && character <= 'Z')
            {
                it->second.second++;
            }
        }

        for (const auto &letter : letters)
        {
            if (letter.second.first != letter.second.second)
            {
                return false;
            }
        }

        constexpr std::size_t suffix_checksum { 'C' + 'A' + 'T' + '0' + '0' };
        std::size_t sum { suffix_checksum };
        for (const auto &character : id)
        {
            sum += character;
        }

        std::size_t calculated_checksum { 98 - (sum % 97) };

        return (checksum == calculated_checksum);
    }

    std::string name_;
    std::string account_number_;
    money_t account_balance_;
    money_t overdraft_limit_;
};

struct Transaction
{
    Transaction(Account &from, Account &to, money_t amount, timestamp_t t)
      : from_(&from)
      , to_(&to)
      , amount_(amount)
      , timestamp_(t)
    { }

    Transaction(const Transaction &) = default;
    Transaction &operator =(const Transaction &other) = default;

    void execute()
    {
        auto from_new_balance = from_->account_balance_ - amount_;
        if ((from_new_balance >= 0) || (abs(from_new_balance) <= from_->overdraft_limit_))
        {
            from_->account_balance_ -= amount_;
            to_->account_balance_ += amount_;
        }
    }

    Account *from_;
    Account *to_;

    money_t amount_;
    timestamp_t timestamp_;
};

std::vector<Account> get_accounts(FILE *file_handle)
{
    std::vector<Account> result;
    int count { 0 };
    fscanf(file_handle, "%d", &count);

    char name[1024];
    char account_id[16];
    money_t mouney;
    money_t overdraft;

    result.reserve(static_cast<std::size_t>(count));
    for (int index = 0; index < count; ++index)
    {
        fscanf(file_handle, "%s %s %ld %ld", &name[0], &account_id[0], &mouney, &overdraft);
        result.emplace_back(name, account_id, mouney, overdraft);
    }

    return result;
}

std::vector<Transaction> get_transactions(FILE *file_handle, std::vector<Account> &people)
{
    std::vector<Transaction> result;
    int count { 0 };
    fscanf(file_handle, "%d", &count);

    char from[1024];
    Account *from_p = nullptr;
    char to[1024];
    Account *to_p = nullptr;
    money_t money;
    timestamp_t timestamp;

    result.reserve(static_cast<std::size_t>(count));
    for (int index = 0; index < count; ++index)
    {
        fscanf(file_handle, "%s %s %ld %ld", &from[0], &to[0], &money, &timestamp);
        for (auto &p: people)
        {
            if ((to_p == nullptr) && (string_view(to) == p.account_number_))
            {
                to_p = &p;
                continue;
            }

            if ((from_p == nullptr) && (string_view(from) == p.account_number_))
            {
                from_p = &p;
            }
        }
        Account &f = *from_p;
        Account &t = *to_p;
        if (f.is_valid() && t.is_valid())
        {
            result.push_back({ f, t, money, timestamp });
        }
        from_p = nullptr;
        to_p = nullptr;
    }

    return result;
}


int main()
{
    using file_handle_t = std::unique_ptr<FILE, decltype(&std::fclose)>;

    file_handle_t input(fopen("/home/kicsyromy/workspace/cccl2/level2-4.txt", "r"), &std::fclose);
    if (!input)
    {
        std::cerr << "Bad file" << strerror(errno) << '\n';
    }

//    {
//        auto accounts = get_accounts(input.get());
//        const auto s = accounts.size();
//        std::cerr << accounts.size() << '\n';
//        for (std::size_t index = 0; index < s; ++index)
//        {
//            const auto &account = accounts.at(index);
//            std::cerr << account.name_ << " "
//                      << account.account_number_ << " "
//                      << account.account_balance_ << " "
//                      << account.overdraft_limit_ << " "
//                      << (account.is_valid() ? "valid" : "!valid")
//                      << (index == s - 1 ? "" : "\n");
//        }
//    }

    auto accounts = get_accounts(input.get());
    auto transactions = get_transactions(input.get(), accounts);

    std::sort(transactions.begin(), transactions.end(), [](const auto &t1, const auto &t2) {
        return t1.timestamp_ < t2.timestamp_;
    });

    for (auto &t : transactions)
    {
        t.execute();
    }

    std::size_t valid_accounts = 0;
    const auto s = accounts.size();
    stringstream ss;
    for (std::size_t index = 0; index < s; ++index)
    {
        const auto &account = accounts.at(index);
        if (account.is_valid())
        {
            ++valid_accounts;
            ss << account.name_ << " "
                      << account.account_balance_ << " "
                      << (index == s - 1 ? "" : "\n");
        }
    }

    std::cerr << valid_accounts << '\n' << ss.str();

    return 0;
}
