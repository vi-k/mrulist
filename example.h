#ifndef EXAMPLE_H
#define EXAMPLE_H

#include <iostream>

class tile_id
{
private:
	int map_id_;
	int z_;
	int x_;
	int y_;

public:
	tile_id()
		: map_id_(0), z_(0), x_(0), y_(0) {}

	tile_id(int map_id, int z, int x, int y)
		: map_id_(map_id), z_(z), x_(x), y_(y) {}

	tile_id(const tile_id &other)
		: map_id_(other.map_id_)
		, z_(other.z_)
		, x_(other.x_)
		, y_(other.y_) {}

	tile_id& operator=(const tile_id &other)
	{
		map_id_ = other.map_id_;
		z_ = other.z_;
		x_ = other.x_;
		y_ = other.y_;
		return *this;
	}
	
	inline bool operator ==(const tile_id &other) const
	{
		return map_id_ == other.map_id_
			&& z_ == other.z_
			&& x_ == other.x_
			&& y_ == other.y_;
	}

	inline bool operator <(const tile_id &other) const
	{
		if (map_id_ != other.map_id_)
			return map_id_ < other.map_id_;
		if (z_ != other.z_)
			return z_ < other.z_;
		if (x_ != other.x_)
			return x_ < other.x_;
	   	return y_ < other.y_;
	}

	friend std::ostream& operator <<(std::ostream& out, const tile_id &t)
	{
		out	<< "{" << t.map_id_
			<< "," << t.z_
			<< "," << t.x_
			<< "," << t.y_ << "}";
		return out;
	}

	friend std::size_t hash_value(const tile_id &t)
	{
		std::size_t seed = 0;
		boost::hash_combine(seed, t.map_id_);
		boost::hash_combine(seed, t.z_);
		boost::hash_combine(seed, t.x_);
		boost::hash_combine(seed, t.y_);

		return seed;
	}

	friend struct ihash; /* Для std::unordered_map */

};

struct ihash : std::unary_function<tile_id, std::size_t>
{
	std::size_t operator()(const tile_id &t) const
	{
		std::size_t seed = 0;
		boost::hash_combine(seed, t.map_id_);
		boost::hash_combine(seed, t.z_);
		boost::hash_combine(seed, t.x_);
		boost::hash_combine(seed, t.y_);
		return seed;
   	}
};

#ifndef TEST_TYPE
#define TEST_TYPE int
#endif

#define TEST_DEBUG 0
class test
{
public:
	TEST_TYPE a;

	void init()
	{
		if (count()++ == 0)
			std::cout << "*** first test::test() ***\n";
		#if TEST_DEBUG
		std::cout << "test(" << a << ") count=" << count() << std::endl;
		#endif
	}

	test() { init(); }
	test(const TEST_TYPE &a) : a(a) { init(); }
	test(const test &t) : a(t.a) { init(); }

	~test()
	{
		if (--count() == 0)
			std::cout << "*** last test::~test() ***\n";
		#if TEST_DEBUG
		std::cout << "~test(" << a << ") count=" << count() << std::endl;
		#endif
	}

	static int& count()
	{
		static int count_ = 0;
		return count_;
	};

	friend std::ostream& operator <<(std::ostream& out, const test &t)
	{
		out	<< t.a;
		return out;
	}
};

#endif
