#pragma once

#include <functional>
#include <initializer_list>
#include <memory>
#include <string>
#include <optional>
#include <filesystem>
#include <vector>
#include <set>
#include "command.hpp"
#include <ranges>
#include <iterator>

namespace ar {

struct operation_t {
	char code;
	std::vector<char> modifiers;
	std::string relpos;
	int m_count = -1;

	auto create_if_not_exists() { modifiers.push_back('c'); return *this; }
	auto create_index() { modifiers.push_back('s'); return *this; }
	auto do_not_create_index() { modifiers.push_back('S'); return *this; }
	auto make_thin_archive() { modifiers.push_back('T'); return *this; }
	auto verbose() { modifiers.push_back('v'); return *this; }
	auto show_version() { modifiers.push_back('V'); return *this; }
	auto deterministic_mode() { modifiers.push_back('D'); return *this; }
	auto truncate_names() { modifiers.push_back('f'); return *this; }
	auto preserve_original_dates() { modifiers.push_back('o'); return *this; }
protected:
	auto after(std::string mem) { modifiers.push_back('a'); relpos = mem; return *this; }
	auto before(std::string mem) { modifiers.push_back('b'); relpos = mem; return *this; }
	auto count(int v) { m_count = v; return *this; }
};

struct del : operation_t {
	del():operation_t{'d'}{}
	using operation_t::count;
};
struct print : operation_t {
	print():operation_t{'p'}{}
};
struct quick_append : operation_t {
	quick_append():operation_t{'q'}{}
};
struct display_table : operation_t {
	display_table():operation_t{'t'}{}
};
struct extract : operation_t {
	extract():operation_t{'x'}{}
	using operation_t::count;
};
struct move : operation_t {
	move():operation_t{'m'}{}
	using operation_t::after;
	using operation_t::before;
};
struct insert : operation_t {
	insert():operation_t{'r'}{}
	using operation_t::after;
	using operation_t::before;
};

struct command_builder {
	using path = std::filesystem::path;
    path archive;
	std::unique_ptr<operation_t> operation;
    std::vector<path> m_members;

	template<class Op>
	command_builder(path archive, Op operation)
	:archive{archive}, operation{make_unique<operation_t>(operation)} {}

	auto& members(std::initializer_list<path> ms) {
		m_members.insert(m_members.end(), ms);
		return *this;
	}

	template<std::ranges::range R>
	auto& members(const R& ms) {
		m_members.insert(m_members.end(), ms.begin(), ms.end());
		return *this;
	}

    operator cmd::command() {
        std::vector<std::string> args;
        
		std::string res{"-"};
		res += operation->code;

        for(auto m : operation->modifiers)
			res += m;

		args.push_back(res);
		if(not operation->relpos.empty())
			args.push_back(operation->relpos);
		if(operation->m_count != -1)
			args.push_back(std::to_string(operation->m_count));
        
		args.push_back(archive.string());

        for(auto& m : m_members) args.push_back(m.string());
        
        return {"ar", args};
    }
};

}