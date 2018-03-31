#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "Primes_List.hh"


using std::cout;
using std::endl;

void writeArray(std::ostream&, const size_t, unsigned long*);
bool isPrime(const unsigned long);

constexpr unsigned long desiredPrime = 2305843009213693951;
constexpr size_t count = 200000;

int main(int argc, const char** argv)
{
	constexpr size_t existingN = sizeof(Utils::detail::_primes_list)/sizeof(unsigned long);
	
	
	std::vector<unsigned long> primes(count);
	if (count < existingN)
	{
		primes.resize(existingN);
	}
	
	//unsigned long primes[count];
	
	//cout << "Starting..." << endl;
	
	static_assert(count >= 2);
	unsigned long last = 3;
	primes[0] = 1;
	primes[1] = 2;
	unsigned long lastPrint = 0;
	int i;
	
	
	for (i = 2; i < existingN; ++i)
	{
		primes[i] = Utils::detail::_primes_list[i];
		last = primes[i];
	}
	
	if (i >= count)
	{
		std::clog << "Done.\n";
		return 0;
	}
	
	bool going = true;
	for (; i < count; ++i)
	{
		while (!isPrime(last))
		{
			++last;
		}
		primes[i] = last;
		last += 2;
		
		if (last - lastPrint > 1000)
		{
			lastPrint = last;
			cout << i << "\t" << primes[i] << endl;
		}
		
		if (last >= desiredPrime)
		{
			++i;
			break;
		}
		
		if (i == primes.size()-1)
		{
			primes.resize(primes.size()*2);
		}
	}
	
	//cout << endl << endl;
	
	
	std::ofstream out("Primes_List.hh");
	
	writeArray(out, i, primes.data());
	out.close();
	std::clog << "Done.\n";
	return 0;
}


void writeArray(std::ostream& out, const size_t n, unsigned long* numbers)
{
	int ind = 0;
	auto indent = [&out, &ind]() -> auto&
	{
		for (int i = 0; i < ind; ++i)
		{
			out << "\t";
		}
		return out;
	};
	
	constexpr const char* includeGuard = "INCLUDED_PRIMES_LIST_HH";
	
	indent() << "#ifndef " << includeGuard << endl;
	indent() << "#define " << includeGuard << endl << endl;
	//indent() << "#include <Common.h>" << endl << endl;
	indent() << "namespace Utils" << endl;
	indent() << "{" << endl;
	++ind;
	indent() << "namespace detail" << endl;
	indent() << "{" << endl << endl;
	
	++ind;
	
	indent() << "constexpr unsigned long _primes_list[] = {" << endl;
	++ind;
	indent();
	for (int i = 0; i < n - 1; ++i)
	{
		out << numbers[i] << ",";
	}
	out << numbers[n-1] << endl;
	
	
	--ind;
	indent() << "};" << endl << endl;
	
	--ind;
	
	
	
	indent() << "}" << endl;
	--ind;
	indent() << "}" << endl << endl;
	indent() << "#endif" << std::flush;
}



bool isPrime(const unsigned long n)
{
	for (int i = 2; i < n/2; ++i)
	{
		if (n % i == 0)
		{
			return false;
		}
	}
	return true;
}