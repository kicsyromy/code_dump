#include <iostream>
#include <memory>
#include <vector>
#include <string_view>
#include <algorithm>
#include <limits>

#include <cstdio>
#include <cstring>


using namespace std;

using money_t = std::int64_t;
using timestamp_t = std::int64_t;

struct Person
{
    Person(std::string &&name, money_t balance)
      : name_(std::move(name))
      , account_balance_(balance)
    {}
    Person(const Person &) = delete;
    Person &operator =(const Person &) = delete;
    Person(Person &&) = default;
    Person &operator =(Person &&) = default;

    std::string name_;
    std::int64_t account_balance_;
};

struct Transaction
{
    Transaction(Person &from, Person &to, money_t amount, timestamp_t t)
      : from_(&from)
      , to_(&to)
      , amount_(amount)
      , timestamp_(t)
    { }

    Transaction(const Transaction &) = default;
    Transaction &operator =(const Transaction &other) = default;

    void execute()
    {
        from_->account_balance_ -= amount_;
        to_->account_balance_ += amount_;
    }

    Person *from_;
    Person *to_;

    money_t amount_;
    timestamp_t timestamp_;
};

std::vector<Person> get_people(FILE *file_handle)
{
    std::vector<Person> result;
    int count { 0 };
    fscanf(file_handle, "%d", &count);

    char name[1024];
    std::int64_t mouney;

    result.reserve(static_cast<std::size_t>(count));
    for (int index = 0; index < count; ++index)
    {
        fscanf(file_handle, "%s %ld", &name[0], &mouney);
        result.emplace_back(name, mouney);
    }

    return result;
}

std::vector<Transaction> get_transactions(FILE *file_handle, std::vector<Person> &people)
{
    std::vector<Transaction> result;
    int count { 0 };
    fscanf(file_handle, "%d", &count);

    char from[1024];
    Person *from_p = nullptr;
    char to[1024];
    Person *to_p = nullptr;
    money_t money;
    timestamp_t timestamp;

    result.reserve(static_cast<std::size_t>(count));
    for (int index = 0; index < count; ++index)
    {
        fscanf(file_handle, "%s %s %ld %ld", &from[0], &to[0], &money, &timestamp);
        for (auto &p: people)
        {
            if ((to_p == nullptr) && (string_view(to) == p.name_))
            {
                to_p = &p;
                continue;
            }

            if ((from_p == nullptr) && (string_view(from) == p.name_))
            {
                from_p = &p;
            }
        }
        Person &f = *from_p;
        Person &t = *to_p;
        from_p = nullptr;
        to_p = nullptr;
        result.push_back({ f, t, money, timestamp });
    }

    return result;
}

int main()
{
    using file_handle_t = std::unique_ptr<FILE, decltype(&std::fclose)>;

    file_handle_t input(fopen("/home/kicsyromy/workspace/cccl1/level1-4.txt", "r"), &std::fclose);
    if (!input)
    {
        std::cerr << "Bad file" << strerror(errno) << '\n';
    }

    auto people = get_people(input.get());
//    for (const auto &p : people)
//    {
//        std::cerr << p.name_ << " " << p.account_balance_ << '\n';
//    }

    {
        auto transactions = get_transactions(input.get(), people);

        std::sort(transactions.begin(), transactions.end(), [](const auto &t1, const auto &t2) {
            return t1.timestamp_ < t2.timestamp_;
        });

        for (auto &t : transactions)
        {
//            std::cerr << t.from_.name << " " << t.to_.name << " " << t.amount_ << " " << t.timestamp_ << '\n';
            t.execute();
        }
    }

    const auto s = people.size();
    std::cerr << people.size() << '\n';
    for (std::size_t index = 0; index < s; ++index)
    {
        std::cerr << people.at(index).name_ << " " << people.at(index).account_balance_ << (index == s - 1 ? "" : "\n");
    }

    return 0;
}
