#pragma once

/* 
			UTILITY FOR SFML GEOMETRICAL OBJECT

*/
#include <filesystem>
#include <fstream>
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

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	//                 String Utility    
	//
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	auto get_first_number(const std::string& str) {
		std::string number;
		bool b = false;
		int i = 0;
		for (auto it = str.begin(); it != str.end(); ) {
			if (std::isdigit(*it)) { number.push_back(*it); b = true; }
			++it; ++i;
			//if (it == str.end()) break;
			if (it == str.end() || (b && !std::isdigit(*it))) {
				b = false;
				break;
			}
		}

		return std::pair(number, i);
	}

	auto get_all_number(std::string str) {
		std::vector<std::string>		vstr;

		while (true) {
			auto pair = get_first_number(str);
			vstr.push_back(pair.first);
			if (pair.second == str.size()) break;
			str = str.substr(pair.second);
		}

		return vstr;
	}

	// Use function when you are 100% now about argument is number int or float point
	int strtoint(const std::string& number) {
		int i = 0;
		for (char c : number) {
			// Checking if the element is digit
			if (c >= '0' && c <= '9') {
				i = i * 10 + (c - '0');
			}
			else
			{
				// bad out put 0
				return 0;
			}
		}

		return i;
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	//                 Structure  
	//
	/////////////////////////////////////////////////////////////////////////////////////////////////////
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

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	//                 Get Number of Processros    
	//
	/////////////////////////////////////////////////////////////////////////////////////////////////////


	bool save_vector_to(const std::vector<int>& maze, const std::string& filename) {

		std::ofstream	file(filename, std::ios::binary);

		if (!file) {
			std::cerr << "Error: Cannot open file for writing : " << filename << '\n';
			return false;
		}

		// 1. write the size of vector
		size_t size = maze.size();
		file.write(reinterpret_cast<const char*>(&size), sizeof(size));

		// 2. write maze data
		file.write(reinterpret_cast<const char*>(maze.data()), size * sizeof(int));

		return file.good();
	}

	bool load_vector_from(std::vector<int>& maze, const std::string& filename) {
		std::ifstream		file(filename, std::ios::binary);

		if (!file) {
			std::cerr << "Error : Cannot open file for reading : " << filename << '\n';
			return false;
		}

		// 1. read size
		size_t size;
		file.read(reinterpret_cast<char*>(&size), sizeof(size));

		// 2. read data to vector
		maze.clear();
		maze.resize(size);

		file.read(reinterpret_cast<char*>(maze.data()), size * sizeof(int));

		return file.good();
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
		return std::string(buf);
	}

	std::string make_current_date() {
		auto stime = make_daytime_string_s();
		return stime.substr(0, 10) + stime.substr(19);
	}

	std::string make_current_time() {
		auto stime = make_daytime_string_s();
		return stime.substr(11, 8);
	}

	std::string make_serial_daytime() {
		// get string like this 07142026123056 
		std::string daytime = make_daytime_string_s();
		std::string serial;
		for (auto& c : daytime) {
			if (std::isdigit(c)) serial.push_back(c);
		}

		return serial;
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

	float sign(float a)
	{
		return a < 0.0f ? -1.0f : 1.0f;
	}

	int sign(int a) {
		return a < 0 ? -1 : 1;
	}


}// Namespace util

template<typename T>
using Coord = util::Coordinate_t<T>;

using iCoord = Coord<int>;
using fCoord = Coord<float>;