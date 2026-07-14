#pragma once

/* 
			UTILITY FOR SFML GEOMETRICAL OBJECT

*/
#include <filesystem>
#include <string>
#include <vector>
#include <map>
#include <random>
#include <numeric>


#define KA_PI							3.14159265359f

using u64 = uint64_t;
using u32 = uint32_t;
using u16 = uint16_t;
using u8 = uint8_t;
using f64 = double;
using f32 = float;
using i64 = int64_t;
using i32 = int32_t;
using i16 = int16_t;
using i8 = int8_t;

namespace util {

	template<typename T>
	struct Coordinate_t {
		T	x;
		T	y;
	};


	int Bit(int x, int number_bit) {
		return (x >> number_bit) & 1;
	}


	template<typename T>
	void reset(std::vector<T>& vec, const T& value) {
		for (auto& e : vec) e = value;
	}

	template<typename T>
	void clear(std::stack<T>& stack) {
		while (!stack.empty()) stack.pop();
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	//                 Get Number of Processros    
	//
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	/*int GetNumberOfCores(void)
	{

#if defined( _WIN64 )
		SYSTEM_INFO sysinfo;
		GetSystemInfo(&sysinfo);
		return sysinfo.dwNumberOfProcessors;
#elif defined( __APPLE__ )
		return (int)sysconf(_SC_NPROCESSORS_ONLN);
#elif defined( __linux__ )
		return (int)sysconf(_SC_NPROCESSORS_ONLN);
#elif defined( __EMSCRIPTEN__ )
		return (int)sysconf(_SC_NPROCESSORS_ONLN);
#else
		return 1;
#endif
	}
	*/

	template<typename T>
	void clamp(T& x, const T& min, const T& max) {
		x = std::clamp(x, min, max);
	}

	template<typename T>
	requires std::is_integral_v<T>
	void priodic_clamp(T& x, const T& min, const T& max) {
		x = x < min ? max : x > max ? min : x;
	}


	bool is_perfect_division(uint32_t a, uint32_t b) {
		return a % b == 0u;
	}

	void get_right_perfect_division(uint32_t a, uint32_t& b) {
		while (a % b != 0u) {
			++b;
		}
	}


	bool inf(float x, float y, float epsilon) {
		return std::abs(y - x) < epsilon;
	}

	template<typename T>
		requires requires(T x) { x > x; }
	void swap_min_max(T& min, T& max) {
		if (min > max) std::swap(min, max);
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 
	//				Maximum and Minimum  of Updated value inside Main Loop 
	// 
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	template<typename T>
		requires std::is_arithmetic_v<T>
	void get_maximum(T& max, const T& var) {
		max = var > max ? var : max;
	}

	template<typename T>
		requires std::is_arithmetic_v<T>
	void get_minimum(T& min, const T& var) {
		min = var < min ? var : min;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 
	//    Date and Current Time system
	// 
	//////////////////////////////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////// Extract Date and Time System //////////////////////
	std::string make_daytime_string_s() {
		time_t now = time(nullptr);
		char buf[256];
		ctime_s(buf, 256, &now);
		return buf;
	}

	std::string make_current_date() {
		auto stime = make_daytime_string_s();
		return stime.substr(0, 10) + stime.substr(19);
	}

	std::string make_current_time() {
		auto stime = make_daytime_string_s();
		return stime.substr(11, 8);
	}

	/// Update a Function every period of time
	template<typename CLOCK, typename FUNCTION>
	void update_every( CLOCK& clock,  float delta, FUNCTION function) {

		if (clock.getElapsedTime().asSeconds() > delta) {
			function();
			clock.restart();
		}
	}

	// Waiting for period of time
	template<typename CLOCK>
	void waiting(float delta) {

		CLOCK clock;
		while (clock.getElapsedTime().asSeconds() < delta) {

		}

		clock.restart();

	}




	//////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	//                 Get working directory for executable file    
	//
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	inline std::string get_working_dir() {


		return {};
	}



	//////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	//                     Get list of file inside directory 
	//
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	inline std::vector<std::string> get_file_list(
		const std::string& directory,
		const std::string& search = "*.*"
	)
	{



		return {};
	}



	//////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	//                     Interpolation function between two number
	//
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	template<typename T>
	requires std::is_arithmetic_v<T>
	inline T interpolate(float tBegin, float tEnd, T begin_val, T end_val, float tX) {
		return static_cast<T>(
			(((end_val - begin_val) / (tEnd - tBegin)) * (tX - tBegin)) + begin_val);
	}



	//////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	//                     Structure that get name of bool and its need
	//
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	template<typename Tchar>
	struct _XBool {
		bool						_action;
		std::basic_string<Tchar>	_comment;

		void not_() {
			_action = !_action;
		}

		bool operator()() const {
			return _action;
		}

		bool& operator()() {
			return _action;
		}
	};

	using XBool = _XBool<char>;
	using mapBool = std::map<std::string, XBool>;

	template<typename T>
	struct _XValue {
		T							_value;
		std::string					_comment;

		T operator()() const {
			return _value;
		}

		T& operator()() {
			return _value;
		}
	};

	template<typename T>
	using mapValue = std::map<std::string, _XValue<T>>;

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	//                     Random variable and random strings
	//
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	std::string		get_random_string() {

		return {};
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	//              SIMPLE RANDOM NUMBER FUNCTOR GENERATOR.
	//              1. INTEGER VALUE  iRG.
	//              2. FLOAT VALUE    fRG.
	// 
	//////////////////////////////////////////////////////////////////////////////////////////////////////////

	template<typename T>
	T get_random(T min, T max) {
		swap_min_max(min, max);
		static_assert(std::is_integral<T>::value, "T type should be integer type");
		std::random_device rd;
		std::mt19937 engin(rd());
		std::uniform_int_distribution<T> rand(min,max);

		return rand(engin);
	}

	template<typename TF>
	TF get_random_float(TF min, TF max) {
		swap_min_max(min, max);

		static_assert(std::is_floating_point<TF>::value, "T type should be floating point type");
		std::random_device rd;
		std::mt19937 engin(rd());
		std::uniform_real_distribution<TF> rand(min, max);

		return rand(engin);
	}

	template<typename T>
	T get_random_one_of(std::initializer_list<T> list) {
		std::vector<T> v(list);

		return v[get_random(0, v.size() - 1)];
	}

	template<typename T>
	class iRG {
		static_assert(std::is_integral<T>::value, "T type should be integer type");

		std::random_device               rd;
		std::mt19937                     engine;
		std::uniform_int_distribution<T> iRand;

	public:
		iRG() : engine(rd()) {}

		T operator () (T min, T max) {
			if (min > max) std::swap(min, max);
			iRand = std::uniform_int_distribution<T>(min, max);
			return iRand(engine);
		}

	};


	template<typename T>
	class fRG {
		static_assert(std::is_floating_point<T>::value, "T type should be floating point type");
		std::random_device rd;
		std::mt19937 engine;
		std::uniform_real_distribution<T> fRand;

	public:
		fRG() : engine(rd()) {}

		T operator () (T min, T max) {
			if (min > max) std::swap(min, max);
			fRand = std::uniform_real_distribution<T>(min, max);
			return fRand(engine);
		}
	};

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	//                     RANDOM COMPONENTS OF (0,1,2,... N) -> (R1, R2,...,RN)
	//
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	std::vector<int>  make_random_map(int n) {
		util::iRG<int>			irand;

		std::vector<int>		order(n);
		std::iota(order.begin(), order.end(), 0);

		std::vector<int>		random;
		random.reserve(n);

		for (int i = 0; i < n; ++i)
		{
			int k = irand(0, n - 1 - i);
			random.push_back(order[k]);
			order.erase(order.begin() + k);
		}

		return random;
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	//                     CHECK IF VALUE IS CHANGED FROM OLD TO NEW VALUE
	//
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	template<typename T>
	class IsValueChanged {
		T	old_value;

	public:
		IsValueChanged(const T& value) :old_value(value) {}

		bool operator()(T value) {
			if (value == old_value) return false;
			else {
				old_value = value;
				return true;
			}
		}
	};



}// Namespace util

template<typename T>
using Coord = util::Coordinate_t<T>;

using iCoord = Coord<int>;
using fCoord = Coord<float>;