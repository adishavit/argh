#include <algorithm>
#include <sstream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <cassert>

#include <optional.hpp>

namespace argh
{
    class parser
    {
    public:
        void register_option_with_param(std::string const& name);
        void parse(int argc, char* argv[], bool strict = true);
        bool flag(std::string const& name, bool default_value = false);
    
        auto flags_count() { return flags_.size(); }
        auto options_count() { return options_.size();  }
        
        auto params_count() { return params_.size();  }
        auto const params_cbegin() const { return params_.cbegin(); }
        auto const params_cend()   const { return params_.cend();  }

        template <typename T>
        using optional = std::experimental::optional<T>;

        template <typename T>
        optional<T> get(std::string const& name);

    private:
        std::string trim_leading_dashes(std::string const& name);
        bool is_number(std::string const& arg);
        bool is_option(std::string const& arg);

    private:
        std::vector<std::string> args_;
        std::multimap<std::string, std::string> options_;
        std::vector<std::string> params_;
        std::multiset<std::string> flags_;
        std::multiset<std::string> registeredOptions_;
    };

    //////////////////////////////////////////////////////////////////////////


    void parser::parse(int argc, char* argv[], bool strict)
    {
        // convert to strings
        args_.resize(argc);
        std::transform(argv, argv+argc, args_.begin(), [](auto arg) { return arg;  });

        // parse line
        for (auto i=0; i < args_.size(); ++i)
        {
            if (!is_option(args_[i]))
            {
               params_.emplace_back(args_[i]);
               continue;
            }

            // any potential option will get as its value the next arg, unless that arg is an option too
            // in that case it will be determined a flag.

            auto name = trim_leading_dashes(args_[i]);
            if (i == args_.size()-1 || is_option(args_[i+1]))
            {
               flags_.emplace(name);
               continue;
            }

            // if 'name' is a pre-registered option, then the next arg cannot be a free parameter to it is skipped
            // otherwise we have 2 modes:
            // Strict mode: a non-registered 'name' is determined a flag. 
            //              The following value (the next arg) will be a free parameter.
            //
            // Slack mode: the next arg will appear BOTH as a free parameter AND as the value of that option.
            //             AND the arg will also be stored as a flag since that is a valid condition too.
            //             this saves the user from having to pre-register all/any options at the cost of having to manage
            //             duplicate flags and option values that also appear as free-params
            //             This is suitable for users who do not use free params and do the validation themselves.

            if (registeredOptions_.find(name) != registeredOptions_.end())
            {
                options_.insert({ name, args_[i + 1] });
                ++i; // skip next value, it is not a free parameter
            }
            else
            {
                flags_.emplace(name);
                if (!strict)
                    options_.insert({ name, args_[i + 1] });
            }
        };
    }

    //////////////////////////////////////////////////////////////////////////

    bool parser::is_number(std::string const& arg)
    {
        // inefficient but simple way to determine if a string is a number
        std::istringstream istr(arg);
        double number;
        istr >> number;
        return !(istr.fail() || istr.bad());
    }

    //////////////////////////////////////////////////////////////////////////

    bool parser::is_option(std::string const& arg)
    {
        assert(0 != arg.size());
        if (is_number(arg))
            return false;
        return '-' == arg[0];
    }

    //////////////////////////////////////////////////////////////////////////

    std::string parser::trim_leading_dashes(std::string const& name)
    {
        auto pos = name.find_first_not_of('-');
        return name.substr(pos);
    }

    //////////////////////////////////////////////////////////////////////////

    bool parser::flag(std::string const & name, bool default_value)
    {
        if (flags_.end() != flags_.find(trim_leading_dashes(name)))
            return true;
        return default_value;
    }

    //////////////////////////////////////////////////////////////////////////

    template <typename T>
    parser::optional<T> parser::get(std::string const& name)
    {
        if (name.empty())
            return {};

        auto optIt = options_.find(trim_leading_dashes(name));
        if (options_.end() == optIt)
            return {};

        std::istringstream istr(optIt->second);
        T val;
        istr >> val;
        if (istr.fail() || istr.bad())
            return {};

        return val;
    }

    //////////////////////////////////////////////////////////////////////////

    void parser::register_option_with_param(std::string const& name)
    {
        registeredOptions_.insert(trim_leading_dashes(name));
    }
}



#include <iostream>

using namespace std;

int main(int argc, char* argv[])
{
    argh::parser handler;
    handler.register_option_with_param("-f");
    handler.parse(argc, argv);

    if (handler.flag("-f"))
        cout << "Got -f" << endl;

    cout << handler.get<std::string>("f").value_or("Nope") << endl;

    cout << "Free params:\n";
    for (auto it = handler.params_cbegin(); it != handler.params_cend(); ++it)
        cout << "\t" << *it << endl;

    return EXIT_SUCCESS;
}