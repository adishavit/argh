#pragma once

#include <algorithm>
#include <sstream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <cassert>

#ifdef HAVE_OPTIONAL

#include <optional.hpp>

#endif

namespace argh
{
    class parser
    {
    public:
        void add_option(std::string const& name);
        void parse(int argc, char* argv[], bool strict = true);

        // iteration
        auto flags_count()        const { return flags_.size();     }
        auto const flags_cbegin() const { return flags_.cbegin();   }
        auto const flags_cend()   const { return flags_.cend();     }

        auto options_count()        const { return options_.size();     }
        auto const options_cbegin() const { return options_.cbegin();   }
        auto const options_cend()   const { return options_.cend();     }

        auto params_count()        const { return params_.size();   }
        auto const params_cbegin() const { return params_.cbegin(); }
        auto const params_cend()   const { return params_.cend();   }

        // accessors
        //////////////////////////////////////////////////////////////////////////
        // flag (boolean) accessors
        // return true if the flag appeared, otherwise false.
        bool operator[](std::string const& name);

        //////////////////////////////////////////////////////////////////////////
        // returns positional parameter string by order. Like argv[] but without the options
        std::string const& operator[](size_t ind);

        template <typename T>
        bool get(size_t ind, T& val_out);

        template <typename T>
        bool get(size_t ind, T& val_out, T const& default_value);

        //////////////////////////////////////////////////////////////////////////
        // option accessors, give a name get an (optional or default) value
        template <typename T>
        bool get(std::string const& name, T& val_out);

        template <typename T>
        bool get(std::string const& name, T& val_out, T const& default_value);

#ifdef HAVE_OPTIONAL

        template <typename T>
        using optional = std::experimental::optional<T>;

        template <typename T>
        optional<T> get(std::string const& name)
        {
            T val;
            if (get(name, val))
                return val;
            return{};
        }

#endif

    private:
        std::string trim_leading_dashes(std::string const& name);
        bool is_number(std::string const& arg);
        bool is_option(std::string const& arg);

    private:
        std::vector<std::string> args_;
        std::multimap<std::string, std::string> options_;
        std::vector<std::string> params_;
        std::multiset<std::string> flags_;
        std::set<std::string> registeredOptions_;
        std::string empty_;
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
        // inefficient but simple way to determine if a string is a number (which can start with a '-')
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

    bool parser::operator[](std::string const& name)
    {
        return flags_.end() != flags_.find(trim_leading_dashes(name));
    }

    //////////////////////////////////////////////////////////////////////////

    std::string const& parser::operator[](size_t ind)
    {
        if (0 <= ind && ind < params_.size())
            return params_[ind];
        return empty_;
    }

    //////////////////////////////////////////////////////////////////////////

    template<typename T>
    bool parser::get(std::string const & name, T & val_out)
    {
        if (name.empty())
            return false;

        auto optIt = options_.find(trim_leading_dashes(name));
        if (options_.end() == optIt)
            return false;

        std::istringstream istr(optIt->second);
        istr >> val_out;
        if (istr.fail() || istr.bad())
            return false;

        return true;
    }

    //////////////////////////////////////////////////////////////////////////

    template<typename T>
    bool parser::get(std::string const & name, T & val_out, T const& default_value)
    {
        if (get(name, val_out))
            return true;
        val_out = default_value;
        return true;
    }

    //////////////////////////////////////////////////////////////////////////

    template <typename T>
    bool parser::get(size_t ind, T& val_out)
    {
        if (name.empty())
            return false;

        if (ind < 0 || params_.size() <= ind)
            return false;

        std::istringstream istr(params_[ind]);
        istr >> val_out;
        if (istr.fail() || istr.bad())
            return false;
        return true;
    }

    //////////////////////////////////////////////////////////////////////////

    template <typename T>
    bool parser::get(size_t ind, T& val_out, T const& default_value)
    {
        if (get(ind, val_out))
            return true;
        val_out = default_value;
        return true;
    }

    //////////////////////////////////////////////////////////////////////////

    void parser::add_option(std::string const& name)
    {
        registeredOptions_.insert(trim_leading_dashes(name));
    }

    //////////////////////////////////////////////////////////////////////////
}


