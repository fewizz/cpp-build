#pragma once

#include <string>
#include <optional>
#include <filesystem>
#include <vector>
#include <set>
#include "command.hpp"
#include <ranges>
#include <iterator>

namespace ar {

struct command_builder {
	
    struct operation_t {
		using code_t = char;
		static constexpr code_t
			del = 'd', move = 'm', print = 'p',
			append = 'q', replace = 'r', index = 's',
			display_table = 't', extract = 'x';
		
		using modifier_t = char;
		static constexpr modifier_t
			create_if_not_exists = 'c',
			create_index = 's',
			make_thin_archive = 'T',
			verbose = 'v';
		
        code_t code;
        std::set<modifier_t> modifiers;

        operator std::string() {
            std::string result{"-"};
            result+=code;
            for(auto m : modifiers) result+=m;
            return result;
        }
    } operation;
    
    std::optional<std::filesystem::path> archive;
    std::vector<std::filesystem::path> members;

    operator cmd::command() {
        std::vector<std::string> args;
        if(!archive)
            throw std::runtime_error("archive name is not specified");
        
        args.push_back(operation);
        args.push_back(archive->string());
        
        for(auto& mp : members)
            args.push_back(mp.string());
        
        return {"ar", args};
    }
};

struct cb_owner {
	command_builder cb;
	
	cb_owner(){}
	cb_owner(command_builder&& cb):cb{std::move(cb)}{}
	
	operator cmd::command() { return cb; }
};

template<class... Ts>
struct multibase : virtual cb_owner, Ts... {
	multibase(command_builder&& v):cb_owner{std::forward<command_builder>(v)}{};
};

struct members_specifier : virtual cb_owner {
	using cb_owner::cb_owner;
	
    members_specifier member(std::filesystem::path m) {
		cb.members.push_back(m); return {std::move(cb)};
	}

	template<std::ranges::range R>
    members_specifier members(const R& r) { return members(r.begin(), r.end()); }

    template<std::input_iterator It>
    members_specifier members(It begin, It end) {
        cb.members.insert(cb.members.end(), begin, end); return {std::move(cb)}; 
    }

    operator cmd::command() { return cb; }
};

template<class R>
struct verbose_modifier : virtual cb_owner {
	using cb_owner::cb_owner;
	
	R verbosely() {
		cb.operation.modifiers.insert(command_builder::operation_t::verbose);
		return {std::move(cb)};
	}
};

enum class archive_prefix { none, of, to, in, from };

template<archive_prefix Prefix, class R>
struct archive_specifier : virtual cb_owner {
	using cb_owner::cb_owner;
	
    R archive(std::filesystem::path p) requires (Prefix==archive_prefix::none)
	{ cb.archive = p; return {std::move(cb)}; }

    R to_archive(std::filesystem::path p) requires (Prefix==archive_prefix::to) { 
		cb.archive = p;
		cb.operation.modifiers.insert(command_builder::operation_t::create_if_not_exists);
		return {std::move(cb)};
	}
	
    R to_thin_archive(std::filesystem::path p) requires (Prefix==archive_prefix::to) {
		cb.archive = p;
		cb.operation.modifiers.insert(command_builder::operation_t::create_if_not_exists);
		cb.operation.modifiers.insert(command_builder::operation_t::make_thin_archive);
		return {std::move(cb)};
	}

    R of_archive(std::filesystem::path p) requires (Prefix==archive_prefix::of) 
	{ cb.archive = p; return {std::move(cb)}; }
	
    R in_archive(std::filesystem::path p) requires (Prefix==archive_prefix::in)
	{ cb.archive = p; return {std::move(cb)}; }

    R from_archive(std::filesystem::path p) requires (Prefix==archive_prefix::from)
	{ cb.archive = p; return {std::move(cb)}; }
};

using to_archive_members = archive_specifier<archive_prefix::to, members_specifier>;

inline multibase<
	to_archive_members, 
	verbose_modifier<to_archive_members>
>
insert() {
    return {command_builder{{
		command_builder::operation_t::replace,
		{command_builder::operation_t::create_if_not_exists}
	}}};
}

inline archive_specifier<archive_prefix::of, cb_owner>
contents() { return {command_builder{{command_builder::operation_t::display_table}}}; }

inline archive_specifier<archive_prefix::from, members_specifier>
del() { return {command_builder{{command_builder::operation_t::del}}}; }

inline archive_specifier<archive_prefix::none, cb_owner>
index() { return {command_builder{{command_builder::operation_t::index}}}; }

}