#pragma once

#include <algorithm>
#include <sstream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <cassert>

namespace argh
{
    // Terminology:
    // A command line is composed of 2 types of args:
    // 1. Positional args, i.e. free standing values
    // 2. Options: args beginning with '-'. We identify two kinds:
    //    2.1: Flags: boolean options =>  (exist ? true : false)
    //    2.2: Parameters: a name followed by a non-option value

    class parser
    {
    public:
        enum Mode { PREFER_FLAG_FOR_UNREG_OPTION  = 1 << 0, 
                    PREFER_PARAM_FOR_UNREG_OPTION = 1 << 1,
                    NO_SPLIT_ON_EQUALSIGN         = 1 << 2,
                  };

        void add_param(std::string const& name);
        void parse(int argc, const char* argv[], int mode = PREFER_FLAG_FOR_UNREG_OPTION);

        auto const& flags()    const { return flags_;    }
        auto const& params()   const { return params_;   }
        auto const& pos_args() const { return pos_args_; }

        //////////////////////////////////////////////////////////////////////////
        // Accessors

        // flag (boolean) accessors: return true if the flag appeared, otherwise false.
        bool operator[](std::string const& name); 

        // returns positional arg string by order. Like argv[] but without the options
        std::string const& operator[](size_t ind);

        // returns a std::istream that can be used to convert a positional arg to a typed value.
        std::istringstream operator()(size_t ind);

        // same as above, but with a default value in case the arg is missing (index out of range).
        template<typename T>
        std::istringstream operator()(size_t ind, T&& def_val);

        // parameter accessors, give a name get an std::istream that can be used to convert to a typed value.
        // call .str() on result to get as string
        std::istringstream operator()(std::string const& name);

        // same as above, but with a default value in case the param was missing.
        // Non-string def_val types must have an operator<<() (output stream operator)
        // If T only has an input stream operator, pass the string version of the type as in "3" instead of 3.
        template<typename T>
        std::istringstream operator()(std::string const& name, T&& def_val);

    private:
        std::string trim_leading_dashes(std::string const& name);
        bool is_number(std::string const& arg);
        bool is_option(std::string const& arg);

    private:
        std::vector<std::string> args_;
        std::map<std::string, std::string> params_;
        std::vector<std::string> pos_args_;
        std::multiset<std::string> flags_;
        std::set<std::string> registeredParams_;
        std::string empty_;
    };


    //////////////////////////////////////////////////////////////////////////

    void parser::parse(int argc, const char* argv[], int mode /*= PREFER_FLAG_FOR_UNREG_OPTION*/)
    {
        // convert to strings
        args_.resize(argc);
        std::transform(argv, argv+argc, args_.begin(), [](auto arg) { return arg;  });

        // parse line
        for (auto i=0; i < args_.size(); ++i)
        {
            if (!is_option(args_[i]))
            {
               pos_args_.emplace_back(args_[i]);
               continue;
            }

            auto name = trim_leading_dashes(args_[i]);

            if (!(mode & NO_SPLIT_ON_EQUALSIGN))
            {
                auto equalPos = name.find('=');
                if (equalPos != std::string::npos)
                {
                    params_.insert({ name.substr(0, equalPos), name.substr(equalPos + 1) });
                    continue;
                }
            }

            // any potential option will get as its value the next arg, unless that arg is an option too
            // in that case it will be determined a flag.
            if (i == args_.size()-1 || is_option(args_[i+1]))
            {
               flags_.emplace(name);
               continue;
            }

            // if 'name' is a pre-registered option, then the next arg cannot be a free parameter to it is skipped
            // otherwise we have 2 modes:
            // PREFER_FLAG_FOR_UNREG_OPTION: a non-registered 'name' is determined a flag. 
            //                               The following value (the next arg) will be a free parameter.
            //
            // PREFER_PARAM_FOR_UNREG_OPTION: a non-registered 'name' is determined a parameter, the next arg
            //                                will be the value of that option.

            if (registeredParams_.find(name) != registeredParams_.end() || 
                argh::parser::PREFER_PARAM_FOR_UNREG_OPTION & mode)
            {
                params_.insert({ name, args_[i + 1] });
                ++i; // skip next value, it is not a free parameter
                continue;
            }

            if (argh::parser::PREFER_FLAG_FOR_UNREG_OPTION & mode)
                flags_.emplace(name);
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
        if (ind < pos_args_.size())
            return pos_args_[ind];
        return empty_;
    }

    //////////////////////////////////////////////////////////////////////////

    std::istringstream parser::operator()(std::string const& name)
    {
        if (name.empty())
            return std::istringstream();

        auto optIt = params_.find(trim_leading_dashes(name));
        if (params_.end() == optIt)
            return std::istringstream();

        return std::istringstream(optIt->second);
    }

    //////////////////////////////////////////////////////////////////////////

    template<typename T>
    std::istringstream parser::operator()(std::string const& name, T&& def_val)
    {
        if (name.empty())
        {
            std::ostringstream ostr;
            ostr << def_val;
            return std::istringstream(ostr.str());
        }

        auto optIt = params_.find(trim_leading_dashes(name));
        if (params_.end() == optIt)
        {
            std::ostringstream ostr;
            ostr << def_val;
            return std::istringstream(ostr.str());
        }

        return std::istringstream(optIt->second);
    }

    //////////////////////////////////////////////////////////////////////////

    std::istringstream parser::operator()(size_t ind)
    {
        if (pos_args_.size() <= ind)
            return std::istringstream();

        return std::istringstream(pos_args_[ind]);
    }

    //////////////////////////////////////////////////////////////////////////

    template<typename T>
    std::istringstream parser::operator()(size_t ind, T&& def_val)
    {
        if (pos_args_.size() <= ind)
        {
            std::ostringstream ostr;
            ostr << def_val;
            return std::istringstream(ostr.str());
        }

        return std::istringstream(pos_args_[ind]);
    }

    //////////////////////////////////////////////////////////////////////////

    void parser::add_param(std::string const& name)
    {
        registeredParams_.insert(trim_leading_dashes(name));
    }

    //////////////////////////////////////////////////////////////////////////
}


