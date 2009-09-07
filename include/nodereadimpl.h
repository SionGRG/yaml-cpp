#pragma once

namespace YAML
{
	// implementation for Node::Read
	// (the goal is to call ConvertScalar if we can, and fall back to operator >> if not)
	// thanks to litb from stackoverflow.com
	// http://stackoverflow.com/questions/1386183/how-to-call-a-templated-function-if-it-exists-and-something-else-otherwise/1386390#1386390
	
	template<bool>
	struct read_impl;
	
	// ConvertScalar available
	template<>
	struct read_impl<true> {
		template<typename T>
		static bool read(const Node& node, T& value) {
			return ConvertScalar(node, value);
		}
	};

	// ConvertScalar not available
	template<>
	struct read_impl<false> {
		template<typename T>
		static bool read(const Node& node, T& value) {
			try {
				node >> value;
			} catch(const Exception&) {
				return false;
			}
			return true;
		}
	};
	
	namespace fallback {
		// sizeof > 1
		struct flag { char c[2]; };
		flag Convert(...);
		
		char (& operator,(flag, flag) )[1];
		
		template<typename T>
		void operator,(flag, T const&);
		
		char (& operator,(char(&)[1], flag) )[1];
	}

	template <typename T>
	inline bool Node::Read(T& value) const {
		using namespace fallback;

		return read_impl<sizeof (fallback::flag(), Convert(std::string(), value), fallback::flag()) != 1>::read(*this, value);
	}
	
	// the main conversion function
	template <typename T>
	inline bool ConvertScalar(const Node& node, T& value) {
		std::string scalar;
		if(!node.GetScalar(scalar))
			return false;
		
		return Convert(scalar, value);
	}
}